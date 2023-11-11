#pragma once

#include <optional>
#include <string>
#include <unordered_map>

namespace EchoUtils {
/// @brief struct containing static methods for interacting with base addresses of libraries in a neat way
struct AddressUtils {
  /// @brief gets the base address of the library, checks remaps, not cached
  /// @return base address of the library so offset calculations can be performed, or 0 if the base was not found
  static uintptr_t get_base_uncached(std::string library);

  /// @brief gets the base address of the library, checks remaps
  /// @return base address of the library so offset calculations can be performed, or 0 if the base was not found
  static uintptr_t get_base(std::string library);

  /// @brief gets an address offset off of the base of the given library
  /// @param library the library to start from with base
  /// @param offset the offset to add / subtract from the base
  /// @return base + offset, or 0 if base was not found
  static uintptr_t get_offset(std::string library, long offset);

  /// @brief lets you remap the name for a given library
  /// @param given the name to remap to something else
  /// @param actual the name to use when given is passed
  static void remap_name(std::string given, std::string actual);

  /// @brief gets the remapped name for the given string, if it exists
  /// @param given the given name as passed to remap_name
  /// @return remapped name, or nullopt if not remapped
  static std::optional<std::string> get_remapped(std::string given);

 private:
  static std::unordered_map<std::string, uintptr_t> bases;
  static std::unordered_map<std::string, std::string> name_remap;

  /// @brief gets the base address of the library, checks remaps
  /// @return base address of the library so offset calculations can be performed, or 0 if the base was not found
  static uintptr_t get_base_cached(std::string library);

  /// @brief gets the base address to store in the map
  static uintptr_t get_base_internal(std::string library);
};
}  // namespace EchoUtils
