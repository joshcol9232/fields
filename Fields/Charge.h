#pragma once

#include "../Body.h"
#include "Field.h"

namespace fields {

class Charge : public Field {
 public:
  void apply_force(Body& a, Body& b) const;
};

}  // namespace fields

