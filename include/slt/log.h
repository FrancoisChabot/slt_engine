#ifndef SLT_LOG_LOG_H
#define SLT_LOG_LOG_H

#include "slt/settings.h"

#include "spdlog/fmt/ostr.h"
#include "spdlog/spdlog.h"

namespace slt {

// The global log instance. See the declaration of spdlog::logger for usage.
extern std::shared_ptr<spdlog::logger> log;

namespace settings {
extern Setting<bool> log_async;
extern Setting<int32_t> async_log_queue;
}
}  // namespace slt

#endif
