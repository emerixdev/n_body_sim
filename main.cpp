#include "../smlib/smlib.hpp"
#include <array>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>

const static size_t dimensions = 2;
const static size_t bodies_n = 1;
const static size_t steps = 10000;
struct Delta_state {
  smlib::mvec<float, dimensions> vel;
  smlib::mvec<float, dimensions> acc;
  Delta_state(smlib::mvec<float, dimensions> a, smlib::mvec<float, dimensions> b) {
    vel = a;
    acc = b;
  };
  Delta_state(smlib::mvec<float, dimensions> a) { vel = a; };
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
  Body_state(float a = 0) { mass = a; };
};
struct Time {
  float value;
  Time(float a = 0) { value = a; }
  Time operator+(Time other) const { return Time(value + other.value); }
  Time operator-(Time other) const { return Time(value - other.value); }
  Time operator*(float other) const { return Time(value * other); }
  Time &operator+=(Time other) {
    value += other.value;
    return *this;
  }
  Time &operator-=(Time other) {
    value -= other.value;
    return *this;
  }
  Time &operator*=(float other) {
    value *= other;
    return *this;
  }
};
Body_state &operator+=(Body_state &bs1, const Body_state &bs2) {
  if (bs1.mass != bs2.mass && bs1.mass != 0 && bs2.mass != 0) {
    throw std::invalid_argument("Cannot add Body_state objects with conflicting non-zero masses");
  }
  bs1.pos += bs2.pos;
  bs1.vel += bs2.vel;
  if (bs1.mass == 0) {
    bs1.mass = bs2.mass;
  }
  return bs1;
}
Body_state operator+(const Body_state &bs1, const Body_state &bs2) {
  Body_state out = bs1;
  out += bs2;
  return out;
};
Body_state operator*(Delta_state &ds, const Time t) {
  Body_state out(ds.vel * t.value, ds.acc * t.value);
  return out;
}

int main(int argc, char *argv[]) {
  std::filesystem::path output_dir("./output");
  if (!exists(output_dir)) {
    create_directory(output_dir);
    std::cout << "Directory created: " << output_dir << "\n";
  }
  std::array<std::filesystem::path, bodies_n> output_files;
  for (size_t i = 0; i < output_files.size(); ++i) {
    output_files[i] = output_dir / std::to_string(i) / ".txt";
    std::ofstream f(output_files[i]);
    if (f.is_open()) {
      f << "";
      /*
      if (f.peek() != std::ifstream::traits_type::eof()) {
        std::cout << std::format("File {} is not empty, file size is {} bytes, overwrite? (Y/n)",
                                 output_files[i].filename().string(), std::filesystem::file_size(output_files[i]));
      }*/
      f.close();
    } else
      std::cout << "Couldnt create file " << output_files[i].string() << "\n";
  }

  Time dt(1.f / steps);
  std::array<Body_state, bodies_n> bodies_bs;
  bodies_bs[0].pos = {0, -1};
  bodies_bs[0].vel = {1, 0};
  bodies_bs[0].mass = 1;
  bodies_bs[1].pos = {0, 1};
  bodies_bs[1].vel = {-1, 0};
  bodies_bs[1].mass = 1;
  for (auto it : bodies_bs) {
    std::cout << std::format("pos: {}\nvel: {}\nmass: {}\n\n", it.pos, it.vel, it.mass);
  }
  for (size_t i = 0; i < steps; ++i) {
    for (auto &it : bodies_bs) {
      Delta_state ds(it.vel, {1, 0});
      it += ds * dt;
    }
  }
  for (auto it : bodies_bs) {
    std::cout << std::format("pos: {}\nvel: {}\nmass: {}\n\n", it.pos, it.vel, it.mass);
  }
  return 0;
}
