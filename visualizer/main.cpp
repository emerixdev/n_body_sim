#include "../../smlib/smlib.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Window.hpp>
#include <array>
#include <cstddef>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
static const size_t bodies_n = 2;
static const unsigned block_size = 100;
int eof = -1;
void make_grid(sf::VertexArray &lines, const sf::Color &color, sf::Vector2f offset, unsigned vertical_cells,
               unsigned horizontal_cells, float spacing);
void load_blocks(std::array<std::ifstream, bodies_n> &f,
                 std::array<std::array<smlib::mvec<float, 2>, block_size>, bodies_n> &blocks);
void draw_bodies(sf::RenderTarget &target ,const auto &blocks,
                 std::array<sf::CircleShape, bodies_n> &bodies_shapes, int step, const auto origin_offset = {0, 0},
                 int draw_scale = 1);
int main() {

  const unsigned winWidth = 1080;
  const unsigned winHeight = 1080;
  const int scale = 50;
  const int radius = 5;
  const unsigned fps_limit = 60;
  const unsigned font_size = 10;
  const sf::Time text_update_time = sf::milliseconds(500); // how long to wait between text updates;
  smlib::mvec<int, 2> origin_offset = {winWidth, winHeight};
  origin_offset /= 2;
  std::array<std::ifstream, bodies_n> files;
  std::array<std::array<smlib::mvec<float, 2>, block_size>, bodies_n> blocks;
  float dt = 0;
  sf::Clock fps_clock;
  sf::Clock update_text_clock;
  sf::Font font;
  sf::Text text_fps(font);
  sf::VertexArray grid_lines(sf::PrimitiveType::Lines);
  std::array<sf::CircleShape, bodies_n> bodies_shapes;
  sf::RenderWindow window(sf::VideoMode({winWidth, winHeight}), "sfml_win", sf::Style::Close);
  sf::RenderTexture texture({winWidth, winHeight});

  if (!font.openFromFile("./0xProtoNerdFontMono-Regular.ttf")) {
    std::cerr << "Error: couldnt load font\n";
    return 1;
  }
  for (int i = 0; i < bodies_n; ++i) {
    std::stringstream ss;
    ss << "./../output/" << i << ".txt";
    files[i].open(ss.str());
    if (!files[i].is_open()) {
      std::cerr << "Error: couldnt open file " << i << "\n";
      return 1;
    }
  }

  text_fps.setString(std::format("fps/fps_limit: {}/{}", 0, fps_limit));
  text_fps.setCharacterSize(font_size);
  text_fps.setFillColor(sf::Color::Red);
  text_fps.setPosition({0, 0});

  window.setFramerateLimit(fps_limit);

  make_grid(grid_lines, sf::Color::White, {(winWidth - 21 * 50) / 2.f, (winHeight - 21 * 50) / 2.f}, 21, 21, 50);
  // texture.draw(grid_lines);
  int step = 0;
  int block_n = 0;
  block_n++;
  load_blocks(files, blocks);
  for (size_t i = 0; i < bodies_n; ++i) {
    bodies_shapes[i].setRadius(radius);
    bodies_shapes[i].setFillColor(
        {static_cast<uint8_t>(255 / (i + 1)), static_cast<uint8_t>(255 - 255 / (i + 1)), 255});
  }
  while (window.isOpen()) {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
      // Close window: exit
      if (event->is<sf::Event::Closed>())
        window.close();
    }

    // update text
    if (update_text_clock.getElapsedTime() > text_update_time) {
      if (dt != 0)
        text_fps.setString(std::format("fps/fps_limit: {:.1f}/{}", 1.f / dt, fps_limit));
      update_text_clock.restart();
    }
    window.clear(sf::Color::Black);
    window.draw(grid_lines);
    window.draw(text_fps);
    if (step < block_size && eof < 0) {
      draw_bodies(window, blocks, bodies_shapes, step, origin_offset, scale);
      draw_bodies(texture, blocks, bodies_shapes, step, origin_offset, scale);
      -----------------------------------------------------------------------------------------------------
      // draw as normal
      step++;
    } else if (eof >= 0) {
      if (step < eof - block_size * (block_n - 1)) {
        // reached eof while loading block but the current step in block is still valid
        // draw as normal
        // draw_bodies(window, blocks, bodies_shapes, step, origin_offset, scale);
        step++;
      }
    } else if (eof < 0) {
      // didnt reach eof yet
      block_n++;
      step = 0;
      load_blocks(files, blocks);
      // draw_bodies(window, blocks, bodies_shapes, step, origin_offset, scale);
    } else {
      // reached eof
    }

    window.display();
    dt = fps_clock.restart().asSeconds();
  }
  return 0;
}
void make_grid(sf::VertexArray &lines, const sf::Color &color, sf::Vector2f offset, unsigned vertical_cells,
               unsigned horizontal_cells, float spacing) {
  lines.clear();
  lines.setPrimitiveType(sf::PrimitiveType::Lines);
  lines.resize((vertical_cells + horizontal_cells + 2) * 2);
  for (unsigned i = 0; i <= vertical_cells; ++i) {
    lines[2 * i].position = {offset.x + i * spacing, offset.y};
    lines[2 * i + 1].position = {offset.x + i * spacing, offset.y + horizontal_cells * spacing};
    lines[2 * i].color = color;
    lines[2 * i + 1].color = color;
  }
  for (unsigned i = 0; i <= horizontal_cells; ++i) {
    lines[2 * (vertical_cells + 1 + i)].position = {offset.x, offset.y + i * spacing};
    lines[2 * (vertical_cells + 1 + i) + 1].position = {offset.x + vertical_cells * spacing, offset.y + i * spacing};
    lines[2 * (vertical_cells + 1 + i)].color = color;
    lines[2 * (vertical_cells + 1 + i) + 1].color = color;
  }
}
void load_blocks(std::array<std::ifstream, bodies_n> &f,
                 std::array<std::array<smlib::mvec<float, 2>, block_size>, bodies_n> &blocks) {
  for (size_t body = 0; body < bodies_n; ++body) {
    for (size_t i = 0; i < block_size; ++i) {
      std::string s;
      if (getline(f[body], s)) {
        std::stringstream ss(s);
        for (int j = 0; j < 2; j++) {
          ss >> blocks[body][i][j];
        }
        // std::cout << std::format("blocks[{}][{}]: {}\n", body, i, blocks[body][i]);
      } else {
        std::cout << "End of file;\n";
        eof = i;
        return;
      }
    }
  }
  std::cout << "loaded block\n";
}
void draw_bodies(sf::RenderTarget &target, const auto &blocks,
                 std::array<sf::CircleShape, bodies_n> &bodies_shapes, int step, const auto origin_offset ,
                 int draw_scale) {
  for (size_t i = 0; i < bodies_n; ++i) {
    smlib::mvec<float, 2> pos = blocks[i][step];
    pos *= draw_scale;
    std::cout << std::format("body={}; step={}; PRE-OFFSET: x={}, y={}; POST-OFFSET: x=", i, step, pos.x(), pos.y());
    pos += smlib::cast<int, float>(origin_offset);
    pos -= bodies_shapes[i].getRadius() / 2.f;
    std::cout << pos.x() << ", y=" << pos.y() << "\n";
    bodies_shapes[i].setPosition({pos.x(), pos.y()});
    target.draw(bodies_shapes[i]);
  }
}
