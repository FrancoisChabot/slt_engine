#ifndef SLT_ANGLE_H
#define SLT_ANGLE_H

#include <iostream>
#include <type_traits>

#include "slt/debug/invariant.h"
#include "slt/math/pi.h"

namespace slt {
// Angle_ts always maintain their internal representation as radians in the
// ]-PI,PI] range.
//
// Arrays and raw values:
//   A contiguous array of Angle_ts can be interpreted as a contigous array of
// radians. This can be convenient to send Angle_ts to a Render Program without
// having to do any memory copies.
//   IMPORTANT: Angle_ts reinterpreted this way should only ever be read, never
// written to. This is because every Angle_t operation assumes that the value
// invariant is  maintained.
//
// Constructing Angle_ts:
//   Angle_ts are constructed through the degrees() and radians() functions.
// ex:
//  auto Angle_t = degrees(12.0f);
template <typename T>
class Angle_t {
  static_assert(std::is_floating_point<T>::value,
                "Only Angle_ts of floating point types are allowed");

 public:
  T degrees() const;
  T radians() const;

  Angle_t operator-() const;

  bool operator==(Angle_t const&) const;
  bool operator!=(Angle_t const&) const;

  Angle_t operator+(Angle_t const&) const;
  Angle_t& operator+=(Angle_t const&);

  Angle_t operator-(Angle_t const&) const;
  Angle_t& operator-=(Angle_t const&);

  Angle_t operator*(T const&)const;
  Angle_t& operator*=(T const&);

  Angle_t operator/(T const&) const;
  Angle_t& operator/=(T const&);

  template <typename U>
  friend Angle_t<U> radians(U const&);

  template <typename U>
  friend Angle_t<U> degrees(U const&);

  T value_;

 private:
  Angle_t(T const&);
};

template <typename T>
Angle_t<T> radians(T const& v);

template <typename T>
Angle_t<T> degrees(T const& v);

template <typename T>
Angle_t<T>::Angle_t(T const& v) : value_(v) {
  SLT_CHECK_INVARIANT(*this);
}

template <typename T>
T Angle_t<T>::degrees() const {
  return value_ * T(180) / pi<T>();
}

template <typename T>
T Angle_t<T>::radians() const {
  return value_;
}

template <typename T>
Angle_t<T> Angle_t<T>::operator-() const {
  // Special case, we try to keep positive pi.
  return Angle_t<T>(value_ == pi<T>() ? value_ : -value_);
}

template <typename T>
bool Angle_t<T>::operator==(Angle_t const& rhs) const {
  return value_ == rhs.value_;
}

template <typename T>
Angle_t<T> Angle_t<T>::operator+(Angle_t<T> const& rhs) const {
  auto result = *this;
  result += rhs;
  return result;
}

template <typename T>
Angle_t<T>& Angle_t<T>::operator+=(Angle_t<T> const& rhs) {
  value_ += rhs.value_;
  if (value_ > pi<T>) {
    value_ -= two_pi<T>;
  } else if (value_ <= -pi<T>) {
    value_ += two_pi<T>;
  }
  SLT_CHECK_INVARIANT(*this);
  return *this;
}

template <typename T>
Angle_t<T> Angle_t<T>::operator-(Angle_t<T> const& rhs) const {
  auto result = *this;
  result -= rhs;
  return result;
}

template <typename T>
Angle_t<T>& Angle_t<T>::operator-=(Angle_t<T> const& rhs) {
  value_ -= rhs.value_;
  if (value_ > pi<T>) {
    value_ -= two_pi<T>;
  } else if (value_ <= -pi<T>) {
    value_ += two_pi<T>;
  }
  SLT_CHECK_INVARIANT(*this);
  return *this;
}

template <typename T>
Angle_t<T>& Angle_t<T>::operator*=(T const& rhs) {
  value_ *= rhs;
  while (value_ > pi<T>()) {
    value_ -= two_pi<T>();
  }

  while (value_ <= -pi<T>()) {
    value_ += two_pi<T>();
  }
  CHECK_INVARIANT(*this);
  return *this;
}

template <typename T>
Angle_t<T> Angle_t<T>::operator*(T const& rhs) const {
  Angle_t<T> result = *this;
  result *= rhs;
  return result;
}

template <typename T>
Angle_t<T>& Angle_t<T>::operator/=(T const& rhs) {
  value_ /= rhs;
  while (value_ > pi<T>()) {
    value_ -= two_pi<T>();
  }

  while (value_ <= -pi<T>()) {
    value_ += two_pi<T>();
  }
  CHECK_INVARIANT(*this);
  return *this;
}

template <typename T>
Angle_t<T> Angle_t<T>::operator/(T const& rhs) const {
  Angle_t<T> result = *this;
  result /= rhs;
  return result;
}

template <typename T>
Angle_t<T> operator*(T const& lhs, Angle_t<T> const& rhs) {
  return rhs * lhs;
}

template <typename T>
inline bool operator<(Angle_t<T> const& lhs, Angle_t<T> const& rhs) {
  return lhs.value_ < rhs.value_;
}

template <typename T>
inline bool operator>(Angle_t<T> const& lhs, Angle_t<T> const& rhs) {
  return lhs.value_ > rhs.value_;
}

template <typename T>
Angle_t<T> radians(T const& v) {
  T tmp = v;
  while (tmp > pi<T>) {
    tmp -= two_pi<T>;
  }

  while (tmp <= -pi<T>()) {
    tmp += two_pi<T>;
  }
  return Angle_t<T>(tmp);
}

template <typename T>
Angle_t<T> degrees(T const& v) {
  T tmp = v;
  while (tmp > T(180)) {
    tmp -= T(360);
  }

  while (tmp <= T(-180)) {
    tmp += T(360);
  }

  return Angle_t<T>(tmp / T(180) * pi<T>);
}

template <typename T>
void check_invariant(Angle_t<T> const& a) {
  SLS_ASSERT(a.value_ <= pi<T>);
  SLS_ASSERT(a.value_ > -pi<T>);
}

template <typename T>
std::ostream& operator<<(std::ostream& stream, Angle_t<T> const& Angle_t) {
  stream << Angle_t.degrees() << "°";
  return stream;
}

using Angle = Angle_t<float>;
}  // namespace slt

#endif
