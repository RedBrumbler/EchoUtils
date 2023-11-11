#include "echo-utils.hpp"
#include "log.h"

#include "yodel/shared/loader.hpp"

extern "C" void setup(CModInfo* info) {
  info->id = MOD_ID;
  info->version = VERSION;
  info->version_long = GIT_COMMIT;
}

extern "C" void load() {
  LOG_INFO("Echo utils library from branch " GIT_BRANCH " (0x{:x}) by " GIT_USER, GIT_COMMIT);
  install_cbs_hook();
}
