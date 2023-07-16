#pragma once

#include "AttributeType.h"

namespace fields {

class Attribute {
 public:
  static const eAttributeType attr_type = eNoAttribute;
  virtual eAttributeType get_type() const { return attr_type; }
};

#define DEFINE_ATTRIBUTE_TYPE(FIELD_NAME) \
  static const eAttributeType attr_type = e##FIELD_NAME##Type; \
  eAttributeType get_type() const override { return attr_type; } \

}  // fields

