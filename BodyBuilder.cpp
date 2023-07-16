#include <memory>
#include <iostream>

#include "BodyBuilder.h"
#include "Fields/GravityAttribute.h"

BodyBuilder::BodyBuilder(const Vector2f& pos, const Vector2f& vel,
                         const float radius) :
  result_(pos, vel, radius)
{}

BodyBuilder& BodyBuilder::set_mass(const float mass) {
  result_.mass_ = mass;
  return *this;
}

BodyBuilder& BodyBuilder::with_gravity() {
  std::cout << "making with gravity..." << std::endl;
  with_attribute<fields::GravityAttribute>();
  return *this;
}


