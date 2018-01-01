#ifndef SLT_LOG_LOG_INTERNAL_H
#define SLT_LOG_LOG_INTERNAL_H

#include "spdlog/fmt/ostr.h"
#include "spdlog/spdlog.h"

#include <memory>

namespace slt {
namespace logging {
// Special logger for use before logging has been formally initialized.
extern std::shared_ptr<spdlog::logger> init_log;

void preInit();
void init();
void shutdown();
}  // namespace logging
}  // namespace slt

#endif
