#include <iostream>
#include <vector>
#include <random>

#include <SFML/Graphics.hpp>
#include <Eigen/Dense>

#include "common.h"
#include "tools.h"
#include "Body.h"

using Eigen::Vector2f;

// Utils
namespace {

void spawn_square_of_bodies(
  std::vector<Body>& bodies,
  Vector2f top_left,
  Vector2f v,
  const size_t w,
  const size_t h,
  const float rad
) {
  for (size_t i = 0; i < w; ++i) {
    for (size_t j = 0; j < h; ++j) {
      bodies.emplace_back(Vector2f(top_left.x() + static_cast<float>(i) * rad * 2.0 + 0.01,
                                   top_left.y() + static_cast<float>(j) * rad * 2.0 + 0.01),
                          v,
                          rad + 1.0);
    }
  }
}

// Reset bodies to start state
void start_state(std::vector<Body>& bodies) {
  bodies.clear();

  spawn_square_of_bodies(bodies, Vector2f(100.0, 100.0), Vector2f::Zero(), 10, 10, 20.0);
}

void move_camera(auto& window, auto& main_camera, const float dx, const float dy, const float dt) {
  main_camera.move(dx * dt, dy * dt);
  window.setView(main_camera);
}

void eliminate_crossover(std::vector<Body>& bodies) {
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
      if (dist < a->get_radius() + b->get_radius()) {
        a->correct_overlap_with(*b, dist);
      }
    }
  }
}

void process_elastic_coll(std::vector<Body>& bodies, const float dt) {
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
        std::cout << i << ", " << j << std::endl;
        std::cout << "Dist: " << dist << std::endl;
        std::cout << "COLLIDING! " << std::endl;
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
  sf::Vector2i mouse_start_pos, curr_mouse_press_pos;
  sf::Vertex drag_line[2];

  // Camera
  bool cam_move_up, cam_move_down, cam_move_left, cam_move_right;

  std::cout << "BODY NUM: " << bodies.size() << std::endl;

  // create the window
  sf::RenderWindow window(sf::VideoMode(static_cast<int>(SCREEN_WIDTH),
                                        static_cast<int>(SCREEN_HEIGHT)),
                          "Fields");
  sf::View main_camera(sf::FloatRect(0.f, 0.f, SCREEN_WIDTH, SCREEN_HEIGHT));

  sf::Clock delta_clock;
  float dt = 1.0/60.0;

  bool renderForce = false;

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }

      // --- Mouse ---
      if (event.type == sf::Event::MouseButtonPressed) {
        dragging = true;
        mouse_start_pos = sf::Mouse::getPosition(window);
      }
      if (event.type == sf::Event::MouseButtonReleased) {
        dragging = false;

        // Spawn planet with velocity
        constexpr float mouse_rad = 10.0;
        const auto drag = mouse_start_pos - curr_mouse_press_pos;
        bodies.emplace_back(Vector2f(mouse_start_pos.x, mouse_start_pos.y),
                            Vector2f(drag.x, drag.y) * 5.0,
                            mouse_rad, tools::volume_of_sphere(mouse_rad) * PLANET_DENSITY * 5.0);
      }
      // -------------
      // --- Keyboard ---
      if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::R) {
          start_state(bodies);
        } else if (event.key.code == sf::Keyboard::C) {
          bodies.clear();
        } else if (event.key.code == sf::Keyboard::F) {
          renderForce = !renderForce;
        }
        // --- CAMERA ---
        cam_move_up    = event.key.code == sf::Keyboard::W;
        cam_move_left  = event.key.code == sf::Keyboard::A;
        cam_move_down  = event.key.code == sf::Keyboard::S;
        cam_move_right = event.key.code == sf::Keyboard::D;
        // --------------
      } else if (event.type == sf::Event::KeyReleased) {
        // --- CAMERA ---
        if (event.key.code == sf::Keyboard::W) {
          cam_move_up = false;
        } else if (event.key.code == sf::Keyboard::A) {
          cam_move_left = false;
        } else if (event.key.code == sf::Keyboard::S) {
          cam_move_down = false;
        } else if (event.key.code == sf::Keyboard::D) {
          cam_move_right = false;
        }
        // --------------
      }

      // ----------------
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
    /*
    for (size_t i = 0; i < bodies.size()-1; ++i) {
      Body& a = bodies[i];
      for (size_t j = i+1; j < bodies.size(); ++j) {
        Body& b = bodies[j];
        
        // TODO: Apply field rules.
      }
    }
    */

    // Euler step
    for (auto& body : bodies) {
      body.step(dt);
    }

    // Overlap passes
    for (size_t o = 0; o < 2; ++o) {
      eliminate_crossover(bodies);
    }
    // Process collisions
    process_elastic_coll(bodies, dt);

    // Draw
    window.clear(sf::Color::Black);

    for (auto& body : bodies) {
      body.draw(window, body_shape);
      if (renderForce) {
        body.render_force(window);
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

  return 0;
}
