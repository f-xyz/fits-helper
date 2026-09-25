#pragma once

#include <memory>

namespace astroutils::allocators {

template <typename T> struct UninitializedAllocator : std::allocator<T> {
  template <typename U, typename... Args> void construct(U *, Args &&...) {
    // No zero init
  }
};

} // namespace astroutils::allocators