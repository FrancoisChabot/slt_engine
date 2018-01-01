#ifndef SLT_SETTINGS_INTERNAL_H
#define SLT_SETTINGS_INTERNAL_H

#include <string_view>
#include <vector>

namespace slt {
namespace settings {
void init(std::vector<std::string_view> const& argv);

// reset all registered settings to their default values.
void resetAll();
}  // namespace settings
}  // namespace slt

#endif