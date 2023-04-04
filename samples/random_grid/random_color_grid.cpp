#include <obsidian/geometry/hex/coordinates.h>
#include <obsidian/geometry/hex/round.h>
#include <obsidian/geometry/hex/rotation.h>
#include <obsidian/geometry/hex/neighbor.h>

#include <obsidian/geometry/hex/disk.h>

#include <obsidian/geometry/hex/hash.h>

#include <SFML/Graphics.hpp>

#include <random>
#include <algorithm>

#include <string>

#include <iostream>
#include <iomanip>
#include <sstream>

#include "grid.h"
#include "sfhex.h"

void usage(char const* executable_name) {
  std::cout << executable_name << " <radius> [number = <radius>*2]\n"
    << "\tfills an hex-disk of given radius with several random letters."
    << std::endl;
}

unsigned int read_int(char const* s) {
  std::istringstream stream(s);
  unsigned int i = 0;
  stream >> i;
  return i;
}

namespace hex = geometry::hex;


int main(int argc, char ** argv) {
  constexpr unsigned int minimal_radius = 1;
  constexpr unsigned int maximal_radius = 7;

  if (argc < 2) {
    std::cout << "no radius provided." << std::endl;
    usage(argv[0]);
    return 1;
  }
  
  auto radius = read_int(argv[1]);
  radius = std::clamp(minimal_radius, radius, maximal_radius);
  
  unsigned int number = radius * 2;
  if (argc > 2) {
    auto n = read_int(argv[2]);
    std::cout << "asked for " << n << " values" << std::endl;
    number = std::clamp(radius * 2, n, 3 * radius * (radius-1));
  }
  
  std::cout << "preparing a disk of radius " << radius << std::endl;

  grid<sf::Color> map{radius};

  std::cout << "filling " << number << " random letters" << std::endl;

  // Will be used to obtain a seed for the random number engine
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<std::size_t> index(0, map.area());
  std::uniform_int_distribution<sf::Uint8> colors(0, 12);
  
  while (map.size() < number) {
    map.set(
      map.position_by_index(index(gen)),
      sf::Color(135+10*colors(gen), 135+10*colors(gen), 135+10*colors(gen))
    );
  }
  
  std::cout << "map filled with " << map.size() << " values.\n";
  
  
  // SFML part
  
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  sf::RenderWindow window({800, 600}, "Random Grid", sf::Style::Default, settings);
  window.setVerticalSyncEnabled(true);


  
  xy2sf screener{ {400, 300}, {20, -20} };
  
  sf::CircleShape dot(15);
  dot.setOrigin(dot.getRadius(), dot.getRadius());
  dot.setOutlineThickness(0);

  auto grid_cell = SFFlatTopHex(screener, sf::Color::Red);

  while (window.isOpen()) {
      sf::Event event;
      while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();
        
        if (event.type == sf::Event::KeyPressed) {
          if (event.key.code == sf::Keyboard::Escape) {
            window.close();
          } else if (event.key.code == sf::Keyboard::Space) {
            map.clear();
            while (map.size() < number) {
              map.set(
                map.position_by_index(index(gen)),
                sf::Color(135+10*colors(gen), 135+10*colors(gen), 135+10*colors(gen))
              );
            }
          }
        }
      }

      window.clear(sf::Color::Black);
      for(auto const& p : map.surface()) {
        grid_cell.setPosition(
          screener(hex::FlatTop::to_xy(p))
        );
        window.draw(grid_cell);
      }
      
      for (auto const& [pos, color] : map.values()) {
        dot.setPosition(screener(hex::FlatTop::to_xy(pos)));
        dot.setFillColor(color);
        window.draw(dot);
      }

      window.display();
  }
  
  
  return 0;
}
