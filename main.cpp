#include <algorithm>
#include <iostream>
#include <vector>
#include <random>

#include <SFML/Graphics.hpp>
#include <Eigen/Dense>

#include "common.h"
#include "tools.h"
#include "Body.h"
#include "BodyBuilder.h"

#include "Fields/Field.h"
#include "Fields/Gravity.h"
#include "Fields/Charge.h"

using Eigen::Vector2f;
constexpr float SPAWN_RADIUS = 7.0;

// Utils
namespace {

template<typename ExtraBuildStepFunctor>
void spawn_square_of_bodies(
  std::vector<Body>& bodies,
  Vector2f top_left,
  Vector2f v,
  const size_t w,
  const size_t h,
  const float rad,
  ExtraBuildStepFunctor Bfunc
) {
  for (size_t i = 0; i < w; ++i) {
    for (size_t j = 0; j < h; ++j) {
      BodyBuilder builder = BodyBuilder(Vector2f(top_left.x() + static_cast<float>(i) * rad * 2.0,
                                                 top_left.y() + static_cast<float>(j) * rad * 2.0),
                                        v,
                                        rad + 1.0);

      Bfunc(i, j, builder);   // Apply custom step
      bodies.push_back(builder.build());
    }
  }
}


void spawn_planet_with_moons(
  std::vector<Body>& bodies,
  const Vector2f position,
  const Vector2f frame_velocity,
  const float main_planet_radius,
  const size_t moon_num,
  const float moon_orbit_radius_range[2],    // Starting from surface of planet
  const float moon_body_radius_range[2],
  const bool orbit_direction_clockwise  // anticlockwise = false, clockwise = true
) {
  BodyBuilder builder(position, frame_velocity, main_planet_radius);
  builder.with_gravity();
  bodies.emplace_back(builder.build());

  const float main_planet_mass = bodies[bodies.size()-1].get_mass();

  // let mut rng = rand::thread_rng();

  //   let orbit_rad_range = Uniform::from(moon_orbit_radius_range.0..moon_orbit_radius_range.1);
  //   let angle_range = Uniform::from(0.0..TWO_PI);
  //   let size_rad_range = Uniform::from(moon_body_radius_range.0..moon_body_radius_range.1);

  std::random_device rd;
  std::mt19937 e2(rd());
  std::uniform_real_distribution<> dist(0.0, 1.0);

  for (size_t n = 0; n < moon_num; ++n) {
    const float orbit_radius = main_planet_radius + moon_orbit_radius_range[0] + dist(e2) * (moon_orbit_radius_range[1] - moon_orbit_radius_range[0]);
    const float orbit_speed = tools::circular_orbit_speed(main_planet_mass, orbit_radius);
    const float start_angle = dist(e2) * 2.0 * M_PI;      // Angle from main planet to moon
    const Vector2f start_pos = tools::get_components(orbit_radius, start_angle);   // Position on circle orbit where planet will start

    const Vector2f start_velocity = tools::get_components(
      orbit_speed,
      orbit_direction_clockwise ? start_angle + M_PI/2.0 : start_angle - M_PI/2.0
    );

    const float moon_radius = moon_body_radius_range[0] + dist(e2) * (moon_body_radius_range[1] - moon_body_radius_range[0]);

    BodyBuilder builder(position + start_pos,
                        start_velocity + frame_velocity,
                        moon_radius);
    builder.with_gravity();
    bodies.emplace_back(builder.build());
  }
}


// Reset bodies to start state
void start_state(std::vector<Body>& bodies) {
  bodies.clear();

  /*
  spawn_square_of_bodies(bodies, Vector2f(100.0, 100.0), Vector2f::Zero(), 15, 15, SPAWN_RADIUS,
                         [](size_t i, size_t j, BodyBuilder& builder) {
                           builder
                                  //.with_charge(static_cast<bool>((i + j) & 1));
                                  .with_gravity();
                         });
  */
  /*
  spawn_square_of_bodies(bodies, Vector2f(150.0, 150.0), Vector2f::Zero(), 25, 25, 10.0,
                         [](size_t i, size_t j, BodyBuilder& builder) {
                           builder
                                  //.with_charge(static_cast<bool>((i + j) & 1));
                                  .with_gravity();
                         });
  */

  // --- planets ---
  constexpr float orbit_range[2] = {150.0, 300.0};
  constexpr float   rad_range[2] = {0.5, 3.0};
  constexpr size_t           num = 500;
  spawn_planet_with_moons(bodies, Vector2f(SCREEN_WIDTH/2, SCREEN_HEIGHT/2),
                          Vector2f::Zero(), 50.0, num, orbit_range,
                          rad_range, true);
}

void move_camera(auto& window, auto& main_camera, const float dx, const float dy, const float dt) {
  main_camera.move(dx * dt, dy * dt);
  window.setView(main_camera);
}

void eliminate_crossover(std::vector<Body>& bodies, const bool reverseOrder) {
  if (bodies.size() < 1) [[unlikely]] return;

  Vector2f dist_vec;
  float dist;

  const auto func = [&](Body& a, Body& b) {
    dist_vec = a.displacement_to(b);
    dist = dist_vec.norm();
    if (dist < a.get_radius() + b.get_radius()) {
      a.correct_overlap_with(b, dist);
    }
  };

  if (reverseOrder) {
    for (int i = bodies.size()-2; i > 0; --i) {
      for (int j = bodies.size()-1; j > i; --j) {
        func(bodies[i], bodies[j]);
      }
    }
  } else {
    for (size_t i = 0; i < bodies.size()-1; ++i) {
      for (size_t j = i+1; j < bodies.size(); ++j) {
        func(bodies[i], bodies[j]);
      }
    }
  }
}

void process_elastic_coll(std::vector<Body>& bodies, const float dt) {
  if (bodies.size() < 1) [[unlikely]] return;

  Vector2f dist_vec;
  float dist;
  Body *a = &bodies[0];
  Body *b = &bodies[1];

  for (size_t i = 0; i < bodies.size()-1; ++i) {
    a = &bodies[i];
    for (size_t j = i+1; j < bodies.size(); ++j) {
      b = &bodies[j];

      dist_vec = a->displacement_to(*b);
      dist = dist_vec.norm();
      // Process collisions
      if (dist < a->get_radius() + b->get_radius()) {
        a->elastic_collide_with(*b, dist, dt);
      }
    }
  }
}

}  // namespace

