#ifndef SLT_RUNTIME_RENDER_H
#define SLT_RUNTIME_RENDER_H

#include "slt/color.h"
#include "slt_runtime/drawing/model.h"

namespace slt {
namespace render {

struct FrameScope {
  FrameScope();
  ~FrameScope();
};

void clearFrame(Color const&);
void draw(ModelRef const&);
}
}

#endif
