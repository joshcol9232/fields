#include <memory>
#include <iostream>

#include "BodyBuilder.h"
#include "Fields/GravityAttribute.h"
#include "Fields/ChargeAttribute.h"

BodyBuilder::BodyBuilder(const Vector2f& pos, const Vector2f& vel,
                         const float radius) :
  result_(pos, vel, radius)
{}

BodyBuilder& BodyBuilder::set_mass(const float mass) {
  result_.mass_ = mass;
  return *this;
}

BodyBuilder& BodyBuilder::set_color(const sf::Color color) {
  result_.color_ = color;
  return *this;
}

BodyBuilder& BodyBuilder::with_gravity() {
  std::cout << "making with gravity..." << std::endl;
  with_attribute<fields::GravityAttribute>();
  return *this;
}

BodyBuilder& BodyBuilder::with_charge(const float charge) {
  std::cout << "making with charge..." << std::endl;
  set_color(std::signbit(charge) ? sf::Color::Yellow : sf::Color::Red);
  with_attribute<fields::ChargeAttribute>(charge);
  return *this;
}

BodyBuilder& BodyBuilder::with_charge(const bool sign) {
  std::cout << "making with charge..." << std::endl;
  return with_charge((static_cast<float>(sign) - 0.5f) * 2.0f);
}

