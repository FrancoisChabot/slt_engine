#ifndef SLT_ASSERT_H
#define SLT_ASSERT_H

#include <stdexcept>
#include <string>

namespace slt {

class AssertionFailure : public std::runtime_error {
 public:
  AssertionFailure(std::string const& cond, std::string const& file,
                   std::string const& line);
};
}  // namespace slt

#ifndef NDEBUG
#define SLT_ENABLE_ASSERTS
#endif

#if defined(SLT_ENABLE_INVARIANT_TESTING) && !defined(SLT_ENABLE_ASSERTS)
#define SLT_ENABLE_ASSERTS
#endif

#ifdef SLT_ENABLE_ASSERTS

//
#ifdef SLT_ASSERT_THROWS
#define SLT_ASSERT(condition)                                \
  if (!(condition)) {                                        \
    throw ::slt::AssertionFailure(#condition, __FILE__,      \
                                  std::to_string(__LINE__)); \
  }                                                          \
// end of macro
#else
#define SLT_ASSERT(condition) \
  if (!(condition)) {         \
    std::terminate();         \
  }                           \
// end of macro
#endif
#else
#define SLT_ASSERT(condition) (void)(condition)
#endif

#define SLT_UNREACHABLE() \
  SLT_ASSERT(false)       \
// end of macro

#define SLT_ASSERT_EQ(LHS, RHS) \
  SLT_ASSERT((LHS) == (RHS))    \
// end of macro

#define SLT_ASSERT_GT(LHS, RHS) \
  SLT_ASSERT((LHS) > (RHS))     \
// end of macro

#define SLT_ASSERT_GE(LHS, RHS) \
  SLT_ASSERT((LHS) >= (RHS))    \
// end of macro

#define SLT_ASSERT_LT(LHS, RHS) \
  SLT_ASSERT((LHS) < (RHS))     \
// end of macro

#define SLT_ASSERT_LE(LHS, RHS) \
  SLT_ASSERT((LHS) <= (RHS))    \
// end of macro

// TODO: This isnt't exactly great, gtest has a much better way of dynamically
// determining EPSILON, but this serves our purposes for now.
#define SLT_ASSERT_FLOAT_EQ(LHS, RHS)              \
  SLT_ASSERT(std::abs((LHS) - (RHS)) < 0.000001f); \
// end of macro

// Utility function for facilitating unit tests.
#ifdef SLT_ENABLE_ASSERTS
#define EXPECT_SLT_ASSERT_FAILURE(EXPR) \
  EXPECT_THROW((EXPR), ::slt::AssertionFailure)
#else
#define EXPECT_SLT_ASSERT_FAILURE(EXPR) (EXPR)
#endif

#endif
