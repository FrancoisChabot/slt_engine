#ifndef SLT_SETTINGS_SETTINGS_H
#define SLT_SETTINGS_SETTINGS_H

#include <functional>
#include <iostream>
#include <string>
#include <string_view>

#include "slt/mem/read_buf.h"

namespace slt {
// Error Thrown when something goes wrong will seting up or processing Settings.
struct SettingsError : public std::runtime_error {
  SettingsError(std::string const& err) : std::runtime_error(err) {}
};

class SettingBase {
 public:
  SettingBase(std::string description) : description_(std::move(description)) {}

  virtual ~SettingBase() {}

  std::string const& description() const { return description_; }

  virtual void assign(std::string_view str) = 0;
  virtual void reset() = 0;

 protected:
  static void registerSetting(SettingBase*, std::string const&);

 private:
  std::string description_;
};

// Settings are self-registering global properties that will be set when the slt
// Core is being initialized.
//
// Settings should always be defined as global variables.
// Settings will have their default until the core has been initialized, and
// after the core has shut down.
template <typename T>
class Setting : public SettingBase {
 public:
  using Validator = std::function<bool(T const&)>;

  Setting(T default_val, std::string name, std::string description,
          Validator validator = nullptr)
      : SettingBase(description),
        default_(default_val),
        value_(default_val),
        validator_(validator) {
    if (validator_ && !validator_(default_val)) {
      throw SettingsError("attempting to set invalid default value");
    }

    registerSetting(this, std::move(name));
  }

  // REtrieve the value of the setting.
  T const& get() const { return value_; }

  // Sets the value of the setting.
  void set(T const& v) {
    if (validator_ && !validator_(v)) {
      throw SettingsError("attempting to set invalid value");
    }
    value_ = v;
  }

  // Set the value of the setting using a string representation of the value.
  void assign(std::string_view str) override {
    T val;
    slt::ReadBuf rd_buf(str.data(), str.length());
    std::istream stream(&rd_buf);
    stream >> val;
    // deserialize from string
    set(val);
  }

  // Resets the setting to whatever its default is.
  void reset() { value_ = default_; }

  // CHanges the default value of the setting. Be carefull, this change does NOT
  // revert when resetting the setting.
  void setDefault(T const& new_val) {
    if (validator_ && !validator_(new_val)) {
      throw SettingsError("attempting to set invalid default value");
    }
    default_ = new_val;
    value_ = new_val;
  }

 private:
  T default_;
  T value_;
  Validator validator_;
};
}  // namespace slt

#endif