int main() {
  srand((unsigned int) time(0));

  std::vector<Body> bodies;
  start_state(bodies);
  std::cout << "start state made." << std::endl;

  // Create assets
  sf::CircleShape body_shape(1.0f, 200);
  body_shape.setFillColor(sf::Color::White);

  sf::Font font;
  font.loadFromFile("../UbuntuMono-B.ttf");
  sf::Text fps_text;
  fps_text.setFont(font);
  fps_text.setString("0");
  fps_text.setPosition(SCREEN_WIDTH - 60.0, 10.0);
  fps_text.setFillColor(sf::Color::Green);
  fps_text.setCharacterSize(12);

  // Mouse
  bool dragging = false;
  auto mouse_button_held = sf::Mouse::Left;
  sf::Vector2i mouse_start_pos, curr_mouse_press_pos;
  sf::Vertex drag_line[2];

  // Camera
  bool cam_move_up, cam_move_down, cam_move_left, cam_move_right;

  std::cout << "BODY NUM: " << bodies.size() << std::endl;

  // Make fields!
  fields::Gravity gravity_field;
  fields::Charge electric_field;

  // create the window
  sf::RenderWindow window(sf::VideoMode(static_cast<int>(SCREEN_WIDTH),
                                        static_cast<int>(SCREEN_HEIGHT)),
                          "Fields");
  sf::View main_camera(sf::FloatRect(0.f, 0.f, SCREEN_WIDTH, SCREEN_HEIGHT));

  sf::Clock delta_clock;
  float dt = 1.0/60.0;

  // Render acceleration?
  bool renderAcc = false;

  std::random_device rd;
  std::mt19937 e2(rd());

  std::cout << "Starting loop!" << std::endl;

  //#pragma omp parallel
  //#pragma omp master
  {
  sf::Event event;
  while (window.isOpen()) {
    if (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }

    // --- Mouse ---
    const bool left = sf::Mouse::isButtonPressed(sf::Mouse::Left);
    const bool right = sf::Mouse::isButtonPressed(sf::Mouse::Right);
    if (!dragging && (left || right)) {
      dragging = true;
      mouse_start_pos = sf::Mouse::getPosition(window);
      mouse_button_held = left ? sf::Mouse::Left : sf::Mouse::Right;
    } else if (dragging && !(left || right)) {   // If not pressed, and previously was then spawn a planet.
      dragging = false;

      // Spawn planet with velocity
      const auto drag = mouse_start_pos - curr_mouse_press_pos;
      Body b = BodyBuilder(Vector2f(mouse_start_pos.x, mouse_start_pos.y),
                           Vector2f(drag.x, drag.y) * 5.0,
                           SPAWN_RADIUS)
                 .set_mass(tools::volume_of_sphere(SPAWN_RADIUS) * PLANET_DENSITY * 5.0)
                 .with_charge(mouse_button_held == sf::Mouse::Left)
                 .with_gravity()
                 .build();

      bodies.emplace_back(b);
    }
    // -------------
    // --- Keyboard ---
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
      start_state(bodies);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
      bodies.clear();
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) {
      renderAcc = !renderAcc;
    }

    // Mouse drag
    if (dragging) {
      curr_mouse_press_pos = sf::Mouse::getPosition(window);
      drag_line[0] = sf::Vertex(sf::Vector2f(static_cast<float>(mouse_start_pos.x), static_cast<float>(mouse_start_pos.y)));
      drag_line[1] = sf::Vertex(sf::Vector2f(static_cast<float>(curr_mouse_press_pos.x), static_cast<float>(curr_mouse_press_pos.y)));
      drag_line[0].color = sf::Color::Green;
      drag_line[1].color = sf::Color::Green;
    }

    // Camera
    // if (cam_move_up)    move_camera(window, main_camera,    0.0, -600.0, dt);
    // if (cam_move_down)  move_camera(window, main_camera,    0.0,  600.0, dt);
    // if (cam_move_left)  move_camera(window, main_camera, -600.0,    0.0, dt);
    // if (cam_move_right) move_camera(window, main_camera,  600.0,    0.0, dt);

    // -- Update physics --

    // Update fields

    for (size_t i = 0; i < bodies.size()-1; ++i) {
      for (size_t j = i+1; j < bodies.size(); ++j) {
        gravity_field.apply_force(bodies[i], bodies[j]);
        electric_field.apply_force(bodies[i], bodies[j]);
      }
    }

    // Euler step
    for (auto& body : bodies) {
      body.step(dt);
    }

    // Overlap passes
    for (size_t o = 0; o < 2; ++o) {
      eliminate_crossover(bodies, static_cast<bool>(o % 2));
    }
    // Process collisions
    process_elastic_coll(bodies, dt);

    // Draw
    window.clear(sf::Color::Black);

    for (auto& body : bodies) {
      body.draw(window, body_shape);
      if (renderAcc) {
        body.render_acc(window);
      }
      body.reset_forces();
    }

    // Draw mouse drag
    if (dragging) window.draw(drag_line, 2, sf::Lines);

    // --- RENDER STATIC ITEMS LIKE FPS ---
    window.setView(window.getDefaultView());
    // Draw FPS counter
    fps_text.setString(std::to_string(1.0/dt));
    window.draw(fps_text);
    // ------------------------------------
    window.setView(main_camera);

    // end the current frame
    window.display();

    sf::Time dt_time = delta_clock.restart();
    dt = dt_time.asSeconds();
  }
  }  // pragma omp master

  return 0;
}
