#ifndef SLT_PI_H
#define SLT_PI_H

namespace slt {
template <typename T = float>
constexpr T pi = T(3.1415926535897932385);

template <typename T = float>
constexpr T two_pi = pi<T>* T(2);

}  // namespace slt
#endif