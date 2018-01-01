#ifndef SLT_INVARIANT_H
#define SLT_INVARIANT_H

#include "slt/debug/assert.h"

// Invariant testing in SLT is meant to be pervasive, but only enabled in
// validation builds.
//
// Adding an invariant check to a type:
// Overload slt::checkInvariant() for it
//
// Checking the invariant:
// Invoke the CHECK_INVARIANT() macro on an instance
#ifdef SLT_ENABLE_INVARIANT_TESTING
#define SLT_CHECK_INVARIANT(instance) ::slt::checkInvariant(instance)
#else
#define SLT_CHECK_INVARIANT(instance) (void)(instance)
#endif

namespace slt {

// Convenience function to avoid having to declare temporary Lvalues for
// validation.
template <typename T>
inline T verified(T const& v) {
  SLT_CHECK_INVARIANT(v);
  return v;
}

// Do not invoke checkInvariant directly. Use CHECK_INVARIANT or verified
template <typename T>
void checkInvariant(T const&) {}
}  // namespace slt

// Utility function for facilitating unit tests.
#ifdef SLT_ENABLE_INVARIANT_TESTING
#define EXPECT_SLT_INVARIANT_FAILURE(EXPR) \
  EXPECT_THROW((EXPR), ::slt::AssertionFailure)
#else
#define EXPECT_SLT_INVARIANT_FAILURE(EXPR) (EXPR)
#endif
#endif
