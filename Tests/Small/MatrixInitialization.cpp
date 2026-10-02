#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <new>
#include <type_traits>

// The legacy tools.h included by Geom.h expects unqualified STL algorithms.
using namespace std;
#define ASSERT(condition) assert(condition)
#include "../../Soft/Andy/Jan03/a5dll/Misc/Geom.h"

static_assert(sizeof(SHMatrix) == 64, "Matrix layout");
static_assert(sizeof(SFBTransform) == 128, "Transform layout");
static_assert(alignof(SHMatrix) == 4, "Matrix alignment");
static_assert(alignof(SFBTransform) == 4, "Transform alignment");
static_assert(offsetof(SFBTransform, backward) == 64, "Backward matrix offset");
static_assert(is_default_constructible<SHMatrix>::value, "Matrix default construction");
static_assert(is_default_constructible<SFBTransform>::value, "Transform default construction");
static_assert(is_trivially_copyable<SHMatrix>::value, "Matrix copy semantics");
static_assert(is_trivially_copyable<SFBTransform>::value, "Transform copy semantics");
static_assert(is_standard_layout<SHMatrix>::value, "Matrix standard layout");
static_assert(is_standard_layout<SFBTransform>::value, "Transform standard layout");

namespace {

// Exercise the same value-member construction that failed in DG.h.
template<class TResult> struct ValueNode {
    TResult value;
    ValueNode() {}
};

int failures = 0;
int checks = 0;

void check(bool condition, const char* description) {
    ++checks;
    if (!condition) {
        ++failures;
        std::printf("FAIL: %s\n", description);
    }
}

bool zero(const SHMatrix& matrix) {
    const unsigned char zeroBytes[sizeof(SHMatrix)] = {};
    return std::memcmp(&matrix, zeroBytes, sizeof(matrix)) == 0;
}

bool zero(const SFBTransform& transform) {
    return zero(transform.forward) && zero(transform.backward);
}

bool same(const SHMatrix& left, const SHMatrix& right) {
    return std::memcmp(&left, &right, sizeof(left)) == 0;
}

} // namespace

int main() {
    SHMatrix defaultMatrix;
    SHMatrix braceMatrix{};
    SHMatrix matrixArray[3];
    check(zero(defaultMatrix), "SHMatrix default initialization");
    check(zero(braceMatrix), "SHMatrix brace initialization");
    for (const SHMatrix& matrix : matrixArray)
        check(zero(matrix), "SHMatrix array initialization");

    alignas(SHMatrix) unsigned char matrixStorage[sizeof(SHMatrix)];
    std::memset(matrixStorage, 0xA5, sizeof(matrixStorage));
    SHMatrix* placedMatrix = new (matrixStorage) SHMatrix;
    check(zero(*placedMatrix), "SHMatrix overwrites prefilled storage");
    placedMatrix->~SHMatrix();

    SFBTransform defaultTransform;
    SFBTransform braceTransform{};
    SFBTransform transformArray[3];
    ValueNode<SFBTransform> node;
    check(zero(defaultTransform), "SFBTransform implicit default constructor");
    check(zero(braceTransform), "SFBTransform brace initialization");
    check(zero(node.value), "CFuncBase-style member initialization");
    for (const SFBTransform& transform : transformArray)
        check(zero(transform), "SFBTransform array initialization");

    alignas(SFBTransform) unsigned char transformStorage[sizeof(SFBTransform)];
    std::memset(transformStorage, 0xA5, sizeof(transformStorage));
    SFBTransform* placedTransform = new (transformStorage) SFBTransform;
    check(zero(*placedTransform), "SFBTransform overwrites prefilled storage");
    placedTransform->~SFBTransform();

    SHMatrix identity;
    Identity(&identity);
    check(identity._11 == 1 && identity._22 == 1 &&
          identity._33 == 1 && identity._44 == 1 && identity._14 == 0,
          "Explicit identity setup");

    SHMatrix translated;
    MakeMatrix(&translated, CVec3(1, 2, 3), CQuat(0, 0, 0, 1));
    check(translated._14 == 1 && translated._24 == 2 && translated._34 == 3,
          "Translation setup");
    check(same(identity * translated, translated), "Matrix multiplication");

    SHMatrix matrixCopy(translated);
    SHMatrix matrixAssigned;
    matrixAssigned = translated;
    check(same(matrixCopy, translated), "Matrix copy construction");
    check(same(matrixAssigned, translated), "Matrix copy assignment");

    SFBTransform transform;
    transform.forward = translated;
    // The existing implementation returns false even after calculating an inverse.
    // Check the result independently of that unrelated return-value defect.
    transform.backward.HomogeneousInverse(transform.forward);
    check(transform.backward._14 == -1 && transform.backward._24 == -2 &&
          transform.backward._34 == -3 && transform.backward._44 == 1,
          "Inverse calculation");
    check(same(transform.forward * transform.backward, identity), "Forward/inverse product");

    SFBTransform transformCopy(transform);
    SFBTransform transformAssigned;
    transformAssigned = transform;
    check(std::memcmp(&transformCopy, &transform, sizeof(transform)) == 0,
          "Transform copy construction");
    check(std::memcmp(&transformAssigned, &transform, sizeof(transform)) == 0,
          "Transform copy assignment");

    std::printf("Checks: %d; failures: %d; matrix bytes: %zu; transform bytes: %zu\n",
                checks, failures, sizeof(SHMatrix), sizeof(SFBTransform));
    return failures ? 1 : 0;
}
