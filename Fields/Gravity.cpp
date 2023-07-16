#include <iostream>

#include "../common.h"
#include "Gravity.h"

namespace fields {

void Gravity::apply_force(Body& a, Body& b) const {
  // If one of the bodies does not have a gravity component, exit with 0vec
  if (!(a.has_attribute<eGravityType>() && b.has_attribute<eGravityType>()))
    return;

  // Otherwise, has force
  Vector2f force = a.displacement_to(b);
  const float distance = force.norm();
  force *= G * a.get_mass() * b.get_mass() / (distance*distance*distance);
  
  a.apply_force(force);
  b.apply_force(-force);
}

}  // fields

