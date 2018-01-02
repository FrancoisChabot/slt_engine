#include "slt/mem/capnputils.h"
#include "slt_capnp/resources.capnp.h"
#include "slt_runtime/resources.h"
#include "slt_runtime/runtime.h"
#include "slt/log.h"
#include "slt/file/read.h"
#include "slt_runtime/drawing/program.h"
#include "slt_runtime/drawing/texture.h"
#include "slt_runtime/gui/font.h"


#include <mutex>

namespace slt {

namespace {
std::unordered_map<std::string, std::shared_ptr<ResourceRegistry>> registries_;
std::mutex registry_lookup_mutex;

void populate_on_demand_registry(ResourceRegistry& tgt) {
  tgt.addOnDemandType<render::Texture>(loadTexture);
  tgt.addOnDemandType<render::Program>(loadProgram);
  tgt.addOnDemandType<gui::FontData>(loadFontData);
}

void populate_preload_registry(ResourceRegistry& tgt) {
  auto name = tgt.name();
  auto tgt_ptr = tgt.shared_from_this();

  tgt.addPendingLoad();
  file::asyncRead(name + "/reg.cap", Runtime::instance->getMainQueue(), [tgt_ptr](DataBlock data) {
    auto cap_reader = getCapReader(data);
    auto prg_data = cap_reader.getRoot<data::ResourceRegistry>();
    tgt_ptr->clearPendingLoad();

    if (prg_data.hasImages()) {
      auto reg = tgt_ptr->addPreloadedType<render::Texture>();
      for (auto img : prg_data.getImages()) {
        reg->add(img.getName().cStr(), loadTexture, tgt_ptr.get());
      }
    }
    if (prg_data.hasShaders()) {
      auto reg = tgt_ptr->addPreloadedType<render::Program>();
      for (auto const& prg : prg_data.getShaders()) {
        reg->add(prg.getName().cStr(), loadProgram, tgt_ptr.get());
      }
    }
    if (prg_data.hasFonts()) {
      auto reg = tgt_ptr->addPreloadedType<gui::FontData>();
      for (auto const& fnt : prg_data.getFonts()) {
        reg->add(fnt.getName().cStr(), loadFontData, tgt_ptr.get());
      }
    }
  });
  //    tgt.addPreloadType<render::Texture>();
  //  tgt.addPreloadType<render::Program>();
}
}

std::shared_ptr<ResourceRegistry> addRegistry(std::string const& name,
                                              RegistryLoadMode mode) {
  std::unique_lock<std::mutex> lock(registry_lookup_mutex);
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
  std::unique_lock<std::mutex> lock(registry_lookup_mutex);
  registries_.erase(name);
}

std::shared_ptr<ResourceRegistry> getRegistry(std::string const& name) {
  std::unique_lock<std::mutex> lock(registry_lookup_mutex);
  return registries_.at(name);
}
}
