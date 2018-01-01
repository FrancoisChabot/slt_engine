#include "slt/log.h"

#include "slt/settings.h"

#ifdef WIN32
#include "spdlog/sinks/msvc_sink.h"
#endif

namespace {
std::shared_ptr<spdlog::logger> createLogger(std::string name) {
  // NOTE: This will get called before settings are processed.

  std::shared_ptr<spdlog::logger> result;
#ifdef WIN32
  auto msvc_sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
  result = spdlog::create(
      name, {msvc_sink, spdlog::sinks::stderr_sink_mt::instance()});
#else
  result = spdlog::stdout_logger_mt(name);
#endif

  return result;
}
}  // namespace

namespace slt {
std::shared_ptr<spdlog::logger> log;

namespace logging {
std::shared_ptr<spdlog::logger> init_log;

void preInit() { init_log = createLogger("SLT_init"); }

void init() {
  init_log = nullptr;
  spdlog::drop("SLT_init");
  if (settings::log_async.get()) {
    spdlog::set_async_mode(settings::async_log_queue.get());
  }

  log = createLogger("SLT");

  log->info("Logging initiated");
}

void shutdown() {
  spdlog::set_sync_mode();
  spdlog::drop("SLT");
  log->info("Logging terminated");
  log->flush();

  log = nullptr;
}
}  // namespace logging

namespace settings {
Setting<bool> log_async(
    false, "log_async",
    "Enable async logging. Less impact on performance, but less consistent");

Setting<int32_t> async_log_queue(
    4096, "async_log_queue",
    "If async logging is enabled, sets the lenght of the async buffer.",
    [](int32_t const& v) { return (v & (v - 1)) == 0; });
}
}  // namespace slt
