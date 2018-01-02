#ifndef MY_GUI_FONT_H
#define MY_GUI_FONT_H

#include "slt_runtime/drawing/texture_atlas.h"
#include "slt/image.h"

#include "stb_truetype.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace slt {
  namespace gui {
  struct Glyph {
    Vec2 top_left_offset;
    Vec2 bottom_right_offset;

    Vec2 advance;

    Vec2 top_left_uv;
    Vec2 top_right_uv;
    Vec2 bottom_left_uv;
    Vec2 bottom_right_uv;
  };

  class FontData;
  class Font {
  public:
    Font(FontData* fnt, float scale);
    ~Font();

    float getAscent() const {
      return ascent_;
    }

    float getDescent() const {
      return descent_;
    }

    float getLineGap() const {
      return line_gap_;
    }

    typename render::TextureRef getTexture() {
      return atlas_->texture();
    }

    float getGlyphAdvance(std::uint32_t c);
    Glyph const & getGlyph(std::uint32_t c);

  private:
    std::shared_ptr<FontData> data_;

    std::unordered_map<std::uint32_t, Glyph> glyphs_;
    std::shared_ptr<TextureAtlas<char>> atlas_;

    float scale_;
    float ascent_;
    float descent_;
    float line_gap_;
  };

  class FontData : public std::enable_shared_from_this<FontData> {
  public:
    FontData(DataBlock ttf_data);
    ~FontData();

    std::shared_ptr<Font> getScaled(float scale);
  private:
    friend class Font;
    // This is kept around for unfrozen fonts, so that we can lookup glyphs on demand.
    DataBlock ttf_data_;
    stbtt_fontinfo font_info_;

    std::unordered_map<float, std::weak_ptr<Font>> scaled_;
  };

    using FontDataRef = std::shared_ptr<FontData>;
    using FontRef = std::shared_ptr<Font>;
  }

  void loadFontData(std::string const&, std::string const&,
                   std::function<void(gui::FontDataRef)>);
}

#endif
