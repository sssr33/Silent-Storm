# Deferred runtime bugs

Preserve the original game code and behavior as closely as possible while
restoring the build. Record existing bugs here instead of fixing them as part of
compiler compatibility work. Investigate them on a working game before changing
behavior, then make focused fixes with checks for gameplay regressions.

## BUG-001: HomogeneousInverse reports failure after successful inversion

**Recorded:** 2026-10-03

**Status:** Deferred. Confirmed by source inspection; gameplay impact has not
been tested on a running game. The implementation remains unchanged.

### Evidence

The primary implementation is `Soft/Andy/Jan03/a5dll/Misc/Geom.h`, in
`SHMatrix::HomogeneousInverse()` (lines 726-754 at the time of recording).

- If the determinant of the upper-left 3x3 matrix is zero, the function returns
  `false` before modifying the destination matrix.
- Otherwise, it computes the inverse rotation/linear part and translation,
  writes the homogeneous bottom row, and still returns `false` at line 753.
- The function body already contains this behavior in the original source
  import, commit `243de3dd3`. It was not introduced by the recent `SHMatrix`
  constructor or Visual Studio compatibility changes.

The function body and caller below indicate an intended success/failure
contract: `true` for successful inversion and `false` for a singular matrix.
Whether the existing behavior was relied on for gameplay tuning is unknown.

### Where the return value matters

There are ten calls in the primary Main source tree. Nine ignore the boolean
result. One checks it in `CASphereSet::Init()`, in
`Soft/Andy/Jan03/a5dll/Main/GAnimParticles.cpp:1061`:

```cpp
if ( bBrick || !inertiaInvBody.HomogeneousInverse( inertia ) )
```

The preceding code calculates inertia from the masses and positions of the
object's spheres, treated as particles. The conditional block replaces that
matrix with the inertia of a rectangular block using the object's bounding
dimensions. Since inversion always reports failure, the block approximation
always replaces the particle-derived inertia, even when inversion succeeded.

Changing only the final return value would affect this selection:

| Initialization case | Current behavior | With a successful inversion returning `true` |
| --- | --- | --- |
| `bBrick == false`, invertible particle-derived inertia | Use block approximation | Keep particle-derived inertia |
| `bBrick == false`, singular particle-derived inertia | Use block approximation | Use block approximation |
| `bBrick == true` | Use block approximation | Use block approximation |

`bBrick` is explicitly set for `bMassCenter == true` and for objects whose
bounding-dimension ratios exceed 20. The grenade caller in
`Main/wGrenade.cpp:92` passes `true`; debris initialization in
`Main/wDebris.cpp:176` passes `false`. These paths use the primary source root
`Soft/Andy/Jan03/a5dll/`.

The resulting inverse inertia feeds angular velocity in
`Main/GAnimParticles.cpp:1317` (`CalcRotVel`) and collision impulse calculations
at line 1280 (`ApplyCollision`). A return-value correction can therefore change
debris rotation, collision response, and settling. Explicit block selection for
grenades and elongated objects would remain in place. The other nine callers
would receive the same computed matrices if only the final return were changed.

### Validation before a future fix

- Establish a working-game baseline with the current behavior and reproducible
  physics scenarios, controlling random inputs where possible.
- Compare debris with different sphere distributions, singular inertia cases,
  grenades, and elongated objects. Check rotation, collision response, and
  settling against the baseline before deciding whether to change the behavior.
- Add standalone checks for a successful inversion returning `true`, a singular
  input returning `false` without changing the destination, and the computed
  inverse. Keep these under `Tests/Small/`.
- Make any behavior correction a separate, focused change after runtime
  validation; do not mix it into compiler compatibility fixes.

`Tests/Small/MatrixInitialization.cpp` currently checks the inverse matrix and
the forward/inverse product, but deliberately ignores the boolean result. Its
passing checks do not validate the return contract or the physics behavior.
