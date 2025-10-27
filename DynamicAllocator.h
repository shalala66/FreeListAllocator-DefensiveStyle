#pragma once
#include "Allocator.h"

class DynamicAllocator : public Allocator {
public:
    struct BlockDesc {
        BlockDesc* prevBlock;
    };

    DynamicAllocator(std::size_t sizeBytes, void* start)
        : Allocator(sizeBytes, start) {
        m_currentBlock = reinterpret_cast<BlockDesc*>(start);
        m_currentBlock->prevBlock = nullptr;
    }

    BlockDesc* m_currentBlock;
};
