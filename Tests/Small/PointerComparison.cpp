#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <list>
#include <new>
#include <type_traits>
#include <vector>

#define ASSERT(condition) assert(condition)
#include "../../Soft/Andy/Jan03/a5dll/Misc/Basic2.h"

namespace {

int failures = 0;
int checks = 0;
int destroyed = 0;
int invalidated = 0;

void check(bool condition, const char* description) {
    ++checks;
    if (!condition) {
        ++failures;
        std::printf("FAIL: %s\n", description);
    }
}

class TestObject : public CObjectBase {
public:
    int references() const { return nRefData; }
    int owners() const { return nObjData; }
    ~TestObject() { ++destroyed; }

protected:
    void DestroyContents() override { ++invalidated; }
};

static_assert(sizeof(CPtr<TestObject>) == sizeof(TestObject*), "CPtr layout");
static_assert(sizeof(CObj<TestObject>) == sizeof(TestObject*), "CObj layout");
static_assert(sizeof(CMObj<TestObject>) == sizeof(TestObject*), "CMObj layout");
static_assert(alignof(CPtr<TestObject>) == alignof(TestObject*), "CPtr alignment");
static_assert(std::is_convertible<CPtr<TestObject>, TestObject*>::value,
              "Preserve implicit pointer conversion");

template<template<class> class TPointer>
void checkPointerKind(const char* kind) {
    using Pointer = TPointer<TestObject>;
    const int destroyedBefore = destroyed;
    const int invalidatedBefore = invalidated;
    {
        Pointer first = new TestObject;
        Pointer second = new TestObject;
        Pointer empty;
        const Pointer& constant = first;
        TestObject* raw = first.GetPtr();
        TestObject* const fixedRaw = raw;
        const TestObject* readOnly = raw;
        TestObject* nullRaw = nullptr;
        const TestObject* nullReadOnly = nullptr;
        const int refsBefore = raw->references();
        const int ownersBefore = raw->owners();

        check(first == raw && !(first != raw), "Mutable wrapper and raw pointer");
        check(constant == fixedRaw && !(constant != fixedRaw), "Const wrapper and T* const");
        check(first == readOnly && !(constant != readOnly), "Pointer to const object");
        check(raw == constant && !(raw != constant), "Raw pointer on the left");
        check(readOnly == constant && !(readOnly != constant), "Const raw pointer on the left");
        check(first != second && !(first == second), "Different wrapped addresses");
        check(constant == first && !(constant != first), "Same wrapped address");
        check(first != second.GetPtr() && !(first == second.GetPtr()), "Different raw address");
        check(empty == nullRaw && !(empty != nullRaw), "Typed null pointer");
        check(empty == nullReadOnly && !(empty != nullReadOnly), "Const typed null pointer");
        check(empty == 0 && !(empty != 0), "Integer null constant");
        check(empty == NULL && !(empty != NULL), "NULL macro");
        check(empty == nullptr && !(empty != nullptr), "nullptr");
        check(first != 0 && first != nullptr && !(first == 0), "Non-null versus null constants");
        check(0 == empty && nullptr == empty && nullptr != constant, "Null constant on the left");

        // Keep the original public const-pointer comparison signatures available.
        using ConstComparison = bool (Pointer::*)(const TestObject*) const;
        const ConstComparison equal = static_cast<ConstComparison>(&Pointer::operator==);
        const ConstComparison unequal = static_cast<ConstComparison>(&Pointer::operator!=);
        check((constant.*equal)(readOnly) && !(constant.*unequal)(readOnly),
              "Original const-pointer comparison overloads");
        check(raw->references() == refsBefore && raw->owners() == ownersBefore,
              "Comparisons preserve reference and owner counts");
        check(destroyed == destroyedBefore && invalidated == invalidatedBefore,
              "Comparisons preserve object lifetime");

        std::vector<Pointer> pointers{first, second, first};
        check(std::find(pointers.begin(), pointers.end(), raw) == pointers.begin(),
              "std::find in mutable vector with T*");
        const std::vector<Pointer>& constPointers = pointers;
        check(std::find(constPointers.begin(), constPointers.end(), raw) == constPointers.begin(),
              "std::find in const vector with T*");
        check(std::count(constPointers.begin(), constPointers.end(), raw) == 2,
              "std::count with repeated addresses");
        check(std::find(constPointers.begin(), constPointers.end(), readOnly) == constPointers.begin(),
              "std::find with const T*");
        check(std::find(constPointers.begin(), constPointers.end(), nullRaw) == constPointers.end(),
              "std::find missing null pointer");

        std::list<Pointer> linked{first, second};
        const std::list<Pointer>& constLinked = linked;
        check(std::find(constLinked.begin(), constLinked.end(), raw) == constLinked.begin(),
              "std::find in const list with T*");
        const int refsWithContainers = raw->references();
        const int ownersWithContainers = raw->owners();
        (void)std::find(constPointers.begin(), constPointers.end(), raw);
        (void)std::count(constLinked.begin(), constLinked.end(), raw);
        check(raw->references() == refsWithContainers && raw->owners() == ownersWithContainers,
              "STL comparisons preserve reference and owner counts");

        pointers.erase(std::remove(pointers.begin(), pointers.end(), raw), pointers.end());
        check(pointers.size() == 1 && pointers.front() == second,
              "std::remove erases all matching addresses and retains the other object");
        linked.remove(raw);
        check(linked.size() == 1 && linked.front() == second,
              "list::remove with T*");
        check(destroyed == destroyedBefore && invalidated == invalidatedBefore,
              "Container removal keeps externally held objects alive");
    }
    check(destroyed == destroyedBefore + 2 && invalidated == invalidatedBefore,
          "Original last-reference destruction");
    std::printf("Checked %s comparisons and lifetime\n", kind);
}

} // namespace

int main() {
    checkPointerKind<CPtr>("CPtr");
    checkPointerKind<CObj>("CObj");
    checkPointerKind<CMObj>("CMObj");
    const int destroyedBefore = destroyed;
    const int invalidatedBefore = invalidated;
    CObj<TestObject> owner = new TestObject;
    CPtr<TestObject> reference = owner.GetPtr();
    TestObject* raw = reference.GetPtr();
    owner = 0;
    check(!IsValid(reference) && reference.GetPtr() == raw,
          "Dropping the last owner preserves an invalidated weak reference");
    check(reference == raw && reference != nullptr,
          "Invalidated references still compare stored addresses");
    check(destroyed == destroyedBefore && invalidated == invalidatedBefore + 1,
          "Invalidated object stays alive while weakly referenced");
    reference = 0;
    check(destroyed == destroyedBefore + 1 && invalidated == invalidatedBefore + 1,
          "Clearing the last invalidated reference destroys the object once");
    std::printf("%d checks, %d failures\n", checks, failures);
    return failures ? 1 : 0;
}
