#include "slt/drawing/image.h"

namespace slt {
Image(iVec2 size, int bits_per_pixel, int row_align = 4)
    : size_(size),
      bits_per_pixel_(bits_per_pixel),
      row_align_(row_align),
      bytes_per_row_(
          mem::align_size((size.x * bits_per_pixel + 7) / 8, row_align)),
      data_(row_len_ * size[1]) {}

Image Image::fromPng(DataView data) {}

Image rotateCCW(Image const& img) {
  auto size = img.size();
  auto row_len = img.rowLength();
  auto channels = img.channels();

  std::swap(size.x, size.y);

  Image result(size, channels);
  for (int i = 0; i < size.x; ++i) {
    for (int j = 0; j < size.y; ++j) {
      result(i, j) = img(j, i);
    }
  }

  return std::move(result);
}
}
}
