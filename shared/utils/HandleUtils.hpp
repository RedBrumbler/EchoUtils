#pragma once

#include <string>
#include <unordered_map>
#include <utility>

namespace EchoUtils {
/// @brief struct providing various utils to interact with library handles, caching handles, but never opening them
struct HandleUtils {
  /// @brief gets the handle regardless of cached status, but does check remapping
  static void* get_handle_uncached(std::string library);

  /// @brief gets the handle from the cache, or caches it if it wasn't already
  static void* get_handle(std::string library);

  /// @brief gets an address from a given library
  /// @param library the library to look in
  /// @param symbol the symbol to look up in that library
  /// @return a pair of the symbol address and empty string for no error. 0 and error string if error
  static std::pair<uintptr_t, std::string> get_symbol_address(std::string library, std::string_view symbol);

  /// @brief lets you remap the name for a given library
  /// @param given the name to remap to something else
  /// @param actual the name to use when given is passed
  static void remap_name(std::string given, std::string actual);

  /// @brief gets the remapped name for the given string, if it exists
  /// @param given the given name as passed to remap_name
  /// @return remapped name, or nullopt if not remapped
  static std::optional<std::string> get_remapped(std::string given);

 private:
  static std::unordered_map<std::string, void*> handles;
  static std::unordered_map<std::string, std::string> name_remap;

  /// @brief gets the handle from the cache or caches if not found
  static void* get_handle_cached(std::string library);

  /// @brief actually performs the operation to get the void*
  static void* get_handle_internal(std::string library);
};
}  // namespace EchoUtils
