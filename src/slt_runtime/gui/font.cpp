#define STB_TRUETYPE_IMPLEMENTATION
#include "slt_runtime/gui/font.h"
#include "slt/file/read.h"
#include "slt_runtime/runtime.h"

namespace {
  std::string getFileForFont(std::string const& package,
                                std::string const& name) {
    return package + "/fonts/" + name + ".fnt";
  }

}
namespace slt {
  void loadFontData(std::string const& package, std::string const& name,
                 std::function<void(gui::FontDataRef)> cb) {

   slt::file::asyncRead(getFileForFont(package, name),
                        slt::Runtime::instance->getMainQueue(),
                        [cb](slt::DataBlock d) {
                          auto result =
                              std::make_shared<gui::FontData>(std::move(d));
                          cb(std::move(result));
                        });

  }
  namespace gui {

    FontData::FontData(DataBlock ttf_data)
      : ttf_data_(std::move(ttf_data)) {
      stbtt_InitFont(&font_info_,
                     reinterpret_cast<const unsigned char*>(ttf_data_.data()),
                     stbtt_GetFontOffsetForIndex(reinterpret_cast<const unsigned char*>(ttf_data_.data()), 0));
    }

    FontData::~FontData() {
      // Nothing, really?
    }


    std::shared_ptr<Font> FontData::getScaled(float size) {
      auto found = scaled_.find(size);
      if(found != scaled_.end()) {
        auto locked = found->second.lock();
        if(locked) {
          return locked;
        }
      }

      auto new_scaled = std::make_shared<Font>(this, size);
      scaled_[size] = new_scaled;
      return new_scaled;
    }


    Font::Font(FontData* fnt, float size)
      : data_(fnt->shared_from_this()) {
      int ascent = 0;
      int descent = 0;
      int linegap = 0;
      stbtt_GetFontVMetrics(&fnt->font_info_, &ascent, &descent, &linegap);

      // For now, we are just going to create an atlas for each font. We'll add atlas sharing later.
      atlas_ = std::make_shared<TextureAtlas<char>>(iVec2{128,128});
      scale_ = stbtt_ScaleForPixelHeight(&fnt->font_info_, size);

      ascent_ = (float)ascent * scale_;
      descent_ = (float)-descent * scale_;
      line_gap_ = (float)linegap * scale_;
    }

    Font::~Font() {

    }

  float Font::getGlyphAdvance(std::uint32_t c) {
    auto found = glyphs_.find(c);
    if (found != glyphs_.end()) {
      return found->second.advance.x;
    }

    int advance_w;
    int left_side_bearing;
    stbtt_GetCodepointHMetrics(&data_->font_info_, c, &advance_w, &left_side_bearing);

    return (float)advance_w * scale_;
  }

  Glyph const & Font::getGlyph(std::uint32_t c) {
    auto found = glyphs_.find(c);
    if(found == glyphs_.end()) {

      std::array<int, 2> top_left;
      std::array<int, 2> bottom_right;
      stbtt_GetCodepointBitmapBox(&data_->font_info_, c, scale_, scale_,
        &top_left[0], &top_left[1],
        &bottom_right[0], &bottom_right[1]);

      top_left[1] *= -1;
      bottom_right[1] *= -1;

      std::swap(top_left[1], bottom_right[1]);
      int advance_w;
      int left_side_bearing;
      stbtt_GetCodepointHMetrics(&data_->font_info_, c, &advance_w, &left_side_bearing);

      iVec2 glyph_size = { bottom_right[0] - top_left[0], bottom_right[1] - top_left[1] };
      Image<char> glyph_img(glyph_size);


      Glyph new_glyph;
      new_glyph.top_left_offset = { (float)top_left[0], (float)bottom_right[1]};
      new_glyph.bottom_right_offset = { (float)bottom_right[0], (float)top_left[1]};

      new_glyph.advance = { (float)advance_w * scale_, 0.0f };

      if (glyph_size[0] > 0 && glyph_size[1] > 0) {
        stbtt_MakeCodepointBitmap(
          &data_->font_info_, reinterpret_cast<unsigned char*>(glyph_img.data().data()), glyph_size[0], glyph_size[1], glyph_img.getRowLength(), scale_, scale_, c);

        UVQuad uvs;
        bool success = atlas_->add_image(glyph_img, uvs);

        new_glyph.top_left_uv = uvs.top_left;
        new_glyph.bottom_right_uv = uvs.bottom_right;
        new_glyph.bottom_left_uv = uvs.bottom_left;
        new_glyph.top_right_uv = uvs.top_right;
      }
      else {
        new_glyph.top_left_uv = { 0,0 };
        new_glyph.bottom_right_uv = { 0,0 };
        new_glyph.bottom_left_uv = { 0,0 };
        new_glyph.top_right_uv = { 0,0 };
      }


      auto emplaced = glyphs_.emplace(c, new_glyph);

      return emplaced.first->second;

    }
    else {
      return found->second;
    }
  }
}
}
