#include "utils/Callbacks.hpp"
#include "utils/HandleUtils.hpp"

#include "echo-utils.hpp"
#include "log.h"

#include "flamingo/shared/trampoline-allocator.hpp"
#include "flamingo/shared/trampoline.hpp"

namespace EchoUtils {
std::unordered_map<std::string, Callbacks::all_callback_func_t> Callbacks::all_cbs = {};
std::unordered_map<std::string, std::unordered_map<std::string, Callbacks::library_callback_func_t>>
    Callbacks::lib_cbs = {};

void Callbacks::add_all_callback(std::string identifier, all_callback_func_t cb) {
  LOG_INFO("Registering all libs callback: {}", identifier);
  all_cbs[identifier] = cb;
}

void Callbacks::remove_all_callback(std::string identifier) {
  LOG_INFO("Removing all libs callback: {}", identifier);
  all_cbs.erase(identifier);
}

void Callbacks::add_library_callback(std::string identifier, std::string library, library_callback_func_t cb) {
  LOG_INFO("Registering lib callback: {} - {}", identifier, library);
  lib_cbs[library][identifier] = cb;
}

void Callbacks::remove_library_callback(std::string identifier, std::string library) {
  LOG_INFO("Removing lib callback: {} - {}", identifier, library);
  auto libmap_itr = lib_cbs.find(library);
  if (libmap_itr == lib_cbs.end()) return;

  libmap_itr->second.erase(identifier);

  if (libmap_itr->second.empty()) {
    lib_cbs.erase(library);
  }
}

void Callbacks::invoke_callbacks(std::string library, void* handle) {
  LOG_DEBUG("Invoking callbacks for {} with handle {}", library, fmt::ptr(handle));
  for (auto const& [_, cb] : all_cbs) cb(library, handle);

  auto individual_cbs_itr = lib_cbs.find(library);

  if (individual_cbs_itr != lib_cbs.end()) {
    for (auto const& [_, cb] : individual_cbs_itr->second) cb(handle);
  }
}
}  // namespace EchoUtils

void install_cbs_hook() {
  auto symbol_lookup = EchoUtils::HandleUtils::get_symbol_address(
      "libr15.so", "_ZN10NRadEngine10CSysModule4LoadERKNS_13CFixedStringTILy512EEEPvjj");

  auto target = (uint32_t*)symbol_lookup.first;
  auto& error = symbol_lookup.second;

  if (!target) {
    LOG_ERROR("Symbol lookup failed so library load hook was not installed: {}", error);
    return;
  }
  if (!EchoUtils::protect(target, EchoUtils::RWX)) return;

  static auto trampoline = flamingo::TrampolineAllocator::Allocate(128);
  trampoline.WriteHookFixups(target);
  trampoline.WriteCallback(&target[4]);
  trampoline.Finish();

  static auto csysmodule_load = [](char const* libname, void* param_2, uint param_3, uint param_4) noexcept -> void* {
    auto handle = reinterpret_cast<void* (*)(char const*, void*, uint, uint)>(trampoline.address.data())(
        libname, param_2, param_3, param_4);

    LOG_DEBUG("Library {} was loaded", libname);
    EchoUtils::Callbacks::invoke_callbacks(libname, handle);

    return handle;
  };

  size_t trampoline_size = 64;
  auto target_hook = flamingo::Trampoline(target, 8, trampoline_size);
  target_hook.WriteCallback(reinterpret_cast<uint32_t*>(+csysmodule_load));
  target_hook.Finish();

  EchoUtils::protect(target, EchoUtils::RX);
}
