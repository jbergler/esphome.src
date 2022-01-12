#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace spanet {

enum class SpanetType {
  BOOLEAN,
  INT,
  FLOAT
};
enum class SpanetAttr {
  current_temperature,
  target_temperature,
  heater_active,
};

struct SpanetValue {
  SpanetAttr attr;
  SpanetType type;
  union {
    optional<bool> val_bool;
    optional<int> val_int;
    optional<float> val_float;
  };
};

struct SpanetListener {
  SpanetAttr attr;
  std::function<void(SpanetValue)> on_value;
};

class Spanet : public Component, public uart::UARTDevice {
 public:
  Spanet() = default;

  void setup() override;
  void dump_config() override;
  void loop() override;

  float get_setup_priority() const override;

  void request_data();
  void register_listener(SpanetAttr attr, const std::function<void(SpanetValue)> &func);

 protected:
  std::string rx_buffer_;
  uint32_t last_rx_byte_ts_ = 0;

  void handle_byte(uint8_t c);
  void handle_msg();
  void parse_value(std::string line, int pos, const char* value);

  void handle_value(SpanetValue value);
  void handle_as_int(SpanetAttr attr, const char* value);
  void handle_as_float(SpanetAttr attr, const char* value);
  void handle_as_temp(SpanetAttr attr, const char* value);
  void handle_as_boolean(SpanetAttr attr, const char* value);

  std::vector<SpanetListener> listeners_;
};

}  // namespace spanet
}  // namespace esphome
