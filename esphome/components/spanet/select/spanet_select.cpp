#include "esphome/core/log.h"
#include "spanet_select.h"

namespace esphome {
namespace spanet {

void SpanetSelect::setup() {
  // Setup options
  this->traits.set_options(options_);

  // Listen for updates
  this->parent_->register_listener(source_, [this](const SpanetValue &value) {
    if(value.type != SpanetType::INT)
      return;

    if(!value.val_int.has_value())
      return;

    this->state = options_.at(value.val_int.value());
    this->publish_state();
  });
}

void SpanetSelect::control(const std::string &value) {
  auto it = find(options_.begin(), options_.end(), value);
    
  if (it != options_.end()) {
    int index = it - options_.begin();
    auto cmd = command_ + ":" + std::to_string(index);
    this->parent_->write_str(cmd.c_str());
  }
  
  this->state = value;
  this->publish_state();
}

}  // namespace spanet
}  // namespace esphome