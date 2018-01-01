#include "slt_runtime/drawing/model.h"
#include "slt_runtime/drawing/attribute.h"
#include "slt_runtime_glfw/render/model.h"

namespace slt {
namespace render {
GLFWModel::GLFWModel(DataView data) {
  SLT_ASSERT(false);  // not implemented yet
}

GLFWModel::GLFWModel(render::PrimitiveType prim,
                     std::vector<render::VertexDescriptor> const &desc,
                     DataView vertex_data, DataView index_data,
                     int index_count) {
  log->info("building model from memory");
  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vertex_vbo_);
  glGenBuffers(1, &indices_vbo_);

  try {
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo_);
    log->info("{} bytes of vertex data", vertex_data.size());
    glBufferData(GL_ARRAY_BUFFER, vertex_data.size(), vertex_data.data(),
                 GL_STATIC_DRAW);

    unsigned int stride = 0;
    for (auto const &attr : desc) {
      stride += sizeof(float) * attr.dimensions;
    }

    size_t offset = 0;
    for (auto const &attr : desc) {
      auto bind_location = getAttributeBindLoc(attr.channel);
      log->info("enabling {}", bind_location);
      glVertexAttribPointer(bind_location, attr.dimensions, GL_FLOAT, GL_FALSE,
                            stride, (const void *)offset);
      offset += sizeof(float) * attr.dimensions;
      glEnableVertexAttribArray(bind_location);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo_);
    log->info("{} bytes of index data", index_data.size());
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_data.size(), index_data.data(),
                 GL_STATIC_DRAW);

    prim_ = GL_TRIANGLES;
    index_count_ = index_count;
    switch (index_data.size() / index_count_) {
      case 1:
        index_type_ = GL_UNSIGNED_BYTE;
        break;
      case 2:
        index_type_ = GL_UNSIGNED_SHORT;
        break;
      case 4:
        index_type_ = GL_UNSIGNED_INT;
        break;
      default:
        throw std::runtime_error("bad index data");
    }
  } catch (...) {
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vertex_vbo_);
    glDeleteBuffers(1, &indices_vbo_);
    throw;
  }
}

GLFWModel::~GLFWModel() {
  if (vao_) {
    glDeleteVertexArrays(1, &vao_);
  }
  if (vertex_vbo_) {
    glDeleteBuffers(1, &vertex_vbo_);
  }
  if (indices_vbo_) {
    glDeleteBuffers(1, &indices_vbo_);
  }
}

void draw(slt::render::ModelRef const &m_raw) {
  GLFWModel *model = static_cast<GLFWModel *>(m_raw.get());
  glBindVertexArray(model->vao_);
  glDrawElements(model->prim_, model->index_count_, model->index_type_, 0);
}
}

void loadModel(std::string const &, std::string const &,
               std::function<void(render::ModelRef)>) {}

render::ModelRef createModel(render::PrimitiveType prim,
                             std::vector<render::VertexDescriptor> const &desc,
                             DataView vertex_data, DataView index_data,
                             int idx_count) {
  return std::make_shared<render::GLFWModel>(prim, desc, vertex_data,
                                             index_data, idx_count);
}
}