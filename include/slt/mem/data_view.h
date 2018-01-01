#ifndef SLT_MEMORY_DATA_VIEW_H
#define SLT_MEMORY_DATA_VIEW_H

#include <memory>
#include <string>
#include <type_traits>
#include <vector>
#include "slt/debug/assert.h"
#include "slt/log.h"
#include "slt/mem/alignment.h"
#include "slt/mem/data_block.h"
//#include <capnp/message.h>

namespace slt {

// Represents a <mutable> representation of a memory range. There is no hold
// being made on the underlying memory. It's the user's responsability to ensure
// this does not become a dangling pointer. As such, these types should be
// primarily used on the stack, and not the heap.
class MutableDataView {
 public:
  MutableDataView(char* data, std::size_t size) : start_(data), size_(size) {}

  template <typename T>
  MutableDataView(T* obj)
      : start_(reinterpret_cast<char*>(obj)), size_(sizeof(T)) {}

  template <typename T>
  MutableDataView(std::vector<T>& data) {
    static_assert(std::is_pod<T>::value,
                  "cannot initialize data view to non-pod vector");
    start_ = data.data();
    size_ = data.size() * sizeof(T);
  }

  MutableDataView(DataBlock& data) {
    start_ = data.data();
    size_ = data.size();
  }

  std::size_t size() const { return size_; }

  char* data() const { return start_; }

  // Checks if the front of the data view can be interpreted as an instance
  // of T. This checks for available size, as well as valid alignment.
  template <typename T>
  bool isValid() const {
    static_assert(std::is_standard_layout<T>::value,
                  "Trying to read data as a non-standard layout type");
    return size_ >= sizeof(T) && mem::isAligned(start_, alignof(T));
  }

  // Convenience function for accessing the front of the data view as a pointer
  // to T. If you use this function, you are responsible for ensuring strict
  // aliasing is not being violated through it.
  template <typename T>
  T* as() {
    static_assert(std::is_pod<T>::value,
                  "Trying to read mutable data as a non-pod type");
    SLT_ASSERT(isValid<T>());
    return reinterpret_cast<T*>(start_);
  }

  template <typename T>
  T const* as() const {
    SLT_ASSERT(isValid<T>());
    return reinterpret_cast<T const*>(start_);
  }

  MutableDataView align(std::size_t bound) {
    MutableDataView result(*this);
    std::align(bound, 0, *(void**)&result.start_, result.size_);
    return result;
  }

  MutableDataView& operator+=(std::size_t offset) {
    SLT_ASSERT_LE(offset, size_);
    start_ += offset;
    size_ -= offset;
    return *this;
  }

 private:
  char* start_;
  std::size_t size_;

  friend class DataView;
};

class DataView {
 public:
  DataView() : start_(nullptr), size_(0) {}
  DataView(MutableDataView const& rhs) : start_(rhs.start_), size_(rhs.size_) {}

  DataView(char const* data, std::size_t size)
      : start_((char const*)data), size_(size) {}

  DataView(DataBlock const& data) {
    start_ = data.data();
    size_ = data.size();
  }

  template <typename T>
  DataView(std::vector<T> const& data)
      : DataView(reinterpret_cast<char const*>(data.data()),
                 data.size() * sizeof(T)) {
    static_assert(std::is_standard_layout<T>::value,
                  "cannot initialize data view to non standard-layout vector");
  }

  template <typename T>
  DataView(T const* obj)
      : DataView(reinterpret_cast<char const*>(obj), sizeof(T)) {}

  // Checks if the front of the data view can be interpreted as an instance
  // of T. This checks for available size, as well as valid alignment.
  template <typename T>
  bool isValid() const {
    static_assert(std::is_standard_layout<T>::value,
                  "Trying to read data as a non-standard layout type");
    return size_ >= sizeof(T) && mem::isAligned(start_, alignof(T));
  }

  // Convenience function for accessing the front of the data view as a pointer
  // to T. If you use this function, you are responsible for ensuring strict
  // aliasing is not being violated through it.
  template <typename T>
  T const* as() const {
    SLT_ASSERT(isValid<T>());
    return reinterpret_cast<T const*>(start_);
  }

  std::size_t size() const { return size_; }
  char const* data() const { return start_; }

  DataView align(std::size_t bound) {
    DataView result(*this);
    std::align(bound, 0, *(void**)&result.start_, result.size_);
    return result;
  }

  DataView& operator+=(std::size_t offset) {
    SLT_ASSERT_LE(offset, size_);
    start_ += offset;
    size_ -= offset;
    return *this;
  }

 private:
  char const* start_;
  std::size_t size_;
};

/*

*/
}  // namespace slt

#endif
