#include "slt/debug/assert.h"
namespace slt {

AssertionFailure::AssertionFailure(std::string const& cond,
                                   std::string const& file,
                                   std::string const& line)
    : std::runtime_error(std::string("assertion failure:\n  ") + file + ":" +
                         line + "\n  " + cond) {}
}  // namespace slt
