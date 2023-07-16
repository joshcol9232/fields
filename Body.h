#pragma once

#include <SFML/Graphics.hpp>
#include <Eigen/Dense>

using Eigen::Vector2f;

class Body {
 public:
  Body() {}
  Body(Vector2f pos, Vector2f velocity, float radius);
  Body(Vector2f pos, Vector2f velocity,
       float radius, float mass);

  // -- Rendering --
  void draw(sf::RenderWindow& window, sf::CircleShape& circle_mesh) const;
  void render_force(sf::RenderTarget &target) const;

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

 private:
  Vector2f x_;
  Vector2f v_;
  Vector2f force_;
  float mass_;
  float radius_;
};
