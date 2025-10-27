#pragma once
#include <iostream>
#include <type_traits>
#include "FixedAllocator.h"
#include "DynamicAllocator.h"

template<typename T, typename Alloc>
class STLAdaptor
{
public:

    typedef T value_type;


    STLAdaptor() = delete;


    STLAdaptor(Alloc& allocator) noexcept
        :
        m_allocator(allocator)
    {}


    template<typename U>
    STLAdaptor(const STLAdaptor<U, Alloc>& other) noexcept
        :
        m_allocator(other.m_allocator)
    {}


    [[nodiscard]] constexpr T* allocate(std::size_t n)
    {
        printf("number of n * sizeof(T): %zu * %zu\n", n, sizeof(T));
        return reinterpret_cast<T*>
            (m_allocator.Allocate(n * sizeof(T), alignof(T)));
    }


    constexpr void deallocate(T* p, [[maybe_unused]] std::size_t n)
        noexcept
    {
        printf("Deallocation <-- STLAdapt: %p\n", p);
        m_allocator.Free(p);
    }

    std::size_t MaxAllocationSize() const noexcept
    {
        return m_allocator.GetSize();
    }


    bool operator==(const STLAdaptor<T, Alloc>& rhs) const noexcept
    {
        if constexpr (std::is_base_of_v<FixedAllocator, Alloc>)
        {
            return m_allocator.GetStart() == rhs.m_allocator.GetStart();
        }
        else
        {
            const auto* left = dynamic_cast<const DynamicAllocator*>(&m_allocator);
            const auto* right = dynamic_cast<const DynamicAllocator*>(&rhs.m_allocator);

            if (!left || !right)
            {
                return false;
            }

            DynamicAllocator::BlockDesc* a = left->m_currentBlock;

            while (a->prevBlock != nullptr)
            {
                a = a->prevBlock;
            }

            DynamicAllocator::BlockDesc* b = right->m_currentBlock;
            while (b->prevBlock != nullptr)
            {
                b = b->prevBlock;
            }

            return a == b;
        }
    }


    bool operator!=(const STLAdaptor<T, Alloc>& rhs) const noexcept
    {
        return !(*this == rhs);
    }


    Alloc& m_allocator;
};
