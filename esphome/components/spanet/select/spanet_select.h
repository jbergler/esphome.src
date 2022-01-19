#pragma once

#include "esphome/core/component.h"
#include "esphome/components/spanet/spanet.h"
#include "esphome/components/select/select.h"

namespace esphome {
namespace spanet {

class SpanetSelect : public select::Select {
  public:
    SpanetSelect(
        std::vector<std::string> options,
        SpanetAttr source,
        std::string command
    ) : options_(options),
        source_(source),
        command_(command) {}

    void setup();
    void control(const std::string &value);
    void set_spanet_parent(Spanet *parent) { this->parent_ = parent; }

  private:
    const std::vector<std::string> options_;
    const SpanetAttr source_;
    const std::string command_;

    Spanet *parent_;
};

}  // namespace spanet
}  // namespace esphome
