#ifndef SLT_IMAGE_H
#define SLT_IMAGE_H

#include "slt/math/vec.h"
#include "slt/mem/alignment.h"
#include "slt/mem/data_block.h"
#include "slt/mem/data_view.h"

#include <array>
#include <numeric>
#include <vector>

namespace slt {

  template<typename Pixel>
  class Image {
  public:
    Image(iVec2 size) :
      size_(size),
      row_len_(mem::alignSize(size.x * sizeof(Pixel), 4)),
      data_(row_len_ * size[1]) {

        if(!std::is_trivially_constructible<Pixel>::value) {
          // This should compile
          char* data_ptr = data_.data();
          for(int j = 0; j < size[1]; ++j) {
            new (data_ptr) Pixel[size.x];
            data_ptr += row_len_;
          }
        }
      }

    Image(Image &&) = default;
    Image& operator=(Image &&) = default;

    Image(Image const&) = delete;
    Image& operator=(Image const&) = delete;

    Pixel& operator()(int x, int y) {
      std::size_t offset = y * row_len_ + x * sizeof(Pixel);
      return *reinterpret_cast<Pixel*>(data_.data() + offset);
    }

    Pixel const& operator()(int x, int y) const {
      std::size_t offset = y * row_len_ + x * sizeof(Pixel);
      return *reinterpret_cast<Pixel const*>(data_.data() + offset);
    }

    iVec2 const& size() const { return size_; }

    DataView data() const { return DataView(data_); }
    MutableDataView data(){ return MutableDataView(data_); }

    std::size_t getRowLength() const {
      return row_len_;
    }
  private:
    iVec2 size_;
    std::size_t row_len_;
    DataBlock data_;
  };

  template<typename Pixel>
  Image<Pixel> flipCCW(Image<Pixel> const& img) {
    auto size = img.size();
    std::swap(size.x, size.y);

    Image<Pixel> result(size);
    for(int i = 0 ; i < size.x ; ++i) {
      for(int j = 0 ; j < size.y ; ++j) {
        result(i , j) = img(j, i);
      }
    }

    return std::move(result);
  }
}

#endif
