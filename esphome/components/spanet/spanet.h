#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace spanet {

class SpaNET : public Component, public uart::UARTDevice {
 public:
  SpaNET() = default;

  // SENSORS
  void set_water_temperature_sensor(sensor::Sensor *sensor) { water_temperature_sensor_ = sensor; }
  void set_set_temperature_sensor(sensor::Sensor *sensor) { set_temperature_sensor_ = sensor; }

  // BINARY SENSORS
  void set_heating_binary_sensor(binary_sensor::BinarySensor *sensor) { heating_sensor_ = sensor; }
  
  void setup() override;
  void dump_config() override;
  void loop() override;

  float get_setup_priority() const override;

 protected:
  std::string rx_buffer_;
  uint32_t last_rx_byte_ts_ = 0;

  void request_data();
  void handle_byte(uint8_t c);
  void handle_msg();
  void handle_status_value(std::string line, int pos, std::string value);

  sensor::Sensor *water_temperature_sensor_{nullptr};
  sensor::Sensor *set_temperature_sensor_{nullptr};

  binary_sensor::BinarySensor *heating_sensor_{nullptr};
};

}  // namespace spanet
}  // namespace esphome
