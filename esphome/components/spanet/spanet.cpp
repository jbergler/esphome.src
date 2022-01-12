#include "spanet.h"
#include "esphome/core/log.h"
#include "esphome/components/network/util.h"
#include <vector>
#include <string>

namespace esphome {
namespace spanet {

static const char *const TAG = "spanet";

static const int RECEIVE_TIMEOUT = 300;
static const uint8_t EOL = 0x0D; // '\r'

void SpaNET::setup() {
  this->set_interval("request_data", 15000, [this] { this->request_data(); });
}

void SpaNET::dump_config() {
  ESP_LOGCONFIG(TAG, "SpaNET:");
  this->check_uart_settings(38400);
}

void SpaNET::loop() {
  while (this->available()) {
    uint8_t b;
    this->read_byte(&b);
    this->handle_byte(b);
  }
}

void SpaNET::handle_byte(uint8_t b) {
  uint32_t now = millis();

  if (now - this->last_rx_byte_ts_ > RECEIVE_TIMEOUT) {
    ESP_LOGD(TAG, "read timeout, buffer was '%s'", this->rx_buffer_.c_str());
    this->rx_buffer_.clear();
  }

  if (b == EOL) {
    this->handle_msg();
    this->rx_buffer_.clear();
  } else {
    this->rx_buffer_ += b;
  }
  
  this->last_rx_byte_ts_ = millis();
}

std::vector<std::string> explode(std::string s, std::string d) {
  std::vector<std::string> result;
  std::string token;
  size_t pos = 0;
  while ((pos = s.find(d)) != std::string::npos) {
    token = s.substr(0, pos);
    result.push_back(token);
    s.erase(0, pos + d.length());
  }
  return result;
}

void SpaNET::handle_msg() {
  std::string msg = this->rx_buffer_;

  // Spa status lines all begin "\n,..."
  if (msg.substr(0, 2) == "\n,") {
    std::vector<std::string> parts = explode(msg.substr(2, std::string::npos), ",");
    if (parts.size() <= 1) {
      ESP_LOGW(TAG, "Unknown/invalid command: '%s'", msg.c_str());
    } else {
      std::string line = parts[0];
      for(int pos = 1; pos < parts.size(); pos++){
        this->handle_status_value(line, pos, parts[pos]);
      }
    } 
  } else {
    ESP_LOGI(TAG, "CMD unknown %s", msg.substr(0,5).c_str());
  }
}

void SpaNET::handle_status_value(std::string line, int pos, std::string value) {
  if (line == "R4") {
    switch (pos) {
      case 1: // Operation mode
        // NORM, ECON, AWAY, WEEK
        break;
    }
  } else if (line == "R5") {
    switch (pos) {
      case 10: // Sleeping
        // boolean (0 or 1)
        break;
      case 11: // Cleaning (UV/Ozone running)
        // boolean (0 or 1)
        break;
      case 12: // Heater running
        if (this->heating_sensor_) {
          int state = parse_number<int>(value.c_str()).value_or(0);
          this->heating_sensor_->publish_state(state);
        }
        break;
      case 13: // Auto
        // boolean (0 or 1)
        break;
      case 14: // Lights
        // boolean (0 or 1)
        break;
      case 15: // Current water temperature
        if (this->water_temperature_sensor_) {
          float temp = parse_number<float>(value.c_str()).value_or(0.0f) / 10;
          this->water_temperature_sensor_->publish_state(temp);
        }
        break;
      case 16: // Cleaning (Sanitise cycle running)
        // boolean (0 or 1)
        break;
    }
  } else if (line == "R6") {
    switch (pos) {
      case 2: // Light brightness
        // range 1->5
        break;
      case 3: // Light color
        // range 1->30
        break;
      case 4: // Light mode
        // 0 -> white, 1 -> color, 2 -> step, 3 -> fade, 4 -> party
        break;
      case 5: // Light effect speed
        // range 1->5
        break;
      case 6: // Filtration runtime
        // range 1->24
        break;
      case 7: // Filtration interval
        // 1, 2, 3, 4, 6, 8, 12, 24
        break;
      case 8: // Set water temperature
        if (this->set_temperature_sensor_) {
          float temp = parse_number<float>(value.c_str()).value_or(0.0f) / 10;
          this->set_temperature_sensor_->publish_state(temp);
        }
        break;
      case 10: // Power save
        // 0 -> off, 1 -> low, 2 -> high
        break;
    }
  } else if (line == "RG") {
    switch (pos) {
      case 12: // Lock mode
        // 0 -> off, 1 -> partial, 2 -> full
        break;
    }
  }
}

float SpaNET::get_setup_priority() const { return setup_priority::DATA; }

void SpaNET::request_data() {
  if (!network::is_connected())
    return;

  this->write_str("RF\n");
  this->flush();
}

}  // namespace spanet
}  // namespace esphome
