#pragma once
#include "Allocator.h"

class FixedAllocator : public Allocator {
public:
    FixedAllocator(std::size_t sizeBytes, void* start)
        : Allocator(sizeBytes, start) {}
};
