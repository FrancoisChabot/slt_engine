#ifndef SLT_MEM_MISC_H
#define SLT_MEM_MISC_H

#include <cstdint>
#include <memory>
#include "slt/debug/invariant.h"

namespace slt {
namespace mem {

struct SizeAndAlignment {
  std::size_t size;
  std::size_t align;
};

// Tests wether a pointer is aligned to a given memory boundary.
inline bool isAligned(void const* ptr, std::size_t align) {
  return (std::uintptr_t(ptr) % align) == 0;
}

inline std::size_t alignSize(std::size_t size, std::size_t align) {
  // make sure we are dealing with a power of two
  SLT_ASSERT((align & (align - 1)) == 0);
  return (size + (align - 1)) & ~(align - 1);
}
}  // namespace mem

template <>
inline void checkInvariant<mem::SizeAndAlignment>(
    mem::SizeAndAlignment const& v) {
  SLT_ASSERT_GT(v.align, 0);
}
}  // namespace slt
#endif