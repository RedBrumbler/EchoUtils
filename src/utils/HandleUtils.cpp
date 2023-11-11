#include "utils/HandleUtils.hpp"
#include <dlfcn.h>

#include "log.h"
#include "yodel/shared/modloader.h"

namespace EchoUtils {
std::unordered_map<std::string, void*> HandleUtils::handles = {};
std::unordered_map<std::string, std::string> HandleUtils::name_remap = { { "libr15.so", "libr15-original.so" } };

void* HandleUtils::get_handle_uncached(std::string library) {
  auto remapped = get_remapped(library);
  return remapped.has_value() ? get_handle_internal(remapped.value()) : get_handle_internal(library);
}

void* HandleUtils::get_handle(std::string library) {
  auto remapped = get_remapped(library);
  return remapped.has_value() ? get_handle_cached(remapped.value()) : get_handle_cached(library);
}

std::pair<uintptr_t, std::string> HandleUtils::get_symbol_address(std::string library, std::string_view symbol) {
  auto handle = get_handle(library);
  if (!handle) return { 0, "Library was not loaded" };
  dlerror();
  auto address = (unsigned long)dlsym(handle, symbol.data());
  if (!address) return { 0, dlerror() };
  return { address, "" };
}

void HandleUtils::remap_name(std::string given, std::string actual) {
  LOG_DEBUG("Remapping name from {} to {}", given, actual);
  name_remap[given] = actual;
}

std::optional<std::string> HandleUtils::get_remapped(std::string given) {
  auto itr = name_remap.find(given);
  if (itr != name_remap.end()) {
    LOG_DEBUG("Retreived remap from {} to {}", given, itr->second);
    return itr->second;
  }
  return std::nullopt;
}

void* HandleUtils::get_handle_cached(std::string library) {
  auto itr = handles.find(library);

  // found a cached value
  if (itr != handles.end()) {
    return itr->second;
  }

  auto handle = get_handle_internal(library);

  // if found
  if (handle) {
    handles[library] = handle;
    LOG_DEBUG("Got handle for {}: {}", library, fmt::ptr(handle));
  } else {
    LOG_WARN("Could not get handle for {}", library);
  }

  return handle;
}

void* HandleUtils::get_handle_internal(std::string library) {
  if (library == "libr15.so") return modloader_r15_handle;
  return dlopen(library.c_str(), RTLD_NOLOAD);
}

}  // namespace EchoUtils
