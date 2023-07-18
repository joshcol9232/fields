#include <iostream>

#include "../common.h"
#include "Charge.h"
#include "ChargeAttribute.h"


namespace fields {

void Charge::apply_force(Body& a, Body& b) const {
  Field::apply_force<ChargeAttribute>(a, b,
    [](const Body& a, const Body& b,
       const ChargeAttribute& ch_a, const ChargeAttribute& ch_b) -> Vector2f
    {
      const float charge_a = ch_a.get_charge();
      const float charge_b = ch_b.get_charge();
      // COULOMBS LAW
      const bool attractive = std::signbit(charge_a) ^ std::signbit(charge_b); // XOR
      const float modifier = static_cast<float>(attractive) * 2.0 - 1.0;

      const Vector2f dist_vec = a.displacement_to(b);
      const float distance = dist_vec.norm();
      return dist_vec * modifier * COULOMB / (distance * distance * distance);
    }); 
}

}  // fields

