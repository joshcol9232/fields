#include <Eigen/Dense>

#include "../common.h"
#include "Charge.h"

namespace fields {

using Eigen::Vector2f;

Charge::Charge() :
  Field([](const Body& a, const Body& b,
           const ChargeAttribute& ch_a, const ChargeAttribute& ch_b) -> Vector2f
        {
          const Vector2f dist_vec = a.displacement_to(b);
          const float distance = dist_vec.norm();
          return dist_vec * ch_a.get_charge() * -ch_b.get_charge() * COULOMB / (distance * distance * distance);
        })
{}

}  // fields

