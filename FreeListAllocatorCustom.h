#pragma once
#include "FixedAllocator.h"

// Not an Abstract class
class FreeListAllocator : public FixedAllocator
{
public:
    FreeListAllocator(const std::size_t sizeBytes, void* start) noexcept;

    FreeListAllocator(const FreeListAllocator&) = delete;
    FreeListAllocator& operator=(const FreeListAllocator&) = delete;

    FreeListAllocator(FreeListAllocator&&) noexcept;
    FreeListAllocator& operator=(FreeListAllocator&&) noexcept;

    ~FreeListAllocator() noexcept override final; // = default;

    virtual void* Allocate(const std::size_t& size, const std::uintptr_t& alignment = sizeof(std::intptr_t)) override final;
    virtual void Free(void* const ptr) noexcept override final;
    void ZeroedAddresses(std::uint8_t* ptr_addr, std::uint8_t* zero_addr) noexcept;

    template<typename T>
    inline std::size_t align_forward_adjustment_with_header(const void* const ptr, const std::size_t& alignment) noexcept;
    inline void* ptr_add(const void* const p, const std::uintptr_t& amount) noexcept;
    inline void* ptr_sub(const void* const ptr, const std::uintptr_t& sizeHeader) noexcept;

private:
    struct FreeBlock;
    struct AllocationHeader;

protected:
    FreeBlock* m_freeBlocks;
};


struct FreeListAllocator::FreeBlock {
    int size;
    FreeBlock* next;
    FreeBlock* prev;
};


struct FreeListAllocator::AllocationHeader {
    int size;
    uintptr_t adjustment;
};


FreeListAllocator::FreeListAllocator(const std::size_t sizeBytes, void* start) noexcept
    :
    FixedAllocator(sizeBytes, start), m_freeBlocks((FreeBlock*)start)
{
    assert(sizeBytes > sizeof(FreeBlock));
    m_freeBlocks->size = sizeBytes;
    m_freeBlocks->next = nullptr;
    m_freeBlocks->prev = nullptr;
}


FreeListAllocator::FreeListAllocator(FreeListAllocator&& other) noexcept
    :
    FixedAllocator(std::move(other)),
    m_freeBlocks(other.m_freeBlocks)
{
    other.m_freeBlocks = nullptr;
}


FreeListAllocator& FreeListAllocator::operator=(FreeListAllocator&& rhs) noexcept
{
    if (this != &rhs) {
        Allocator::operator=(std::move(rhs));
        m_freeBlocks = rhs.m_freeBlocks;
        rhs.m_freeBlocks = nullptr;
    }

    return *this;
}


FreeListAllocator::~FreeListAllocator() noexcept
{
    printf("Destructor called. Allocations left: %zu, Used bytes: %zu\n", m_numAllocations, m_usedBytes);
    assert(m_numAllocations == 0 && m_usedBytes == 0);
}


// Defensive programming style, essentially in colaescing operations
void* FreeListAllocator::Allocate(const std::size_t& size, const std::uintptr_t& alignment)
{
    FreeBlock* freeBlock = m_freeBlocks;
    FreeBlock* bestFit = nullptr;
    std::size_t bestFitTotalSize = 0;
    std::uintptr_t bestFitAdjustment = 0;

    while (freeBlock != nullptr)
    {
        std::uintptr_t adjustment = align_forward_adjustment_with_header<AllocationHeader>(freeBlock, alignment);
        std::size_t totalSize = size + adjustment + sizeof(AllocationHeader);

        if (freeBlock->size > totalSize && (bestFit == nullptr || freeBlock->size < bestFit->size))
        {
            // Defensive pointer operations samples
            if (freeBlock->next != nullptr)
                freeBlock->next->prev = freeBlock;
            if (freeBlock->prev != nullptr)
                freeBlock->prev->next = freeBlock;

            bestFit = freeBlock;
            bestFitAdjustment = adjustment;
            bestFitTotalSize = totalSize;
        }

        freeBlock = freeBlock->next;
    }

    if (bestFit == nullptr)
        throw std::bad_alloc();

    if (bestFit->size <= bestFitTotalSize + sizeof(AllocationHeader))
    {
        bestFitTotalSize = bestFit->size;

        if (bestFit->prev != nullptr)
            bestFit->prev->next = bestFit->next;
        else
            m_freeBlocks = bestFit->next;

        if (bestFit->next != nullptr)
            bestFit->next->prev = bestFit->prev;
    }
    else
    {
        FreeBlock* newBlock = reinterpret_cast<FreeBlock*>(ptr_add(bestFit, bestFitTotalSize));
        newBlock->size = bestFit->size - bestFitTotalSize;
        newBlock->next = bestFit->next;
        newBlock->prev = bestFit->prev;

        if (bestFit->next != nullptr)           
            bestFit->next->prev = newBlock;
        if (bestFit->prev != nullptr)            
            bestFit->prev->next = newBlock;
        else
            m_freeBlocks = newBlock;

        
    }

    std::uintptr_t alignedAddr = reinterpret_cast<std::uintptr_t>(bestFit) + bestFitAdjustment;
    AllocationHeader* header = reinterpret_cast<AllocationHeader*>(alignedAddr - sizeof(AllocationHeader));
    header->adjustment = bestFitAdjustment;
    header->size = bestFitTotalSize;

    m_usedBytes += bestFitTotalSize;
    ++m_numAllocations;

    return reinterpret_cast<void*>(alignedAddr);
}


