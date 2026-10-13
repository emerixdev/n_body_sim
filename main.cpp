#include "../smlib/smlib.hpp"
#include <cstddef>
#include <format>
#include <iostream>

const static size_t dimensions = 2;
const static size_t bodies_n = 2;
struct Delta_state {
  smlib::mvec<float, dimensions> vel;
  smlib::mvec<float, dimensions> acc;
  Delta_state(smlib::mvec<float, dimensions> a, smlib::mvec<float, dimensions> b) {
    vel = a;
    acc = b;
  };
};
struct Body_state {
  smlib::mvec<float, dimensions> pos;
  smlib::mvec<float, dimensions> vel;
  float mass;
  Body_state(smlib::mvec<float, dimensions> a, smlib::mvec<float, dimensions> b, float c) {
    pos = a;
    vel = b;
    mass = c;
  };
  Body_state(smlib::mvec<float, dimensions> a, smlib::mvec<float, dimensions> b) {
    pos = a;
    vel = b;
    mass = 1;
  };
  Body_state() { mass = 1; };
};
void operator+=(Body_state &bs, const Delta_state &ds) {
  bs.pos += ds.vel;
  bs.vel += ds.acc;
};
int main(int argc, char *argv[]) {
  std::cout << "Meow Meow\n";
  std::array<Body_state, bodies_n> bodies;
  bodies[0].pos = {1, 0};
  bodies[0].vel = {0, 1};
  bodies[1].pos = {0, 1};
  bodies[1].vel = {1, 0};
  for (auto it : bodies) {
    std::cout << std::format("pos: {}\nvel: {}\nmass: {}\n\n", it.pos, it.vel, it.mass);
  }

  return 0;
}
