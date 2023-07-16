#pragma once

#include "Attribute.h"

namespace fields {

// Type showing that this body has gravity. Body already has mass
struct GravityAttribute : public Attribute {
  eAttributeType get_type() const override { return eGravityType; }
};

}  // namespace fields

