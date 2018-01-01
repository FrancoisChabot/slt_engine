#ifndef SLT_COLOR_H
#define SLT_COLOR_H

#include "slt/math/vec.h"

namespace slt {
using Color = Vec4;

static const Color red = Color{1.0f, 0.0f, 0.0f, 1.0f};
static const Color green = Color{0.0f, 1.0f, 0.0f, 1.0f};
static const Color blue = Color{0.0f, 0.0f, 1.0f, 1.0f};
static const Color white = Color{1.0f, 1.0f, 1.0f, 1.0f};
static const Color black = Color{0.0f, 0.0f, 0.0f, 1.0f};
}

#endif