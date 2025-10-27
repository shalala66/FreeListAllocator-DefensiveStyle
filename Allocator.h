#pragma once

#include <cstddef>
#include <cstdint>
#include <cassert>

// Abstract class
class Allocator
{
public:
    Allocator(const std::size_t sizeBytes, void* const start) noexcept;

    Allocator(const Allocator&) = delete;
    Allocator& operator=(Allocator&) = delete;

    Allocator(Allocator&&) noexcept;
    Allocator& operator=(Allocator&&) noexcept;

    virtual ~Allocator() noexcept;

    // pure virtual methods
    virtual void* Allocate(const std::size_t& size, const std::uintptr_t& alignment = sizeof(std::intptr_t)) = 0;    // size could be defined like a macros
    virtual void Free(void* const ptr) = 0;      // Deallocate

    const std::size_t& GetSize() const noexcept;
    const std::size_t& GetUsed() const noexcept;
    const std::size_t& GetNumAllocation() const noexcept;

    const void* GetStart() const noexcept;

protected:
    std::size_t m_size;
    std::size_t m_usedBytes;
    std::size_t m_numAllocations;

    void* m_start;
};


Allocator::Allocator(const std::size_t sizeBytes, void* const start) noexcept
    :
    m_size(sizeBytes),
    m_usedBytes(0),
    m_numAllocations(0),
    m_start(start)
{
    assert(sizeBytes > 0);
}

Allocator::Allocator(Allocator&& other) noexcept
    :
    m_size(other.m_size),
    m_usedBytes(other.m_usedBytes),
    m_numAllocations(other.m_numAllocations),
    m_start(other.m_start)
{
    other.m_start = nullptr;
    other.m_size = 0;
    other.m_numAllocations = 0;
    other.m_usedBytes = 0;
}

Allocator& Allocator::operator=(Allocator&& rhs) noexcept
{
    m_size = rhs.m_size;
    m_usedBytes = rhs.m_usedBytes;
    m_numAllocations = rhs.m_numAllocations;
    m_start = rhs.m_start;

    rhs.m_start = nullptr;
    rhs.m_size = 0;
    rhs.m_numAllocations = 0;
    rhs.m_usedBytes = 0;

    return *this;
}

Allocator::~Allocator() noexcept
{
    assert(m_numAllocations == 0 && m_usedBytes == 0);
}

const std::size_t& Allocator::GetSize() const noexcept
{
    return m_size;
}

const std::size_t& Allocator::GetUsed() const noexcept
{
    return m_usedBytes;
}

const std::size_t& Allocator::GetNumAllocation() const noexcept
{
    return m_numAllocations;
}

const void* Allocator::GetStart() const noexcept
{
    return m_start;
}
