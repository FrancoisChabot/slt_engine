#ifndef SLT_MATH_VEC_H
#define SLT_MATH_VEC_H

#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

namespace slt {

using iVec2 = glm::ivec2;
using Vec2 = glm::vec2;
using Vec3 = glm::vec3;
using Vec4 = glm::vec4;

using Quat = glm::quat;

using Mat2 = glm::mat2;
using Mat3 = glm::mat3;
using Mat4 = glm::mat4;

struct SQT {
  Vec3 s;
  Vec4 q;
  Vec3 t;
};

}  // namespace slt

inline std::istream& operator>>(std::istream& stream, slt::iVec2& vec) {
  stream >> vec.x;
  stream >> vec.y;
  return stream;
}
#endif