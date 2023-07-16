#pragma once

#include "Attribute.h"

namespace fields {

class ChargeAttribute : public Attribute {
 public:
  DEFINE_ATTRIBUTE_TYPE(Charge)

  ChargeAttribute(const float charge) : charge_(charge) {}
  ChargeAttribute(const ChargeAttribute& other) : charge_(other.charge_) {}

  float get_charge() const { return charge_; }
  
 private:
  float charge_;
};

}  // namespace fields
