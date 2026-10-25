#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>
#include <cstddef>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
void make_grid(sf::VertexArray &lines, const sf::Color &color, sf::Vector2f offset, unsigned vertical_cells,
               unsigned horizontal_cells, float spacing);
static const size_t bodies_n = 2;
int main() {

  const unsigned winWidth = 1080;
  const unsigned winHeight = 1080;
  const unsigned fps_limit = 60;
  const unsigned font_size = 10;
  const sf::Time text_update_time = sf::milliseconds(500); // how long to wait between text updates;
  std::array<std::ifstream, bodies_n> files;
  float dt = 0;
  sf::Clock fps_clock;
  sf::Clock update_text_clock;
  sf::Font font;
  sf::Text text_fps(font);
  sf::VertexArray grid_lines(sf::PrimitiveType::Lines);
  if (!font.openFromFile("./0xProtoNerdFontMono-Regular.ttf")) {
    std::cerr << "Error: couldnt load font\n";
    return 1;
  }
  for (int i = 0; i < bodies_n; ++i) {
    std::stringstream ss;
    ss << "./../output/" << i << ".txt";
    files[i].open(ss.str());
    if (!files[i].is_open()) {
      std::cerr << "Error: couldnt open file "<<i<<"\n";
      return 1;
    }
  }

  text_fps.setString(std::format("fps/fps_limit: {}/{}", 0, fps_limit));
  text_fps.setCharacterSize(font_size);
  text_fps.setFillColor(sf::Color::Red);
  text_fps.setPosition({0, 0});
  sf::RenderWindow window(sf::VideoMode({winWidth, winHeight}), "sfml_win", sf::Style::Close);
  window.setFramerateLimit(fps_limit);

  make_grid(grid_lines, sf::Color::White, {(winWidth - 21 * 50) / 2.f, (winHeight - 21 * 50) / 2.f}, 21, 21, 50);

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
