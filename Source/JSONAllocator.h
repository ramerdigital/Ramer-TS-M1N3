#pragma once

#include <memory>
#include <atomic>

struct AllocationTracker
{
    static inline std::atomic<size_t> activeAllocations { 0 };
    static inline std::atomic<size_t> totalAllocatedBytes { 0 };

    static void reset()
    {
        activeAllocations = 0;
        totalAllocatedBytes = 0;
    }
};

template <typename T>
class MyJSONAllocator
{
public:
    using value_type = T;

    MyJSONAllocator() noexcept = default;
    
    template <typename U>
    MyJSONAllocator (const MyJSONAllocator<U>&) noexcept {}

    T* allocate (std::size_t n)
    {
        AllocationTracker::activeAllocations++;
        AllocationTracker::totalAllocatedBytes += n * sizeof (T);
        return std::allocator<T>{}.allocate (n);
    }

    void deallocate (T* p, std::size_t n) noexcept
    {
        AllocationTracker::activeAllocations--;
        std::allocator<T>{}.deallocate (p, n);
    }
};

template <typename T, typename U>
bool operator== (const MyJSONAllocator<T>&, const MyJSONAllocator<U>&) noexcept { return true; }

template <typename T, typename U>
bool operator!= (const MyJSONAllocator<T>&, const MyJSONAllocator<U>&) noexcept { return false; }
