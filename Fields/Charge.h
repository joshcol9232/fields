#pragma once

#include "../Body.h"

namespace fields {

class Charge {
 public:
  void apply_force(Body& a, Body& b) const;
};

}  // namespace fields

