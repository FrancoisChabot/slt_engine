#ifndef SLT_RUNTIME_RESOURCE_REGISTRY_H
#define SLT_RUNTIME_RESOURCE_REGISTRY_H

#include <functional>
#include <list>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>

#include "slt/debug/assert.h"

namespace slt {

template <typename T>
using ResourceRef = std::shared_ptr<T>;

template <typename T>
using ResourceLoadCb = std::function<void(ResourceRef<T>)>;

template <typename T>
using ResourceFactory =
    std::function<void(std::string const&, std::string const&,
                       std::function<void(ResourceRef<T>)>)>;

class TypedRegistry {
 public:
  virtual ~TypedRegistry() {}
};

template <typename T>
using pending_res_load_queue_t =
    std::unordered_map<std::string, std::list<ResourceLoadCb<T>>>;

class ResourceRegistry : public std::enable_shared_from_this<ResourceRegistry> {
 public:
  ResourceRegistry(std::string name) : name_(std::move(name)) {}
  // This is prefered interface in general.
  // If the resource is currently loaded, then cb will be invoked immediately.
  // Otherwise, either cb will be invoked in the main event queue.
  //
  // If the load fails, or the object fails to be initialized, cb will be
  // invoked
  // with a default constructed ResourceRef (effectively a null pointer).
  //
  // TODO:
  //   - add a priorisation mechanism
  //   - add a cancellation mechanism
  template <typename T>
  inline void get(std::string const& name,
                  std::function<void(ResourceRef<T>)> cb);

  template <typename T>
  inline void addOnDemandType(ResourceFactory<T>);

  // This convenience function will throw if the resource is not currently
  // loaded.
  // It should only be used on Registries that have been preloaded.
  template <typename T>
  ResourceRef<T> getPreloaded(std::string const& name);

  std::string const& name() const { return name_; }

 private:
  std::string name_;
  std::unordered_map<std::type_index, std::unique_ptr<TypedRegistry>>
      registries_;
};

enum class RegistryLoadMode { ON_DEMAND, PRELOAD };

std::shared_ptr<ResourceRegistry> addRegistry(std::string const& name,
                                              RegistryLoadMode mode);

// This only delinks the registry, it will only be actually removed when all
// pending loads on it have been completed/cancelled. Also, any used resource
// will remain until removed.
void removeRegistry(std::string const& name);

// Obtain a registry of a given name.
std::shared_ptr<ResourceRegistry> getRegistry(std::string const& name);

// Implementation details, do not worry too much about this.

template <typename T>
class TypedRegistryInterface : public TypedRegistry {
 public:
  virtual void get(std::string const& name, ResourceLoadCb<T> cb,
                   ResourceRegistry* owner) = 0;

  virtual ResourceRef<T> getPreloaded(std::string const& name) = 0;
};

template <typename T>
class OnDemandTypedRegistry : public TypedRegistryInterface<T> {
 public:
  OnDemandTypedRegistry(ResourceFactory<T> fac) : factory_(std::move(fac)) {}

  void get(std::string const& name, ResourceLoadCb<T> cb,
           ResourceRegistry* owner) override {
    auto found = loaded_.find(name);

    if (found != loaded_.end()) {
      auto locked = found->second.lock();
      if (locked) {
        cb(std::move(locked));
        return;
      }
    }
    auto pending_found = pending_.find(name);
    if (pending_found != pending_.end()) {
      pending_found->second.emplace_back(std::move(cb));
    } else {
      pending_[name].emplace_back(std::move(cb));

      // Prevent the registry from being deleted while there is an asynchronous
      // load
      // operation pending on it.
      auto owner_ref = owner->shared_from_this();
      factory_(owner->name(), name, [owner_ref, this, name](ResourceRef<T> r) {
        for (auto const& cb : pending_[name]) {
          cb(r);
        }
        pending_.erase(name);
      });
    }

    return;
  }

  ResourceRef<T> getPreloaded(std::string const& name) override {
    // We can theoretically implement this, but its use is heavily
    // discouraged at the moment.

    // If you run into a situation where you are hitting this and feel
    // like you have a legitimate use-case, feel free to plead your case.
    SLT_ASSERT(false);
    throw std::runtime_error(
        "Requesting pre-loaded resource from on-demand registry");
  }

 private:
  ResourceFactory<T> factory_;
  std::unordered_map<std::string, std::weak_ptr<T>> loaded_;
  pending_res_load_queue_t<T> pending_;
};

template <typename T>
class PreloadedTypedRegistry : public TypedRegistryInterface<T> {
 public:
  void get(std::string const& name, ResourceLoadCb<T> cb,
           ResourceRegistry*) override {
    auto found = loaded_.find(name);
    if (found == loaded_.end) {
      // if it's not currently loaded, then there's not much we can do about it.
      cb(ResourceRef<T>());
    } else {
      cb(found->second);
    }
  }

  ResourceRef<T> getPreloaded(std::string const& name) override {
    return loaded_.at(name);
  }

 private:
  std::unordered_map<std::string, std::shared_ptr<T>> loaded_;
};

template <typename T>
void ResourceRegistry::get(std::string const& name,
                           std::function<void(ResourceRef<T>)> cb) {
  auto found = registries_.find(std::type_index(typeid(T)));
  if (found == registries_.end()) {
    throw std::runtime_error("unexpected resource type");
  }

  TypedRegistryInterface<T>* handler =
      static_cast<TypedRegistryInterface<T>*>(found->second.get());
  return handler->get(name, std::move(cb), this);
}

template <typename T>
void ResourceRegistry::addOnDemandType(ResourceFactory<T> fac) {
  registries_[std::type_index(typeid(T))] =
      std::make_unique<OnDemandTypedRegistry<T>>(std::move(fac));
}
}

#endif
