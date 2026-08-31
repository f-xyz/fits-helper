#include <memory>

namespace utils::allocators {

template <typename T> struct UninitializedAllocator : std::allocator<T> {
  template <typename U, typename... Args> void construct(U *, Args &&...) {
    // No zero init
  }
};

} // namespace utils::allocators