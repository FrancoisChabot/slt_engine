#include "slt_runtime/drawing/attribute.h"
#include "slt/debug/assert.h"

#include <unordered_map>

namespace {
std::unordered_map<std::string, std::uint16_t> attributes_;
}

namespace slt {
namespace render {
void registerAttribute(std::string name, uint16_t bind_loc) {
  auto found = attributes_.find(name);

  if (found != attributes_.end()) {
    SLT_ASSERT(found->second == bind_loc);
  } else {
    attributes_.emplace(std::move(name), bind_loc);
  }
}

std::uint16_t getAttributeBindLoc(std::string const& name) {
  auto bind_loc = attributes_.find(name);
  if (bind_loc == attributes_.end()) {
    throw std::runtime_error("Unknown Attribute");
  }
  return bind_loc->second;
}
}  // namespace render
}  // namespace slt