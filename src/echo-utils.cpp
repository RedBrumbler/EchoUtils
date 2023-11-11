#include "echo-utils.hpp"

#include "log.h"

namespace EchoUtils {
bool protect(void* address, int protection) {
  constexpr static auto kPageSize = 4096ULL;

  auto* page_aligned_target = reinterpret_cast<uint32_t*>(reinterpret_cast<uint64_t>(address) & ~(kPageSize - 1));
  LOG_DEBUG("Marking address: {}, 0b{:b}, page aligned: {}", fmt::ptr(address), protection,
            fmt::ptr(page_aligned_target));
  if (::mprotect(page_aligned_target, kPageSize, protection) != 0) {
    // Log error on mprotect!
    LOG_ERROR("Failed to mark: {} (page aligned: {}). err: {}", fmt::ptr(address), fmt::ptr(page_aligned_target),
              std::strerror(errno));
    return false;
  }
  return true;
}
}  // namespace EchoUtils
