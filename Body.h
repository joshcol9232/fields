#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <type_traits>
#include <stdexcept>

#include <SFML/Graphics.hpp>
#include <Eigen/Dense>

#include "Fields/Attribute.h"

using Eigen::Vector2f;

class Body {
 public:
  Body() {}
  Body(Vector2f pos, Vector2f velocity, float radius);
  Body(Vector2f pos, Vector2f velocity,
       float radius, float mass);

  // -- Rendering --
  void draw(sf::RenderWindow& window, sf::CircleShape& circle_mesh) const;
  void render_acc(sf::RenderTarget &target) const;

  // -- Physics --
  void step(const float dt);
  void apply_force(const Vector2f& force);
  void reset_forces() { force_.x() = 0.0; force_.y() = 0.0; };

  //    Collisions
  void elastic_collide_with(Body& other, const float distance, const float dt);
  void correct_overlap_with(Body& other, const float distance);
  
  // -- Getters / tools --
  float get_radius() const { return radius_; }
  float get_mass() const { return mass_; }
  Vector2f displacement_to(const Body& other) const;

  template<typename Attr>
  bool has_attribute() const {
    for (const auto& attribute : attributes_) {
      if (attribute->get_type() == Attr::attr_type) return true;
    }
    return false;
  }

  // WARNING: Only do this after checking there is an attribute
  template<typename Attr>
  const Attr& get_attribute() const {
    auto it = std::find_if(attributes_.begin(),
                           attributes_.end(),
                           [](const auto& attr) -> bool {
                             return attr->get_type() == Attr::attr_type;
                           });
    if (it == attributes_.end()) {
      throw std::runtime_error("Attribute not found in body - make sure to check beforehand.");
    }

    return dynamic_cast<const Attr&>(*(*it));
  }

  friend class BodyBuilder;
 private:
  Vector2f x_;
  Vector2f v_;
  Vector2f force_;
  sf::Color color_;
  float mass_;
  float radius_;

  std::vector<std::shared_ptr<fields::Attribute>> attributes_;
};
