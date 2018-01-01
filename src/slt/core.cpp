#include "slt/core.h"

#include "slt/debug/assert.h"
#include "slt/file/file_internal.h"
#include "slt/log/log_internal.h"
#include "slt/settings/settings_internal.h"

namespace {
std::vector<std::string_view> argvToArgs(int argc, const char* argv[]) {
  std::vector<std::string_view> args(argc - 1);
  for (int i = 1; i < argc; ++i) {
    args[i - 1] = argv[i];
  }
  return args;
}
}  // namespace

namespace slt {

Core* Core::instance = nullptr;

Core::Core(int argc, const char* argv[]) : Core(argvToArgs(argc, argv)) {}

Core::Core(std::vector<std::string_view> const& args) {
  // There should only ever be a single core instance in existence.
  SLT_ASSERT(instance == nullptr);
  instance = this;
  logging::preInit();

  settings::init(args);
  logging::init();

  file::startFilesystemThread();
}

Core::~Core() {
  file::stopFilesystemThread();
  instance = nullptr;

  settings::resetAll();
  logging::shutdown();
}

// void Core::update() {
// main_queue.executeAll();
//}
}  // namespace slt