void FreeListAllocator::Free(void* const ptr) noexcept
{
    assert(ptr != nullptr);

    AllocationHeader* header = reinterpret_cast<AllocationHeader*>(ptr_sub(ptr, sizeof(AllocationHeader)));
    std::uintptr_t blockStart = reinterpret_cast<std::uintptr_t>(ptr) - header->adjustment;
    std::size_t blockSize = header->size;
    std::uintptr_t blockEnd = blockStart + blockSize;

    FreeBlock* prevFreeBlock = nullptr;
    FreeBlock* freeBlock = m_freeBlocks;

    while (freeBlock != nullptr && reinterpret_cast<std::uintptr_t>(freeBlock) < blockStart)
    {
        prevFreeBlock = freeBlock;
        freeBlock = freeBlock->next;
    }

    FreeBlock* newBlock = reinterpret_cast<FreeBlock*>(blockStart);
    newBlock->size = blockSize;
    newBlock->next = freeBlock;
    newBlock->prev = prevFreeBlock;

    if (newBlock->prev != nullptr &&
        reinterpret_cast<std::uintptr_t>(newBlock->prev) + newBlock->prev->size == reinterpret_cast<std::uintptr_t>(newBlock))
    {
        newBlock->prev->size += newBlock->size;
        newBlock->prev->next = newBlock->next;

        if (newBlock->next)
        {
            newBlock->next->prev = newBlock->prev;
        }

        newBlock = newBlock->prev;
    }

    if (newBlock->next != nullptr &&
        reinterpret_cast<std::uintptr_t>(newBlock) + newBlock->size == reinterpret_cast<std::uintptr_t>(newBlock->next))
    {
        newBlock->size += newBlock->next->size;
        newBlock->next = newBlock->next->next;

        if (newBlock->prev)
        {
            newBlock->prev->next = newBlock;
        }
    }

    if (newBlock->next)
        newBlock->next->prev = newBlock;
    if (newBlock->prev)
        newBlock->prev->next = newBlock;
    else
        m_freeBlocks = newBlock;

    std::uint8_t* start = reinterpret_cast<std::uint8_t*>(blockStart + header->adjustment);
    std::uint8_t* end = reinterpret_cast<std::uint8_t*>(blockStart + blockSize);
    ZeroedAddresses(start, end);

    --m_numAllocations;
    m_usedBytes -= blockSize;
}


template<typename T>
inline std::size_t FreeListAllocator::align_forward_adjustment_with_header(const void* const ptr, const std::size_t& alignment) noexcept      // ptr - could be declared like std::uintptr_t
{
    const auto iptr = reinterpret_cast<std::uintptr_t>(ptr);
    const auto aligned = (iptr + (alignment - 1u)) & ~(alignment - 1u);
    std::size_t adjustment = aligned - iptr;

    constexpr std::size_t required = sizeof(T);

    if (adjustment < required)
    {
        std::size_t neededSpace = required - adjustment;
        adjustment += alignment * ((neededSpace + alignment - 1u) / alignment);
    }

    return adjustment;
}


inline void* FreeListAllocator::ptr_add(const void* const p, const std::uintptr_t& amount) noexcept
{
    return reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(p) + amount);
}


inline void* FreeListAllocator::ptr_sub(const void* const ptr, const std::uintptr_t& sizeHeader) noexcept
{
    return reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(ptr) - sizeHeader);
}


void FreeListAllocator::ZeroedAddresses(std::uint8_t* ptr_addr, std::uint8_t* zero_addr) noexcept
{
    while (ptr_addr < zero_addr)
        *ptr_addr++ = 0;
    // iptr_ptr_addr = izero_zero_addr;
}