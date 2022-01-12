#pragma once

#include "esphome/core/component.h"
#include "esphome/components/spanet/spanet.h"
#include "esphome/components/climate/climate.h"

namespace esphome {
namespace spanet {

class SpanetClimate : public climate::Climate, public Component {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;

  void set_spanet_parent(Spanet *parent) { this->parent_ = parent; }

 protected:
  /// Override control to change settings of the climate device.
  void control(const climate::ClimateCall &call) override;

  /// Return the traits of this controller.
  climate::ClimateTraits traits() override;

  Spanet *parent_;
};

}  // namespace spanet
}  // namespace esphome
