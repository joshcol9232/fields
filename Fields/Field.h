#pragma once

#include <Eigen/Dense>
#include "../Body.h"

namespace fields {

class Field {
 public:
  template<typename Attr>
  void apply_force(Body& a, Body& b,
      const std::function<Vector2f(const Body&, const Body&, const Attr&, const Attr&)>& F) const;
};


template<typename Attr>
void Field::apply_force(Body& a, Body& b, const std::function<Vector2f(const Body&, const Body&, const Attr&, const Attr&)>& F) const {
    // If one of the bodies does not have a gravity component, exit with 0vec
    if (!(a.has_attribute<Attr>() && b.has_attribute<Attr>()))
      return;

    const auto attribute_a = a.get_attribute<Attr>();
    const auto attribute_b = b.get_attribute<Attr>();
    const Vector2f force = F(a, b, attribute_a, attribute_b);

    /*
    // COULOMBS LAW
    const bool attractive = std::signbit(charge_a) ^ std::signbit(charge_b); // XOR
    const float modifier = static_cast<float>(attractive) * 2.0 - 1.0;

    const Vector2f dist_vec = a.displacement_to(b);
    const float distance = dist_vec.norm();
    const Vector2f force = dist_vec * modifier * COULOMB / (distance * distance * distance);
    */

    a.apply_force(force);
    b.apply_force(-force);
  }

}  // namespace fields
