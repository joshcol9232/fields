#include "Body.h"

#include <cmath>
#include <Eigen/Dense>

#include "common.h"
#include "tools.h"
#include "SfLine.h"

#include <iostream>


using Eigen::Vector2f;

Body::Body(Vector2f pos, Vector2f velocity, float radius) :
  Body(pos, velocity, radius, tools::volume_of_sphere(radius) * PLANET_DENSITY)
{}

Body::Body(Vector2f pos, Vector2f velocity,
           float radius, float mass) :
  x_(pos), v_(velocity), mass_(mass), radius_(radius), force_(0.0, 0.0),
  color_(sf::Color::White)
{}

void Body::step(const float dt) {
  // F = ma
  v_ += force_ * dt/mass_;
  x_ += v_ * dt;

  // Wall bouncing
#ifdef WALL_BOUNCE
  if (x_.x() < radius_) {
    v_.x() *= -1;
    x_.x() = radius_;
  }
  if (x_.x() > SCREEN_WIDTH - radius_) {
    v_.x() *= -1;
    x_.x() = SCREEN_WIDTH - radius_;
  }

  if (x_.y() < radius_) {
    v_.y() *= -1;
    x_.y() = radius_;
  }
  if (x_.y() > SCREEN_HEIGHT - radius_) {
    v_.y() *= -1;
    x_.y() = SCREEN_HEIGHT - radius_;
  }
#endif
}

void Body::draw(sf::RenderWindow& window, sf::CircleShape& circle_mesh) const {
  circle_mesh.setScale(1.0, 1.0);
  circle_mesh.setOrigin(1.0, 1.0);

  circle_mesh.setScale(radius_, radius_);
  circle_mesh.setPosition(x_.x(), x_.y());
  circle_mesh.setFillColor(color_);
  window.draw(circle_mesh);
}

void Body::apply_force(const Vector2f& df) {
  // #pragma omp atomic
  force_ += df;
}

Vector2f Body::displacement_to(const Body& other) const {
  return other.x_ - x_;
}

namespace {

// 2D "cross product" ie. determenant of packed matrix
inline float cross2d(const Vector2f& a, const Vector2f& b) {
  return a.x() * b.y() - b.x() * a.y();
}

}

void Body::elastic_collide_with(Body& other, const float distance, const float dt) {
  // --- Resolve collision ---
  const Vector2f dist_vec = other.x_ - x_;
  const Vector2f v_diff = other.v_ - v_;
  const float total_mass = mass_ + other.mass_;

  float vel_mul_dist_along_collision_normal = v_diff.dot(dist_vec);
  const float dist_sqr = distance * distance;

  // save change in velocity along collision normal for friction
  const float dv_0_along_normal = (2 * other.mass_ / total_mass) *
                    (vel_mul_dist_along_collision_normal / dist_sqr);

  const Vector2f dv_0 = dv_0_along_normal * dist_vec;

  const float dv_1_along_normal = - (2 * mass_ / total_mass) *
                    (vel_mul_dist_along_collision_normal / dist_sqr);
  const Vector2f dv_1 = dv_1_along_normal * dist_vec;

#ifdef DEBUG
  std::cout << "dv_0: (" << dv_0.x() << ", " << dv_0.y() << ")" << std::endl;
  std::cout << "dv_1: (" << dv_1.x() << ", " << dv_1.y() << ")" << std::endl;
#endif

  v_ += dv_0 * COLLISION_DAMPING;
  other.v_ += dv_1 * COLLISION_DAMPING;
}

void Body::correct_overlap_with(Body& other, const float distance) {
  // Move the bodies apart so they are not overlapping (this would cause issues)
  // NOTE: TODO - Maybe this is causing the spinning - not conserving angular momentum.
  //       Should instead shift the planet's along their trajectory?
  //        

  const Vector2f norm = (other.x_ - x_) / distance;  // Normal to collision
  // Here, calculate the overlap (dx) between the bodies. Both need to move apart by this amount.
  // Should conserve centre of mass though. Hence needs weighting, not just moving by 0.5 * dx.
  const Vector2f overlap_vec = norm * (distance - radius_ - other.radius_);

  // const Vector2f half_dx = 0.5 * overlap * norm;
  const float alpha = other.mass_ / (other.mass_ + mass_);
  x_ += alpha * overlap_vec;
  other.x_ -= (1.0 - alpha) * overlap_vec;
}

void Body::render_acc(sf::RenderTarget &target) const {
  sf::Vector2f start(x_.x(), x_.y());
  sf::Vector2f force_sfvec(force_.x(), force_.y());
  force_sfvec *= FORCE_DEBUG_MUL / mass_;

  const SfLine line(start, start + force_sfvec);
  target.draw(line);
}
