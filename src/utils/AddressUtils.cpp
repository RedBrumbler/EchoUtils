#include "utils/AddressUtils.hpp"
#include "log.h"

#include <dlfcn.h>

namespace EchoUtils {
std::unordered_map<std::string, uintptr_t> AddressUtils::bases = {};
std::unordered_map<std::string, std::string> AddressUtils::name_remap = { { "libr15.so", "libr15-original.so" } };

uintptr_t AddressUtils::get_base_uncached(std::string library) {
  auto remapped = get_remapped(library);
  return remapped.has_value() ? get_base_internal(remapped.value()) : get_base_internal(library);
}

uintptr_t AddressUtils::get_base(std::string library) {
  auto remapped = get_remapped(library);
  return remapped.has_value() ? get_base_cached(remapped.value()) : get_base_cached(library);
}

uintptr_t AddressUtils::get_offset(std::string library, long offset) {
  auto base = get_base(std::string(library));
  if (!base) return 0;
  LOG_DEBUG("Generated offset pointer for library {}: 0x{:x}", library, offset);
  return base + offset;
}

void AddressUtils::remap_name(std::string given, std::string actual) {
  LOG_DEBUG("Remapping name from {} to {}", given, actual);
  name_remap[given] = actual;
}

std::optional<std::string> AddressUtils::get_remapped(std::string given) {
  auto itr = name_remap.find(given);
  if (itr != name_remap.end()) {
    LOG_DEBUG("Retreived remap from {} to {}", given, itr->second);
    return itr->second;
  }
  return std::nullopt;
}

uintptr_t AddressUtils::get_base_cached(std::string library) {
  auto itr = bases.find(library);

  // found a cached value
  if (itr != bases.end()) {
    return itr->second;
  }

  auto base = get_base_internal(library);

  // if found
  if (base) {
    bases[library] = base;
    LOG_DEBUG("Found base for {}: 0x{:x}", library, base);
  } else {
    LOG_WARN("Could not find base for {}", library);
  }

  return base;
}

// credits to https://github.com/ikoz/AndroidSubstrate_hookingC_examples/blob/master/nativeHook3/jni/nativeHook3.cy.cpp
uintptr_t AddressUtils::get_base_internal(std::string library) {
  if (library.empty()) return (uintptr_t)NULL;
  if (dlopen(library.c_str(), RTLD_NOLOAD) == 0) return (uintptr_t)NULL;
  char const* soname = library.c_str();
  FILE* f = NULL;
  char line[200] = { 0 };
  char* state = NULL;
  char* tok = NULL;
  char* baseAddr = NULL;
  if ((f = fopen("/proc/self/maps", "r")) == NULL) return (uintptr_t)NULL;
  while (fgets(line, 199, f) != NULL) {
    tok = strtok_r(line, "-", &state);
    baseAddr = tok;
    strtok_r(NULL, "\t ", &state);
    strtok_r(NULL, "\t ", &state);        // "r-xp" field
    strtok_r(NULL, "\t ", &state);        // "0000000" field
    strtok_r(NULL, "\t ", &state);        // "01:02" field
    strtok_r(NULL, "\t ", &state);        // "133224" field
    tok = strtok_r(NULL, "\t ", &state);  // path field

    if (tok != NULL) {
      int i;
      for (i = (int)strlen(tok) - 1; i >= 0; --i) {
        if (!(tok[i] == ' ' || tok[i] == '\r' || tok[i] == '\n' || tok[i] == '\t')) break;
        tok[i] = 0;
      }
      {
        size_t toklen = strlen(tok);
        size_t solen = strlen(soname);
        if (toklen > 0) {
          if (toklen >= solen && strcmp(tok + (toklen - solen), soname) == 0) {
            fclose(f);
            return (uintptr_t)strtoll(baseAddr, NULL, 16);
          }
        }
      }
    }
  }
  fclose(f);
  return (uintptr_t)NULL;
}

}  // namespace EchoUtils
