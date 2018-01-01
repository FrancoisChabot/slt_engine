#include "slt_runtime/resources.h"
#include "slt/log.h"
#include "slt_runtime/drawing/program.h"
#include "slt_runtime/drawing/texture.h"

#include <mutex>

namespace slt {

namespace {
std::unordered_map<std::string, std::shared_ptr<ResourceRegistry>> registries_;
std::mutex registry_lookup_mutex;

void populate_on_demand_registry(ResourceRegistry& tgt) {
  tgt.addOnDemandType<render::Texture>(loadTexture);
  tgt.addOnDemandType<render::Program>(loadProgram);
}

void populate_preload_registry(ResourceRegistry& tgt) {
  //    tgt.addPreloadType<render::Texture>();
  //  tgt.addPreloadType<render::Program>();
}
}

std::shared_ptr<ResourceRegistry> addRegistry(std::string const& name,
                                              RegistryLoadMode mode) {
  std::unique_lock lock(registry_lookup_mutex);
  auto found = registries_.find(name);
  if (found != registries_.end()) {
    SLT_ASSERT(false);
    throw std::runtime_error("registry is already loaded");
  }

  log->info("creating registry: {}", name);
  auto new_reg = std::make_shared<ResourceRegistry>(name);
  registries_.emplace(name, new_reg);

  if (mode == RegistryLoadMode::PRELOAD) {
    populate_preload_registry(*new_reg);
  } else {
    populate_on_demand_registry(*new_reg);
  }

  return new_reg;
}

void removeRegistry(std::string const& name) {
  std::unique_lock lock(registry_lookup_mutex);
  registries_.erase(name);
}

std::shared_ptr<ResourceRegistry> getRegistry(std::string const& name) {
  std::unique_lock lock(registry_lookup_mutex);
  return registries_.at(name);
}
}
