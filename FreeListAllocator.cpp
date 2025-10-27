#include <iostream>
#include <vector>
#include "STLAdaptor.h"
#include "FreeListAllocatorCustom.h"
#include "DynamicAllocator.h"

int main() {
    const std::size_t memSize = 300;
    void* memory = std::malloc(memSize);

    FreeListAllocator myAlloc(memSize, memory);

    STLAdaptor<int, FreeListAllocator> a(myAlloc);
    STLAdaptor<int, FreeListAllocator> b(myAlloc);

    if (a == b) {
        std::cout << "Same allocator!\n";
    }
    else {
        std::cout << "Different allocator!\n";
    }

    std::vector<int, STLAdaptor<int, FreeListAllocator>> vec(a);
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    return 0;
}
