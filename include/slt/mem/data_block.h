#ifndef SLT_MEMORY_DATA_BLOCK_H
#define SLT_MEMORY_DATA_BLOCK_H

#include <cstring>
#include <streambuf>

#include "slt/debug/assert.h"
#include "slt/mem/alignment.h"
#include "slt/mem/read_buf.h"

namespace slt {

// Defines a movable raw block of memory.

// Unlike std::vector<char>, DataBlock leaves allocated data completely
// uninitialized.
class DataBlock {
 public:
  DataBlock() : length_(0), data_(nullptr) {}
  explicit DataBlock(std::size_t size) : length_(size), data_(new char[size]) {}
  explicit DataBlock(mem::SizeAndAlignment spec)
      : length_(spec.size), data_(new char[spec.size]) {
    // Double check, in case a sneaky memory system is being used.
    SLT_ASSERT(mem::isAligned(data_, spec.align));
  }

  DataBlock(DataBlock&& rhs) : length_(rhs.length_), data_(rhs.data_) {
    rhs.data_ = nullptr;
    rhs.length_ = 0;
  }

  ~DataBlock() {
    if (data_) {
      delete[] data_;
    }
  }

  DataBlock& operator=(DataBlock&& rhs) {
    if (data_) {
      delete[] data_;
    }
    length_ = rhs.length_;
    data_ = rhs.data_;
    rhs.data_ = nullptr;
    rhs.length_ = 0;
    return *this;
  }

  template <typename T>
  DataBlock& operator=(std::initializer_list<T> l) {
    length_ = l.size() * sizeof(T);
    data_ = new char[length_];

    memcpy(data_, l.begin(), length_);
    return *this;
  }

  size_t size() const { return length_; }

  char const* data() const { return data_; }

  char* data() { return data_; }

  operator bool() const { return length_ > 0; }
  // These are only implemented to accomodate std::function's requirements, but
  // should never be called.
  DataBlock(DataBlock const& rhs) { SLT_UNREACHABLE(); }
  DataBlock& operator=(DataBlock const& rhs) { SLT_UNREACHABLE(); }

  ReadBuf getStreamBuf() const { return ReadBuf(data_, length_); }

 private:
  std::size_t length_;
  char* data_;
};

}  // namespace slt
#endif