#ifndef SLT_RUNTIME_RENDER_ATTRIBUTE_H
#define SLT_RUNTIME_RENDER_ATTRIBUTE_H

#include <cstdint>
#include <string>

namespace slt {
namespace render {
void registerAttribute(std::string, std::uint16_t);
std::uint16_t getAttributeBindLoc(std::string const&);
}  // namespace render
}  // namespace slt
#endif