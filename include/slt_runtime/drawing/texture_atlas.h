#ifndef MY_GUI_TETXURE_ATLAS_H
#define MY_GUI_TETXURE_ATLAS_H

#include "slt/math/vec.h"
#include "slt/image.h"
#include "slt/algo/rect_pack.h"
#include "slt_runtime/drawing/texture.h"

#include <numeric>

namespace slt {

  struct UVQuad{
    Vec2 top_left;
    Vec2 top_right;
    Vec2 bottom_right;
    Vec2 bottom_left;
  };

  template<typename Pixel>
  class TextureAtlas {
  public:
    TextureAtlas(iVec2 size)
      : size_(size)
      , packer_(size) {
      render::TexConfig cfg;
      cfg.size = size;
      cfg.tex_format = render::PixelTraits<Pixel>::getTexFormat();
      texture_ = render::createTexture(cfg);
    }

    render::TextureRef texture() { return texture_; }

    bool add_image(Image<Pixel> const& img, UVQuad& result) {
      auto img_size = img.size();
      auto pack_size = img_size;
      pack_size += iVec2{1, 1};

      packer_t::Result packing_result;
      bool success = packer_.pack(pack_size, packing_result);

      if(!success) {
        return false;
      }

      if (packing_result.flipped) {

        //assign outgoing UVS
        result.top_right.x = float(packing_result.pos[0] ) / float(size_[0]);
        result.top_right.y = float(packing_result.pos[1] + img_size[0]) / float(size_[1]);

        result.bottom_left.x = float(packing_result.pos[0] + img_size[1]) / float(size_[0]);
        result.bottom_left.y = float(packing_result.pos[1]) / float(size_[1]);

        result.bottom_right.x = result.bottom_left.x;
        result.bottom_right.y = result.top_right.y;

        result.top_left.x = result.top_right.x;
        result.top_left.y = result.bottom_left.y;

        auto flipped_img = flipCCW(img);

        std::swap(img_size[0], img_size[1]);
        render::updateTexture(texture_, packing_result.pos, flipped_img);
      }
      else {
        result.top_left.x = float(packing_result.pos[0]) / float(size_[0]);
        result.top_left.y = float(packing_result.pos[1]) / float(size_[1]);

        result.bottom_right.x = float(packing_result.pos[0] + img_size[0] ) / float(size_[0]);
        result.bottom_right.y = float(packing_result.pos[1] + img_size[1] ) / float(size_[1]);

        result.bottom_left.x = result.top_left.x;
        result.bottom_left.y = result.bottom_right.y;

        result.top_right.x = result.bottom_right.x;
        result.top_right.y = result.top_left.y;

        render::updateTexture(texture_, packing_result.pos, img);
      }

      return true;
    }

  private:
    using packer_t = slt::RectPacker2D<int>;
    packer_t packer_;

    iVec2 size_;

    render::TextureRef texture_;
  };
}

#endif
