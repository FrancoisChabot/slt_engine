#ifndef SLT_RUNTIME_RENDER_TEXTURE_H
#define SLT_RUNTIME_RENDER_TEXTURE_H

#include "slt/math/vec.h"
#include "slt/image.h"
#include "slt/mem/data_view.h"

#include <functional>
#include <memory>

namespace slt {
namespace render {
// Opaque Texture object interface.
struct Texture {
  virtual ~Texture() {}
};

// Format of a texture as encoded on the GPU.
enum class TexFormat { RGBA_8, DEPTH, SINGLE_8};

// Format of texture data when uploading.
enum class TexDataFormat { RGBA_8, BGRA_8, SINGLE_8 };

template<typename Pixel>
struct PixelTraits {};

template<>
struct PixelTraits<char> {
  static TexFormat getTexFormat() {
    return TexFormat::SINGLE_8;
  }

  static TexDataFormat getTexDataFormat() {
    return TexDataFormat::SINGLE_8;
  }
};

struct TexConfig {
  iVec2 size;
  TexFormat tex_format = TexFormat::RGBA_8;
  unsigned int levels = 1;
};


using TextureRef = std::shared_ptr<Texture>;

TextureRef createTexture(TexConfig const& cfg);

void updateTexture(TextureRef const& tex, iVec2 pos, iVec2 size,
                   DataView data,
                   TexDataFormat format,
                   std::size_t row_length = 0);

template<typename Pixel>
void updateTexture(TextureRef const& tex, iVec2 pos, Image<Pixel> const& img) {
  updateTexture(tex, pos, img.size(), img.data(), PixelTraits<Pixel>::getTexDataFormat(), img.getRowLength() / sizeof(Pixel));
}

struct TextureUsage {
  TextureUsage(TextureRef const& tex, int unit = 0);
  ~TextureUsage();
};

}  // namespace render

void loadTexture(std::string const&, std::string const&,
                 std::function<void(render::TextureRef)>);
}  // namespace slt
#endif
