#pragma once

#include "Attribute.h"

namespace fields {

// Type showing that this body has gravity. Body already has mass
class GravityAttribute : public Attribute {
  DEFINE_ATTRIBUTE_TYPE(Gravity)
};

}  // namespace fields

