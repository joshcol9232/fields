#pragma once

#include "AttributeType.h"

namespace fields {

struct Attribute {
  virtual eAttributeType get_type() const = 0;
};

}  // fields

