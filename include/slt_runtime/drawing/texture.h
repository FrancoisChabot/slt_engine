#ifndef SLT_RUNTIME_RENDER_TEXTURE_H
#define SLT_RUNTIME_RENDER_TEXTURE_H

#include <functional>
#include <memory>

namespace slt {
namespace render {

struct Texture {
  virtual ~Texture() {}
};

using TextureRef = std::shared_ptr<Texture>;

struct TextureUsage {
  TextureUsage(TextureRef const& tex, int unit = 0);
  ~TextureUsage();
};

}  // namespace render

void loadTexture(std::string const&, std::string const&,
                 std::function<void(render::TextureRef)>);
}  // namespace slt
#endif