#include "slt/settings.h"
#include "slt/log.h"
#include "slt/log/log_internal.h"
#include "slt/settings/settings_internal.h"
#include "slt/str_manip/split.h"

#include <iostream>
#include <map>
#include <string>

namespace {
using settings_store_t =
    std::map<std::string, ::slt::SettingBase*, std::less<void>>;
settings_store_t& settingsStore() {
  static settings_store_t instance;
  return instance;
}
}  // namespace

namespace slt {

namespace settings {

void init(std::vector<std::string_view> const& args) {
  auto& store = settingsStore();

  logging::init_log->info("Begin args:");
  for (auto arg : args) {
    logging::init_log->info("arg: {}", arg);

    if (arg.find_first_of("--") == 0) {
      arg = arg.substr(2);
      auto parts = slt::split(arg, std::string_view("="));

      std::vector<std::string_view> parts_v(parts.begin(), parts.end());

      if (parts_v.size() == 2) {
        auto found = store.find(parts_v[0]);
        if (found == store.end()) {
          throw SettingsError(fmt::format("unknown setting: {}", arg));
        } else {
          found->second->assign(parts_v[1]);
        }
      } else {
        throw SettingsError(fmt::format("Badly formatted setting: {}", arg));
      }
    } else {
      throw SettingsError(fmt::format("Badly formatted setting: {}", arg));
    }
  }
  logging::init_log->info("Done parsing args");
}

void resetAll() {
  for (auto& setting : settingsStore()) {
    setting.second->reset();
  }
}

}  // namespace settings

void SettingBase::registerSetting(SettingBase* setting,
                                  std::string const& name) {
  auto& store = settingsStore();

  if (store.find(name) != store.end()) {
    throw SettingsError(
        fmt::format("trying to register the same setting twice: {}", name));
  }

  store[name] = setting;
}

}  // namespace slt