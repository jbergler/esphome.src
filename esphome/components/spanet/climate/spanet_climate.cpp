#include "esphome/core/log.h"
#include "spanet_climate.h"

namespace esphome {
namespace spanet {

static const char *const TAG = "spanet.climate";

void SpanetClimate::setup() {
  this->mode = climate::CLIMATE_MODE_HEAT;

  this->parent_->register_listener(SpanetAttr::current_temperature, [this](const SpanetValue &value) {
    if(!value.val_float.has_value())
      return;
    this->current_temperature = value.val_float.value();
    this->publish_state();
  });
  this->parent_->register_listener(SpanetAttr::target_temperature, [this](const SpanetValue &value) {
    if(!value.val_float.has_value())
      return;
    this->target_temperature = value.val_float.value();
    this->publish_state();
  });
  this->parent_->register_listener(SpanetAttr::heater_active, [this](const SpanetValue &value) {
    auto heating = value.val_bool;
    if (heating.has_value()) {
      this->action = heating.value() ? climate::CLIMATE_ACTION_HEATING
                                     : climate::CLIMATE_ACTION_IDLE;
    } else {
      this->action = climate::CLIMATE_ACTION_OFF;
    }
    this->publish_state();
  });
}

void SpanetClimate::loop() {}

void SpanetClimate::control(const climate::ClimateCall &call) {
  if (call.get_target_temperature().has_value()) {
    const float target_temperature = *call.get_target_temperature();
    ESP_LOGV(TAG, "Setting target temperature: %.1f", target_temperature);
    char buffer[16];
    sprintf(buffer, "W40:%i\n", (int)(target_temperature * 10));
    this->parent_->write_str(buffer);
  }
}

climate::ClimateTraits SpanetClimate::traits() {
  auto traits = climate::ClimateTraits();
  traits.set_supports_action(true);
  traits.set_supports_current_temperature(true);
  traits.add_supported_mode(climate::CLIMATE_MODE_HEAT);
  traits.set_visual_min_temperature(10.0);
  traits.set_visual_max_temperature(41.0);
  // traits.add_supported_preset(climate::CLIMATE_PRESET_ECO)

  return traits;
}

void SpanetClimate::dump_config() {
  LOG_CLIMATE("", "SpaNET Climate", this);
}

}  // namespace spanet
}  // namespace esphome
