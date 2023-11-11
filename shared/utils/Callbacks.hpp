#pragma once

#include <functional>
#include <list>
#include <string>

/// @brief installs a hook for when libs are loaded
extern void install_cbs_hook();

namespace EchoUtils {
struct Callbacks {
  using all_callback_func_t = std::function<void(std::string, void*)>;
  using library_callback_func_t = std::function<void(void*)>;

  /// @brief adds a callback that gets called for all libs the game loads
  /// @param identifier the identifier for your callback, use something unique like your mod id
  /// @param cb the callback that is invoked
  static void add_all_callback(std::string identifier, all_callback_func_t cb);
  /// @brief removes a callback
  /// @param identifier your identifier
  static void remove_all_callback(std::string identifier);

  /// @brief adds a callback that gets called for a specific library
  /// @param identifier the identifier for your callback, use something unique like your mod id
  /// @param library the library you want to be called for
  /// @param cb the callback that is invoked
  static void add_library_callback(std::string identifier, std::string library, library_callback_func_t cb);
  /// @brief removes a callback
  /// @param identifier your identifier
  /// @param library the library for which to remove the callback
  static void remove_library_callback(std::string identifier, std::string library);

 private:
  friend void ::install_cbs_hook();
  /// @brief identifier : cb
  static std::unordered_map<std::string, all_callback_func_t> all_cbs;
  /// @brief lib : { identifier : cb }
  static std::unordered_map<std::string, std::unordered_map<std::string, library_callback_func_t>> lib_cbs;

  static void invoke_callbacks(std::string library, void* handle);
};
}  // namespace EchoUtils
