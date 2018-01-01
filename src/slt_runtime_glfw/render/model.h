#ifndef SLT_RUNTIME_GLFW_PROGRAM_H
#define SLT_RUNTIME_GLFW_PROGRAM_H

#include "slt/mem/data_view.h"
#include "slt_runtime/drawing/model.h"
#include "slt_runtime_glfw/render/gl_includes.h"

namespace slt {
namespace render {
class GLFWModel : public slt::render::Model {
 public:
  GLFWModel(DataView data);
  GLFWModel(render::PrimitiveType, std::vector<render::VertexDescriptor> const&,
            DataView vertex_data, DataView index_data, int index_count);
  ~GLFWModel();

  GLuint vao_ = 0;
  GLuint vertex_vbo_ = 0;
  GLuint indices_vbo_ = 0;

  GLenum prim_;
  GLenum index_type_;
  int index_count_;
};
}
}

#endif