#pragma once

#include "Field.h"
#include "ChargeAttribute.h"

namespace fields {

class Charge : public Field<ChargeAttribute> {
 public:
  Charge();
};

}  // namespace fields

