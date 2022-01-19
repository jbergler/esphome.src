#pragma once

#include "spanet_select.h"

namespace esphome {
namespace spanet {

class LockMode : public SpanetSelect {
  public:
    LockMode() : SpanetSelect({"Off", "Partial", "Full"}, SpanetAttr::lock_mode, "S21") {}
};

}  // namespace spanet
}  // namespace esphome
