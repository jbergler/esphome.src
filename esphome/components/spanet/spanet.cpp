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

void Spanet::setup() {
  this->set_interval("request_data", 15000, [this] { this->request_data(); });
}

void Spanet::dump_config() {
  ESP_LOGCONFIG(TAG, "SpaNET:");
  this->check_uart_settings(38400);
}

void Spanet::loop() {
  while (this->available()) {
    uint8_t b;
    this->read_byte(&b);
    this->handle_byte(b);
  }
}

void Spanet::handle_byte(uint8_t b) {
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

void Spanet::register_listener(SpanetAttr attr, const std::function<void(SpanetValue)> &func) {
  auto listener = SpanetListener{
      .attr = attr,
      .on_value = func,
  };
  this->listeners_.push_back(listener);
}

void Spanet::handle_value(SpanetValue value) {
  for (auto &listener : this->listeners_)
    if (listener.attr == value.attr)
      listener.on_value(value);
}

void Spanet::handle_as_int(SpanetAttr attr, const char* value) {
  auto i = parse_number<int>(value);
  SpanetValue v = {
    .attr = attr,
    .type = SpanetType::INT,
    { .val_int = i }
  };
  this->handle_value(v);
}
void Spanet::handle_as_float(SpanetAttr attr, const char* value) {
  auto f = parse_number<float>(value);
  SpanetValue v = {
    .attr = attr,
    .type = SpanetType::FLOAT,
    { .val_float = f }
  };
  this->handle_value(v);
}
void Spanet::handle_as_temp(SpanetAttr attr, const char* value) {
  auto f = parse_number<float>(value);
  if (f.has_value())
    f = f.value() / 10.0f;

  SpanetValue v = {
    .attr = attr,
    .type = SpanetType::FLOAT,
    { .val_float = f }
  };
  this->handle_value(v);
}
void Spanet::handle_as_boolean(SpanetAttr attr, const char* value) {
  auto i = parse_number<int>(value);
  auto b = i.has_value() ? optional<bool>{(bool) i.value()} : nullopt;
  
  SpanetValue v = {
    .attr = attr,
    .type = SpanetType::BOOLEAN,
    { .val_bool = b }
  };
  this->handle_value(v);
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

void Spanet::handle_msg() {
  std::string msg = this->rx_buffer_;

  // Spa status lines all begin "\n,..."
  if (msg.substr(0, 2) == "\n,") {
    std::vector<std::string> parts = explode(msg.substr(2, std::string::npos), ",");
    if (parts.size() <= 1) {
      ESP_LOGW(TAG, "Unknown/invalid command: '%s'", msg.c_str());
    } else {
      std::string line = parts[0];
      for(int pos = 1; pos < parts.size(); pos++){
        this->parse_value(line, pos, parts[pos].c_str());
      }
    } 
  } else {
    ESP_LOGI(TAG, "CMD unknown %s", msg.substr(0,5).c_str());
  }
}

void Spanet::parse_value(std::string line, int pos, const char* value) {
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
        this->handle_as_boolean(SpanetAttr::heater_active, value);
        break;
      case 13: // Auto
        // boolean (0 or 1)
        break;
      case 14: // Lights
        // boolean (0 or 1)
        break;
      case 15: // Current water temperature
        this->handle_as_temp(SpanetAttr::current_temperature, value);
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
        this->handle_as_temp(SpanetAttr::target_temperature, value);
        break;
      case 10: // Power save
        // 0 -> off, 1 -> low, 2 -> high
        break;
    }
  } else if (line == "RG") {
    switch (pos) {
      case 12: // Lock mode
        // 0 -> off, 1 -> partial, 2 -> full
        this->handle_as_int(SpanetAttr::lock_mode, value);
        break;
    }
  }
}

float Spanet::get_setup_priority() const { return setup_priority::DATA; }

void Spanet::request_data() {
  if (!network::is_connected())
    return;

  this->write_str("RF\n");
  this->flush();
}

}  // namespace spanet
}  // namespace esphome
