# FreeListAllocator (Defensive Style)
Free-List Allocator in Defensive Programming Style


## Overview

`FreeListAllocator (Defensive Style)` is a custom memory allocator designed to ensure robust memory management in C++ through defensive programming techniques. This allocator implements a free-list allocation strategy and includes comprehensive checks to prevent memory errors such as leaks, invalid access, and buffer overflows. It provides safety and stability in the allocation and deallocation of memory blocks.

The allocator supports various memory management operations, with an emphasis on error handling, alignment, and freeing unused memory in an optimized manner.

## Features

- **Defensive programming**: Prevents common memory errors by ensuring valid access, boundary checks, and handling corner cases.
- **Free-list allocation**: Efficient management of memory through the free list technique.
- **Allocator class hierarchy**: Supports multiple allocator types, including fixed and dynamic allocation schemes.
- **STL Adapter**: Integrates seamlessly with STL containers using custom allocators.
- **Eliminate Undefined Behaviors**: Handling total size calculation without UB, alignment ddjustment without UB, freeing memory blocks with care.

## Usage

To use the `FreeListAllocator`, include the header file and create an instance of the allocator:

```cpp
#include "FreeListAllocatorCustom.h"

int main() {
    const std::size_t memSize = 300;
    void* memory = std::malloc(memSize);

    FreeListAllocator myAlloc(memSize, memory);

    STLAdaptor<int, FreeListAllocator> a(myAlloc);
    STLAdaptor<int, FreeListAllocator> b(myAlloc);

    if (a == b) {
        std::cout << "Same allocator!\n";
    } else {
        std::cout << "Different allocator!\n";
    }

    std::vector<int, STLAdaptor<int, FreeListAllocator>> vec(a);
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    return 0;
}
```

## Requirements

- **C++17 or higher**

- **Standard C++ library**


## Design

### Allocators

1. **Allocator (Abstract)**: 
   - The base class for all memory allocators. It defines the common interface for memory management functions, such as `Allocate` and `Free`. Derived classes must implement these functions to handle specific memory allocation strategies.
   
2. **FixedAllocator**: 
   - A fixed-size memory allocator that allocates a predefined block of memory. It cannot resize, so it is ideal for use in scenarios where memory size is known in advance and remains constant.

3. **DynamicAllocator**: 
   - A dynamic memory allocator that manages a memory pool and supports more complex memory allocation schemes. This allocator includes block descriptions for efficient memory management and is capable of growing or shrinking memory blocks as required.

4. **FreeListAllocator**: 
   - A custom allocator that extends `FixedAllocator` and implements a free-list allocation strategy. It manages memory by maintaining a list of free blocks, which can be reused. It is designed to reduce fragmentation and optimize memory usage.

### Defensive Features

1. **Boundary checks and pointer safety**: 
   - The allocator includes checks to ensure that memory accesses stay within allocated boundaries. This prevents buffer overflows and out-of-bounds access, which are common sources of memory errors.

2. **Memory fragmentation prevention**: 
   - The allocator attempts to minimize fragmentation by efficiently managing free memory blocks. It coalesces adjacent free blocks to form larger contiguous blocks, helping reduce wasted space and improve memory utilization.

3. **Zeroing out freed memory**: 
   - Freed memory is zeroed out to prevent access to stale data, ensuring that no sensitive information remains in memory after deallocation. This is particularly important for preventing security vulnerabilities such as data leaks.
  

# Understanding `std::vector` and Allocators in C++

## Vector Class Declaration

