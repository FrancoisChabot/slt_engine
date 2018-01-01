#ifndef SLT_DEBUG_VALIDATED_H
#define SLT_DEBUG_VALIDATED_H

#include "slt/debug/invariant.h"

namespace slt {
template <typename T>
using VALID_T = void(T const&);

// Validated types combine a value and a validating function.
// The validator function will be invoked upon construction and whenever a
// new value is assigned.
// The underlying value may only be read, using operator*(), and never
// modified without reassining an entire new value.
//
// In non-validating optimized builds, performance should be identical to
// using the underlying type directly.
template <typename T, VALID_T<T> VALIDATOR>
struct Validated {
  Validated(Validated const& v) : val_(v.val_) {}

  Validated(Validated&& v) : val_(std::move(v.val_)) {
    // A moved v.val_ could fail validation.
    SLT_CHECK_INVARIANT(v);
  }

  template <typename... ARGS_T>
  Validated(ARGS_T&&... a) : val_(std::forward<ARGS_T>(a)...) {
    SLT_CHECK_INVARIANT(*this);
  }

  Validated& operator=(Validated const& v) {
    val_ = v.val_;
    return *this;
  }

  Validated& operator=(Validated&& v) {
    val_ = std::move(v.val_);
    SLT_CHECK_INVARIANT(v);
    return *this;
  }

  Validated& operator=(T const& v) {
    val_ = v;
    SLT_CHECK_INVARIANT(*this);
    return *this;
  }

  Validated& operator=(T&& v) {
    val_ = std::move(v);
    SLT_CHECK_INVARIANT(*this);
    return *this;
  }

  T const& operator*() const { return val_; }

  T const* operator->() const { return &val_; }

 private:
  T val_;

  template <typename U, VALID_T<U> VALIDATOR_ARG>
  friend void checkInvariant(Validated<U, VALIDATOR_ARG> const& v);
};

template <typename T, VALID_T<T> VALIDATOR>
void checkInvariant(Validated<T, VALIDATOR> const& v) {
  SLT_CHECK_INVARIANT(v.val_);
  VALIDATOR(*v);
}
}  // namespace slt

#endif
