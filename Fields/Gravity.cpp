#include <Eigen/Dense>

#include "../common.h"
#include "Gravity.h"

namespace fields {

using Eigen::Vector2f;

Gravity::Gravity() :
  Field([](const Body& a, const Body& b,
           const GravityAttribute&, const GravityAttribute&) -> Vector2f
        {
          Vector2f force = a.displacement_to(b);
          const float distance = force.norm();
          force *= G * a.get_mass() * b.get_mass() / (distance*distance*distance);
          return force; 
        })
{}

}  // fields

