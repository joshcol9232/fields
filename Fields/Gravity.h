#pragma once

#include <Eigen/Dense>
#include "../Body.h"

// Pre-declaration
//class Body;

namespace fields {

class Gravity {
 public:
  void apply_force(Body& a, Body& b) const;
};

}  // namespace fields

