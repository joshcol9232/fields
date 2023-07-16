#pragma once

#include <memory>

#include <Eigen/Dense>
#include <SFML/Graphics.hpp>
#include "Body.h"
#include "Fields/Attribute.h"

using Eigen::Vector2f;

class BodyBuilder {
 public:
  // Start with required parameters
  BodyBuilder(const Vector2f& pos, const Vector2f& vel,
              const float radius);
  // -- Optional attributes --
  // Add an arbitrary attribute
  template<typename Attr, typename ...AttrArgs>
  BodyBuilder& with_attribute(AttrArgs... args) {
    auto attr = std::make_shared<Attr>(args...);
    result_.attributes_.push_back(attr);
    return *this;
  }

  BodyBuilder& set_color(const sf::Color color);
  BodyBuilder& set_mass(const float mass);  // Set mass manually
  BodyBuilder& with_gravity();
  BodyBuilder& with_charge(const float charge);
  BodyBuilder& with_charge(const bool sign);

  // Finalise
  Body build() const { return result_; }

 private:
  void with_attribute(std::shared_ptr<fields::Attribute> attr);

  Body result_;
};

