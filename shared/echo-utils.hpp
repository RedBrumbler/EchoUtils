#pragma once

#include "utils/AddressUtils.hpp"
#include "utils/Callbacks.hpp"
#include "utils/HandleUtils.hpp"

#include <sys/mman.h>

namespace EchoUtils {
enum Prot : int {
  R = PROT_READ,
  W = PROT_WRITE,
  X = PROT_EXEC,

  RW = R | W,
  RX = R | X,
  WX = W | X,
  RWX = R | W | X,
};
/// @brief protects the page that address resides on with protection prot
/// @param address the adress to get the page to protect from
/// @param protection the protection to apply
/// @return whether the protection was successful
bool protect(void* address, int protection);
}  // namespace EchoUtils
