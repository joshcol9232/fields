#pragma once

#include "Field.h"
#include "GravityAttribute.h"

namespace fields {

class Gravity : public Field<GravityAttribute> {
 public:
  Gravity();
};

}  // namespace fields

