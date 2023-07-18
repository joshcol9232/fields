#pragma once

#include <Eigen/Dense>
#include "../Body.h"

namespace fields {

template<typename Attr>
class Field {
 public:
  Field(std::function<Vector2f(const Body&, const Body&, const Attr&, const Attr&)> force_func) :
    force_func_(force_func)
  {}

  void apply_force(Body& a, Body& b) const {
    // If one of the bodies does not have a field component, exit
    if (!(a.has_attribute<Attr>() && b.has_attribute<Attr>()))
      return;

    const auto attribute_a = a.get_attribute<Attr>();
    const auto attribute_b = b.get_attribute<Attr>();
    const Vector2f force = force_func_(a, b, attribute_a, attribute_b);
    // Apply force between bodies
    a.apply_force(force);
    b.apply_force(-force);
  }

 private:
  std::function<Vector2f(const Body&, const Body&, const Attr&, const Attr&)> force_func_;
};

}  // namespace fields

