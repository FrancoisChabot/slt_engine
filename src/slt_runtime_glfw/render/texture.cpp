#include "slt_runtime/drawing/texture.h"
#include "lodepng.h"
#include "slt/file/read.h"
#include "slt/log.h"
#include "slt_runtime/runtime.h"
#include "slt_runtime_glfw/render/texture.h"

#include <unordered_map>

namespace {
std::unordered_map<std::string, std::weak_ptr<slt::render::GLFWTexture>>
    loaded_textures_;

std::string getFileForTexture(std::string const& package,
                              std::string const& name) {
  return package + "/images/" + name + ".png";
}
}

namespace slt {
namespace render {
GLFWTexture::GLFWTexture(DataView data) {
  glGenTextures(1, &handle_);

  unsigned char* decoded_data = nullptr;
  unsigned w = 0;
  unsigned h = 0;

  log->info("Decoding texture...");
  auto status = lodepng_decode32(
      &decoded_data, &w, &h, (const unsigned char*)data.data(), data.size());

  if (status != 0) {
    log->error("failed to decode image: {}", status);
    glDeleteTextures(1, &handle_);
    throw std::runtime_error("failed to decode image");
  }
  log->info("success...");

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, handle_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               decoded_data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  free(decoded_data);
}

GLFWTexture::~GLFWTexture() { glDeleteTextures(1, &handle_); }

TextureUsage::TextureUsage(TextureRef const& tex, int unit) {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, static_cast<GLFWTexture*>(tex.get())->handle());
}

TextureUsage::~TextureUsage() {}
}

void loadTexture(std::string const& package, std::string const& name,
                 std::function<void(render::TextureRef)> cb) {
  std::string tex_key = package + "/" + name;
  auto found = loaded_textures_.find(tex_key);
  if (found != loaded_textures_.end() && !found->second.expired()) {
    cb(found->second.lock());
  } else {
    slt::file::asyncRead(getFileForTexture(package, name),
                         slt::Runtime::instance->getMainQueue(),
                         [tex_key, cb](slt::DataBlock d) {
                           auto result =
                               std::make_shared<render::GLFWTexture>(d);
                           loaded_textures_[tex_key] = result;
                           cb(result);
                         });
  }
}
}