```cpp
template<
    class T,
    class Allocator = std::allocator<T>
> 
class vector;
```
Source:
[🔗 cppreference: std::vector](https://en.cppreference.com/w/cpp/container/vector)


## Default Allocator: `std::allocator`

The `std::allocator` class template serves as the default allocator used by all standard library containers when no custom allocator is specified.
It is stateless, meaning that all instances of a given allocator type are interchangeable, compare equal, and are capable of deallocating memory allocated by any other instance of the same type.

Source:
[🔗 cppreference: std::allocator](https://en.cppreference.com/w/cpp/memory/allocator)


## Allocator Overview

An allocator is a mechanism used to obtain and release memory, as well as to construct and destroy objects stored in that memory.
The allocator type must satisfy the Allocator requirements.
If `Allocator::value_type` is not identical to `T`, the program will be ill-formed.

All standard library components that may need to allocate or release storage — including `std::string`, `std::vector`, and all containers except `std::array`, as well as types like `std::shared_ptr` and `std::function` (up to C++17) — use an Allocator object.
This object must be of a class type that meets the allocator requirements.

Many allocator requirements are optional because allocator-aware classes (such as standard containers) interact with allocators indirectly through `std::allocator_traits`.
The `std::allocator_traits` class provides default implementations for many of these optional requirements.

## In Other Words

All STL containers that use an allocator actually do so through `std::allocator_traits`.
This class supplies default implementations for most allocator-related behaviors.

In short, as long as the allocator you pass to an STL container is compatible with `std::allocator_traits`, it can be used seamlessly.

Source:
[🔗 cppreference: Allocator Requirements](https://en.cppreference.com/w/cpp/named_req/Allocator)


## Inside `std::vector`

Inside the implementation of `std::vector`, the code utilizes `std::allocator_traits<std::allocator<int>>`.
This default setup essentially directs all allocation and deallocation operations through the standard `new` and `delete` operators.

- **`std::allocator`** — the default allocator used by all standard library containers when no custom allocator is provided. It is stateless, meaning all instances of the same type are interchangeable and can deallocate each other’s memory.  
- **Allocators** — handle memory acquisition, release, and the construction/destruction of elements stored in that memory.  
- **`std::allocator_traits`** — acts as an intermediary between containers and allocators, providing default implementations for most allocator behaviors.  
- **STL Containers** — such as `std::vector`, `std::string`, and others, interact with allocators indirectly through `std::allocator_traits`.  
- **`std::vector` Implementation** — internally uses `std::allocator_traits<std::allocator<int>>`, routing all allocation and deallocation operations through the standard `new` and `delete` operators.  



### STL Adaptor

This project includes an `STLAdaptor` template that allows STL containers (such as `std::vector`, `std::list`, etc.) to use custom allocators. It provides a way to pass custom memory allocators (e.g., `FreeListAllocator`) into the containers, enabling them to manage memory in a custom way.

```cpp
STLAdaptor<int, FreeListAllocator> a(myAlloc);
std::vector<int, STLAdaptor<int, FreeListAllocator>> vec(a);
```

The `STLAdaptor` template allows seamless integration of custom memory allocators into the C++ Standard Library. By providing custom memory allocation strategies, the adaptor enables more efficient and fine-tuned memory management, especially in performance-critical applications.

### This constructor accepts a specific Alloc object and binds it to the STLAdaptor. It is used to pass an allocator directly to the adaptor:
```cpp
STLAdaptor(Alloc& allocator) noexcept
    : m_allocator(allocator)
{}
```

### This constructor is a template-based copy constructor, which is important for STL (Standard Template Library) containers.
It allows creating a new STLAdaptor object from another `STLAdaptor<U, Alloc>` object with the same Alloc type, but potentially a different U type. This behavior is required for STL containers to work with allocators. For example, when `std::vector<T>` is copied from another `std::vector<U>`, the allocator must be shared. Even if U = double and T = int, the allocator remains the same, so this constructor is triggered.

STL containers (such as `std::vector`, `std::list`, etc.) need to be able to copy-construct allocators, especially for different T types. Thus, such a template constructor is crucial:

```cpp
template<typename U>
STLAdaptor(const STLAdaptor<U, Alloc>& other) noexcept
    : m_allocator(other.m_allocator)
{}
```

The `operator==` function is written to compare `STLAdaptor<T, Alloc>` objects, checking whether two allocators manage the same memory region. This is essential because STL containers (e.g., `std::vector`) may check if allocators are "equal." Allocators that manage the same memory region are considered equal:

```cpp
bool operator==(const STLAdaptor<T, Alloc>& rhs) const noexcept
```

It checks if the Alloc class is derived from `FixedAllocator`:

```cpp
if constexpr(std::is_base_of_v<FixedAllocator, Alloc>)
```

If Alloc is of type `FixedAllocator`, a simple and efficient comparison is made by comparing the starting address of the memory managed by the allocator:

```cpp
return m_allocator.GetStart() == rhs.m_allocator.GetStart();
```

If Alloc is of type DynamicAllocator, the m_allocator and `rhs.m_allocator` objects are used as `DynamicAllocator*`. Then, starting from the `m_currentBlock` pointer for each allocator, it checks backward through previous blocks (`prevBlock`), effectively comparing the memory chains:

Why both a and b? Why not a single pointer?
Because two different allocators are being compared: `this->m_allocator` (the left-side adaptor) and `rhs.m_allocator` (the right-side adaptor).
- *a tracks the memory blocks for `this->m_allocator`*
- *b tracks the memory blocks for `rhs.m_allocator`*

If the oldest memory blocks are at the same address, it means the two allocators share the same memory chain, and the result of `operator==` will be true.
If the allocators do not share the same memory chain, the comparison will return false.

The structure of memory blocks is a linked list. This code checks whether two `DynamicAllocator` instances share the same memory chain (blocks) by comparing the starting addresses of the oldest memory blocks after traversing the linked list:

```cpp
return a == b;
```

## FixedAllocator and DynamicAllocator have different memory management models:

- **FixedAllocator** manages static memory blocks. It is designed for fixed-size objects. Memory is pre-allocated (e.g., a large `char[] array`). Each `allocate()` call returns one of the currently available blocks. `deallocate()` adds the block back to the free list. The `GetStart()` method returns the starting address of the memory, which was used in the `operator==` comparison.
- **DynamicAllocator** dynamically allocates blocks, which are maintained as a linked list. Memory is allocated dynamically (e.g., via `malloc/new`). It supports allocating memory for objects of varying sizes. Each block is represented by a `BlockDesc`, which keeps track of the previous block (`prevBlock`), forming a chain of blocks.

```cpp
bool operator==(const STLAdaptor<T,Alloc>& rhs) const noexcept {
    if constexpr (std::is_base_of_v<FixedAllocator, Alloc>) {
        return m_allocator.GetStart() == rhs.m_allocator.GetStart();
    } else {
        DynamicAllocator::BlockDesc* a = 
            reinterpret_cast<DynamicAllocator*>(&m_allocator)->m_currentBlock;

        while (a->prevBlock != nullptr) {
            a = a->prevBlock;
        }

        DynamicAllocator::BlockDesc* b = 
            reinterpret_cast<DynamicAllocator*>(&rhs.m_allocator)->m_currentBlock;

        while (b->prevBlock != nullptr) {
            b = b->prevBlock;
        }

        return a == b;
    }
}
```

This function defines the `operator!=` (not equal) operator and is implemented using the already defined `operator==`. It effectively checks if the allocators do not share the same memory chain:

```cpp
bool operator!=(const STLAdaptor<T,Alloc>& rhs) const noexcept {
    return !(*this == rhs);
}
```

### `allocate()` Method Parameter Explanation

#### 1. **Where Does the "n" Parameter Come From in the `allocate()` Method?**

The parameter `n` in the `allocate()` method is passed as an argument from **STL containers** such as `std::vector`, `std::list`, `std::map`, and others.

- When calling `allocate(n)`, `n` represents the number of objects of type `T` that the user wishes to allocate memory for in one operation.

#### 2. **How is `n` Passed to `allocate(n)`?**

The STL containers, when calling the `allocate()` function, pass the required number of elements to allocate as the argument `n`. For example, consider the following scenario:

```cpp
T* new_mem = alloc.allocate(new_capacity);
```

### Example Breakdown

In this example:

- `alloc` is an instance of the `STLAdaptor`.

- The `new_capacity` (an integer) is passed to the `allocate(n)` function as the argument `n`, indicating how many elements should be allocated.

#### 3. How Many Times Will `STLAdaptor::allocate(n)` Be Called, and What Will Be Printed?

When a container like `std::vector` uses the allocator, it will call the `allocate()` function multiple times based on its internal needs. The value `n` will vary each time the `allocate()` method is called, depending on the new capacity or size requirements of the container.

For example:

- If a vector needs to expand its capacity, it will call `allocate()` to request a new memory block large enough to fit the new capacity. During this process, the value of `n` (i.e., the new capacity) will be printed, helping you track the allocations.

Example usage:

```cpp
std::vector<int, STLAdaptor<int, FreeListAllocator>> vec(a);
vec.push_back(1);
vec.push_back(2);
vec.push_back(3);
```
### std::vector and Capacity Doubling Strategy

`std::vector` operates using a **capacity doubling** strategy. 

Each time it allocates new memory, it allocates more space than necessary to avoid needing to call `allocate()` again for subsequent `push_back()` operations. This helps to reduce the frequency of reallocations, improving performance as the vector grows.


# FreeListAllocator

`FreeListAllocator` is a memory allocator class that inherits from `FixedAllocator`. It manages fixed-size memory blocks and uses a linked list of free blocks to efficiently allocate and free memory. This allocator applies a custom memory management strategy to optimize memory usage and reduce fragmentation.

---

## Features

- Manages memory blocks using **free blocks**.
- Chooses the most suitable block for allocation (best fit).
- Implements **zeroing** for memory regions to ensure cleared memory after deallocation.
- Follows **defensive programming** principles and validates memory regions.

---

## Constructors

1. **`FreeListAllocator(const std::size_t sizeBytes, void* start) noexcept`**  

   Allocates `sizeBytes` of memory starting at `start`.

   ```cpp
   FreeListAllocator(sizeBytes, start);
   ```
2. **`FreeListAllocator(FreeListAllocator&& other) noexcept`**
   Move constructor for transferring ownership of allocator resources.
   ```cpp
   FreeListAllocator(std::move(other));
   ```
   This constructor allows a `FreeListAllocator` object to be moved, effectively transferring memory management ownership from one instance to another. The original object is left in a       valid but unspecified state.
3. **`FreeListAllocator& operator=(FreeListAllocator&& rhs) noexcept`**
   Move assignment operator.
   ```cpp
   allocator = std::move(rhs);
   ```
   This operator allows for the transfer of ownership between two `FreeListAllocator` instances using move semantics. The resources of the current object are replaced by those of `rhs`,      and `rhs` is left in a valid but unspecified state.
4. **`~FreeListAllocator() noexcept`**
   Destructor checks that all allocations are freed and ensures memory consistency.
   ```cpp
   ~FreeListAllocator();
   ```
   The destructor ensures that when the `FreeListAllocator` is destroyed, all allocated memory is properly freed. It also checks that no memory is leaked, verifying that the number of      allocations and used memory is zero at destruction.

## Main Functions
1. **`Allocate(const std::size_t& size, const std::uintptr_t& alignment = sizeof(std::intptr_t))`**
   Allocates a memory block of the requested size and alignment. Ensures proper alignment for allocated memory.
   ```cpp
   void* ptr = Allocate(size, alignment);
   ```

   - **Uses align_forward_adjustment_with_header to calculate the alignment and adjustment.**
   - **Implements best-fit search to find the most suitable free block.**
   - **Splits blocks if necessary to minimize wasted memory.**
2. **`Free(void* const ptr) noexcept`**
   Frees the memory block and merges adjacent free blocks (coalescing). Ensures memory is zeroed for security.
   ```cpp
   Free(ptr);
   ```

## Helper Functions

1. **`align_forward_adjustment_with_header(const void* const ptr, const std::size_t& alignment)`**
   Calculates the required alignment adjustment and ensures there is enough space for the allocation header.
   ```cpp
   std::size_t adjustment = align_forward_adjustment_with_header<AllocationHeader>(ptr, alignment);
   ```
2. **`ptr_add(const void* const p, const std::uintptr_t& amount)`**
   Adds a byte offset to a pointer.
   ```cpp
   void* newPtr = ptr_add(ptr, amount);
   ```

3. **`ptr_sub(const void* const ptr, const std::uintptr_t& sizeHeader)`**
   Subtracts a byte offset from a pointer.
   ```cpp
   void* newPtr = ptr_sub(ptr, sizeHeader);
   ```

4. **`ZeroedAddresses(std::uint8_t* ptr_addr, std::uint8_t* zero_addr) noexcept`**
   Fills the memory region between ptr_addr and zero_addr with zeros for security.
   ```cpp
   ZeroedAddresses(start, end);
   ```

## Memory Coalescing
To prevent fragmentation, `FreeListAllocator` merges adjacent free blocks when memory is freed. This process ensures larger contiguous blocks are available for future allocations and improves memory utilization.

## Use Cases

- **Memory Reuse: Ideal for game engines or high-performance applications where objects are frequently allocated and deallocated.**

- **Dynamic Memory Management: Allocates small, variable-sized blocks efficiently, minimizing waste.**


## Safety and Performance

- **Safety: Implements defensive programming, validating memory boundaries and preventing corruption.**

- **Performance: Efficient allocation and deallocation with low overhead. Best-fit search may add slight computation cost but reduces fragmentation.**

## 1. Handling Total Size Calculation without Undefined Behavior

In the Allocate method, when calculating the `totalSize` required for allocation (which includes the requested size, the adjustment for alignment, and the memory header size), special care is taken to ensure that the calculation does not result in undefined behavior (UB). Specifically, the following steps are taken to prevent UB:

- **Alignment adjustment** is carefully calculated to avoid any overflows or pointer misalignments that could lead to UB. We make sure that the resulting memory address adheres to the required alignment while avoiding exceeding available memory bounds.

- **Block size calculations** and subsequent pointer manipulations account for both alignment and the space needed for the allocation header. The method performs checks and adjustments based on the current block’s size to prevent errors during allocation or fragmentation.

The calculation of `totalSize` involves:
```cpp
std::uintptr_t adjustment = align_forward_adjustment_with_header<AllocationHeader>(freeBlock, alignment);
std::size_t totalSize = size + adjustment + sizeof(AllocationHeader);
```
This formula ensures that the `totalSize` remains within valid memory bounds and is properly aligned.

## 2. Alignment Adjustment without Undefined Behavior

In the `align_forward_adjustment_with_header` method, the alignment adjustment is calculated by first determining the pointer's address and aligning it to the next valid boundary, ensuring no overflow or misalignment occurs. The formula used accounts for required space (including headers) and prevents any miscalculations that would result in undefined behavior:
```cpp
std::uintptr_t aligned = (iptr + (alignment - 1u)) & ~(alignment - 1u);
std::size_t adjustment = aligned - iptr;
```
To ensure that the memory alignment is correct without causing UB:

- **The adjustment is carefully calculated, considering both the alignment and any additional space needed for the header.**
- **In cases where the adjustment is too small to satisfy the alignment requirements, additional space is allocated to account for any extra memory needed for proper alignment.**

This approach prevents misalignment and overflows, thereby eliminating the risk of undefined behavior related to memory addressing.

### Example: Alignment and Adjustment Calculation
```cpp
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
```
### Alignment and Adjustment Details

1. **Alignment**:
   - The memory address where data is placed must be divisible by the size of the data type.
   - For example, an `int` is 4 bytes. The aligned addresses should be at multiples of 4: `0x04`, `0x08`, `0x0C`, and so on.

2. **Adjustment**:
   - Adjustment determines how many bytes need to be added to a pointer to satisfy the alignment requirement for a specific data type.
   - If the `allocator` header does not have enough space (i.e., the adjustment is insufficient), the alignment block is increased to accommodate both the header and the alignment requirement.

3. **Allocator Header and Metadata**:
   - If there's not enough room for the allocator's metadata due to the alignment requirement, the block size is increased to ensure that both the metadata and the alignment are properly handled.
   - After this adjustment, the pointer returned to the user will be the aligned memory address (e.g., `ptr + adjustment`), such as `0x0110`. This pointer represents the aligned memory, but the metadata (header) is placed before it, for example at `0x0108` (12 bytes before the aligned address, if the header is 12 bytes in size).


Consider the following parameters:

- `ptr = 100`
- `alignment = 8`
- `sizeof(T) = 12`

Here is the breakdown of the alignment and adjustment calculation:

1. **Initial Pointer**: 
   - `iptr = 100`
   
2. **Aligned Address Calculation**:
   - `aligned = (100 + 7) & ~7 = 104`
   - `adjustment = 104 - 100 = 4`
   
3. **Required Space**:
   - `neededSpace = 12 - 4 = 8`
   - `4 < 8` → **True**

4. **Final Adjustment**:
   - `adjustment += 8 * ((8 + 7) / 8) = 4 + 8 * (15 / 8) = 4 + 8 * 1 = 12`

This ensures that the address is aligned correctly, considering the size of `T` (12 bytes) and the alignment requirement of 8 bytes.

#### Breakdown:

- The total address of the aligned pointer is `112`:
  - `112 % 8 = 0`: The alignment is correct.
  - `112 - 123`: There is exactly 12 bytes for the header (`AllocationHeader`).
  - `124 +`: There is space for user data after the header.
 


### **Note:** The `std::size_t` type is the type returned by the `sizeof` operator:

```cpp
namespace std {
    using size_t = decltype(sizeof(0));
}
```

The `sizeof` operator returns an **unsigned integer type**, which can vary depending on the platform, such as:  
`unsigned int`, `unsigned long`, `unsigned long long` (in MSVC), `unsigned __int64`, and others.
So we have:
```
std::size_t → unsigned integer type
```
Because of this, in the expression:
```
(offset + (alignment - 1)) & -alignment;
```
If the variable `alignment` is of type `std::size_t`, an **unsigned wrap-around** occurs.  
This means that the unary minus does **not** produce a negative value but instead yields a very large unsigned mask,  
which can result in **overflow**.

Therefore, in addition to being potentially **undefined behavior (UB)**, this expression can also generate a **compile-time warning or error**:
Compiler warning (level 2) C4146 - unary minus operator applied to unsigned type, result still unsigned

To solve this, we simply use the algorithm in this form:
```
(offset + (alignment - 1u)) & ~(alignment - 1u)
```
Source:
 - [🔗 indiegamedev.net ](https://indiegamedev.net/2022/03/27/custom-c20-memory-allocators-for-stl-containers/#:~:text=std%3A%3Asize_t-,align_forward_adjustment,-(const%20void)


In addition to this section, there is another potentially dangerous situation with `std::size_t`.

### Issue with `bestFit->size - bestFitTotalSize` and unsigned overflow

In the FreeListAllocator implementation, there is a potential problem in the following check:
```
if (bestFit->size - bestFitTotalSize <= sizeof(AllocationHeader)) ...
```

The issue arises because `bestFit->size` is of type `std::size_t`, an unsigned integer.  

- If `bestFitTotalSize` exceeds `bestFit->size`, the subtraction would be negative if signed,  
  but since `std::size_t` is unsigned, it **wraps around** to a very large positive number.  
- As a result, the condition is **never satisfied**, even when logically the block may be smaller than the `AllocationHeader` size.

To avoid this, we perform the comparison via the addition operation:
```
if (bestFit->size <= bestFitTotalSize + sizeof(AllocationHeader)) ...
```

Sources: 
 - [🔗 gamedev.net ](https://www.gamedev.net/tutorials/programming/general-and-gameplay-programming/c-custom-memory-allocation-r3010/#:~:text=will%20be%20impossible-,if,-(free_block%2D%3Esize%20%2D%20total_size)

 - [🔗 indiegamedev.net ](https://indiegamedev.net/2022/03/27/custom-c20-memory-allocators-for-stl-containers/#:~:text=std%3A%3Abad_alloc()%3B%0A%20%20%20%20%7D-,if,-(bestFit%2D%3Esize%20%2D%20bestFitTotalSize)


## 3. Freeing Memory Blocks with Care

In the Free method, blocks are merged or split based on the totalSize of the allocated memory to minimize fragmentation and ensure that memory management does not cause undefined behavior when merging or splitting free blocks.

# Why This Matters

When implementing custom memory allocators, it’s crucial to **avoid undefined behavior (UB)**, especially with pointer arithmetic and memory alignment. UB can lead to **crashes**, **memory corruption**, or **security vulnerabilities**. By carefully controlling memory block sizes, alignment adjustments, and pointer manipulations, the `FreeListAllocator` ensures robust and reliable memory management without UB.

This careful handling of memory allocation and deallocation ensures that operations are **safe**, **predictable**, and **efficient**—even in complex memory management scenarios.

## Memory Alignment Calculation Example

**Example Scenario:**

- **`bestFit`** = **`0x1E64`**

- **`adjustment`** = **`16`**

- **`alignedAddr`** = **`0x1E74`**

- **`header location`** = **`0x1E64`**

The calculation results in:
```
0x1E64 + 16 = 0x1E74
0x1E64 + 0x10 = 0x1E74
```

## 1. Explanation of Terms

- **bestFit (0x1E64)**:  
  This is the address where the free memory block starts. In other words, it is the address of the available memory block.

- **adjustment (16)**:  
  The number of bytes that need to be added to the `bestFit` address to reach the aligned address.

- **alignedAddr (0x1E74)**:  
  This is the memory address that satisfies the alignment requirements. It is the address after the adjustment, ensuring compliance with alignment rules (such as 8-byte or 16-byte alignment).

- **header location (0x1E64)**:  
  This is where the metadata (such as size, state, etc.) of the block is stored. In this case, it is the same as `bestFit` since the metadata is stored at the beginning of the block.


## 2. How is the Calculation Performed?

The formula for calculating alignedAddr is:
```
alignedAddr = bestFit + adjustment
```

Thus, using hexadecimal addition:
```
alignedAddr = 0x1E64 + 0x10
```

In decimal:
```
0x1E64 = 7780 (decimal)
0x10 = 16 (decimal)
0x1E64 + 0x10 = 0x1E74 → 7796 (decimal)
```

This calculation simply adds the two hexadecimal numbers.


## 3. Why is `adjustment` Needed?

The `adjustment` is necessary to align the pointer to a specific boundary. For example, if `alignedAddr` needs to be aligned to a 16-byte boundary (which is often important for CPU performance), we check the remainder when `bestFit` is divided by 16.

Example:
```
0x1E64 % 16 = 4 (not aligned to 16-byte boundary)
```

Since the remainder is 4, the `adjustment` is calculated as:

adjustment = (16 - 4) = 12

However, in this case, the `adjustment` is explicitly set to 16, meaning:

- **Either `alignedAddr` is aligned to a larger alignment requirement (e.g., 32-byte alignment)**
or
- **The header is included in the calculation, accounting for metadata space at the start of the block.**


## 4. How Are the Header and Aligned Data Positioned?

The memory layout for a block looks like this:
```
0x1E64: [ Header (metadata) ]
0x1E74: [ Actual data (aligned start) ]
```

In this design:

- The **actual data** starts at the `alignedAddr` (`0x1E74`), which satisfies the alignment requirement.**

- **Metadata (header)** is placed just before the aligned data at the `bestFit` address (`0x1E64`).*

This is a typical design where the **data** is placed at the aligned address, but a small **padding** (e.g., 16 bytes) is reserved for the **header** before it. This ensures that the data starts at the correct alignment boundary.


## 5. Memory Alignment and Header Layout

The user-supplied pointer (`ptr`) starts at the aligned address. Here's how the memory block layout works:
```
|<------------------------ blockSize ------------------->|
| blockStart |<----------- adjustment ------------------>| ptr | ... user data ... | blockEnd
                                |<-- AllocationHeader -->|
                                                         ↑
                                                         aligned ptr
```

- **blockStart** is the `bestFit` address.

- **Header** is the metadata stored before the user data.

- **padding** represents the space needed to align the user data correctly.

- **ptr** is the aligned pointer that points to the actual user data.

When the `Free()` function is called, the pointer passed (`ptr`) is already aligned, so we subtract the header size (`sizeof(AllocationHeader)`) to reach the start of the block:
```
ptr - sizeof(AllocationHeader)  // Reaches the header location
ptr - header->adjustment        // Reaches the bestFit location
```

This ensures that the memory block is properly freed by first accessing the metadata (header) and then calculating the correct memory address for the free operation.


## AllocationHeader Memory Trick Explanation

```cpp
struct FreeBlock {
    size_t size;
    FreeBlock* next;
};
```

```cpp
std::size_t totalSize = size + adjustment;  --->  bestFitTotalSize
```
...

```cpp
FreeBlock* newBlock = reinterpret_cast<FreeBlock*>(ptr_add(bestFit, bestFitTotalSize));
```

The memory space for the `AllocationHeader` is not explicitly reserved.  
Instead, a clever **memory trick** is used here — the header (`AllocationHeader`) is **hidden inside the padding area** of the `FreeBlock`.

Since the `FreeBlock` structure is relatively small (16 bytes), the starting address of the newly allocated block (`newBlock`) falls just after  
`bestFit + bestFitTotalSize`. This means that there is **no overlap** between the `FreeBlock` structure and the new allocation block.  
However, the boundary between them is very close because these limits are not calculated with absolute precision.

Additionally, depending on the **compiler** and its **alignment rules**, those extra 4 bytes of padding could be arranged differently,  
which theoretically might cause an overlap with the `AllocationHeader` — though this is only a **theoretical possibility**.

adjustment = padding:
```
|<---------------------------- blockSize ----------------------->|
|<---- FreeBlock (size, next) ----->|<------- adjustment ------->| ptr | ... user data ... | blockEnd
                                        |<-- AllocationHeader -->|                         ↑
                                                                 ↑                         newBlock starts here
                                                                 aligned ptr
```

---

## Overlap Issue When Adding `prev` to FreeBlock

```cpp
struct FreeBlock {
    size_t size;
    FreeBlock* next;
    FreeBlock* prev;    <------\
};
```

```cpp
std::size_t totalSize = size + adjustment;  --->  bestFitTotalSize
```
...

```cpp
FreeBlock* newBlock = reinterpret_cast<FreeBlock*>(ptr_add(bestFit, bestFitTotalSize));
```

When the `prev` pointer is added (making the `FreeBlock` structure 24 bytes in size),  
and the `newBlock` is still positioned using `bestFit + bestFitTotalSize`,  
the size of the `AllocationHeader` is **not accounted for**.  

As a result, an **overlap** occurs between the `FreeBlock` structure and the `newBlock`.  
The memory region allocated for `newBlock` actually overwrites the metadata of the `FreeBlock` —  
specifically (in order: 1. `size`, 2. `next`, 3. `prev`),- **Line 142 (C++)**.

```cpp
Exception thrown --> Write access violation:
bestFit = 0x0000021483a43660 {size=0x0000010c next=0x0000000000000000 <NULL> prev = 0x000000f800000000 {size=??? next=??? prev=??? } }

>	FreeListAllocator.exe!FreeListAllocator::Allocate(const unsigned __int64 & size, const unsigned __int64 & alignment) Line 142	C++:

else
    {
        FreeBlock* newBlock = reinterpret_cast<FreeBlock*>(ptr_add(bestFit, bestFitTotalSize));
        newBlock->size = bestFit->size - bestFitTotalSize;
        newBlock->next = bestFit->next;
        newBlock->prev = bestFit->prev;

        if (bestFit->next != nullptr)           
            bestFit->next->prev = newBlock;
        if (bestFit->prev != nullptr)            
            bestFit->prev->next = newBlock;   <------  bestFit->**prev** was 0xF800000000
        else
            m_freeBlocks = newBlock;

        
    }
```

adjustment = padding:
```
|<------------------------------ blockSize --------------------------->|
|<---- FreeBlock (size, next, prev) ----->|<------- adjustment ------->| ptr | ... user data ... | blockEnd
                             ↑                |<-- AllocationHeader -->|                         
                             newBlock starts here                      ↑                         
                                                                       aligned ptr
```

---

## Proving the Exception Cause

```cpp
struct FreeBlock {
    size_t size;
    FreeBlock* prev;
    FreeBlock* next;    <------\
    
};
```

```cpp
std::size_t totalSize = size + adjustment;  --->  bestFitTotalSize
```
...

```cpp
FreeBlock* newBlock = reinterpret_cast<FreeBlock*>(ptr_add(bestFit, bestFitTotalSize));
```

It is quite easy to demonstrate that the **exception** occurs precisely because of this issue.  
If we slightly modify the layout of the `FreeBlock` structure, even though the entire metadata becomes corrupted,  
we can still observe a **difference in the line number** reported during exception handling —  
which clearly indicates the memory overlap problem,- **Line 140 (C++)**.

```cpp
Exception thrown --> Write access violation:
bestFit = 0x0000025e74624530 {size=0x0000010c prev=0x0000000000000000 <NULL> next=0x000000f800000000 {size=??? prev=??? next=??? } }


>	FreeListAllocator.exe!FreeListAllocator::Allocate(const unsigned __int64 & size, const unsigned __int64 & alignment) Line 140	C++:

else
    {
        FreeBlock* newBlock = reinterpret_cast<FreeBlock*>(ptr_add(bestFit, bestFitTotalSize));
        newBlock->size = bestFit->size - bestFitTotalSize;
        newBlock->next = bestFit->next;
        newBlock->prev = bestFit->prev;

        if (bestFit->next != nullptr)           
            bestFit->next->prev = newBlock;   <------  bestFit->next was 0xF800000000

        if (bestFit->prev != nullptr)            
            bestFit->prev->next = newBlock;
        else
            m_freeBlocks = newBlock;

        
    }
```

adjustment = padding:
```
|<------------------------------ blockSize --------------------------->|
|<---- FreeBlock (size, prev, next) ----->|<------- adjustment ------->| ptr | ... user data ... | blockEnd
                             ↑                |<-- AllocationHeader -->|                         
                             newBlock starts here                      ↑                         
                                                                       aligned ptr
```

---

## Fixing the Overlapping Issue

```cpp
struct FreeBlock {
    size_t size;
    FreeBlock* next;
    FreeBlock* prev;
};
```

```cpp
std::size_t totalSize = size + adjustment + sizeof(AllocationHeader);  --->  bestFitTotalSize    <------\
```
...

```cpp
FreeBlock* newBlock = reinterpret_cast<FreeBlock*>(ptr_add(bestFit, bestFitTotalSize));
```

To prevent this **overlapping problem**, we simply add the size of the `AllocationHeader`  
(`sizeof(AllocationHeader)`) to the `totalSize` calculation.  

By doing this, the **WAV (Write Access Violation)** error is completely eliminated,  
as the allocated memory regions no longer overlap.

adjustment = padding + header:
```
|<--------------------------- blockSize ---------------------------->|
|<---- FreeBlock (size, next, prev) ---->|<------- adjustment ------>| aligned ptr | ... user data ... | blockEnd
                                            |<-- AllocationHeader -->|                                 ↑
                                                                     ↑                                 newBlock starts here
                                                                     aligned ptr
```

---

### Deep Notes:
- [Custom C++20 Memory Allocators for STL Containers](https://indiegamedev.net/2022/03/27/custom-c20-memory-allocators-for-stl-containers/)
- [C Custom Memory Allocation](https://www.gamedev.net/tutorials/programming/general-and-gameplay-programming/c-custom-memory-allocation-r3010/)
- [Writing Your Own Memory Allocators](https://screwjankgames.github.io/engine%20programming/2020/09/24/writing-your-own-memory-allocators.html?utm_source=chatgpt.com)
- [Understanding std::align](https://lesleylai.info/en/std-align/)
- [Video: Custom Memory Allocators](https://youtu.be/pP15kDeXJU0)
- [C++ Design: Allocators](https://hackingcpp.com/cpp/design/allocators.html)
- [Writing a Memory Allocator](http://dmitrysoshnikov.com/compilers/writing-a-memory-allocator/)
- [C++ Allocators for Games](https://anki3d.org/cpp-allocators-for-games/)
- [cppreference: std::align](https://en.cppreference.com/w/cpp/memory/align)
- [Apache StdCXX: Allocator Reference](https://stdcxx.apache.org/doc/stdlibref/allocator.html)
- [ROS Advanced Allocator Template Tutorial](https://docs.ros.org/en/foxy/Tutorials/Advanced/Allocator-Template-Tutorial.html)
- [Oracle Documentation: Memory Allocators](https://docs.oracle.com/cd/E19205-01/819-3703/15_3.htm)
- [Medium Article: Implementing Custom Allocators](https://medium.com/@simontoth/daily-bit-e-of-c-implementing-custom-allocators-d8a54c44a1dc)
- [Habr Article on Memory Allocators](https://habr.com/ru/articles/304308/)
- [StackOverflow: Aligning an Address Forward](https://stackoverflow.com/questions/53914652/aligning-an-address-forward)
- [Alexandrescu CppCon Video on Allocators](https://www.youtube.com/watch?v=LIb3L4vKZ7U&t=2598s&ab_channel=CppCon)


## Conclusion

This explanation outlines how **memory alignment** is handled in the allocator, ensuring that both **user data** and **metadata** are placed correctly in memory. The adjustment ensures that data starts at an aligned address, which is crucial for both performance and correctness in memory management.
This project is a demonstration of how custom allocators can be implemented and integrated with STL containers in C++. It focuses on defensive programming techniques to ensure memory safety, prevent fragmentation, and make memory management more predictable.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
