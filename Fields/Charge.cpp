#include <iostream>

#include "../common.h"
#include "Charge.h"
#include "ChargeAttribute.h"


namespace fields {

void Charge::apply_force(Body& a, Body& b) const {
  // If one of the bodies does not have a gravity component, exit with 0vec
  if (!(a.has_attribute<ChargeAttribute>() && b.has_attribute<ChargeAttribute>()))
    return;

  const float charge_a = a.get_attribute<ChargeAttribute>().get_charge();
  const float charge_b = b.get_attribute<ChargeAttribute>().get_charge();

  // COULOMBS LAW
  const bool attractive = std::signbit(charge_a) ^ std::signbit(charge_b); // XOR
  const float modifier = static_cast<float>(attractive) * 2.0 - 1.0;

  const Vector2f dist_vec = a.displacement_to(b);
  const float distance = dist_vec.norm();
  const Vector2f force = dist_vec * modifier * COULOMB / (distance * distance * distance);
  
  a.apply_force(force);
  b.apply_force(-force);
}

}  // fields

