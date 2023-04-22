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
#include <filesystem>

#include "grid.h"
#include "sfhex.h"

void usage(char const* executable_name) {
  std::cout << executable_name << " [number = 15]\n"
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

struct Stellar {
  enum sizes {
    tiny,
    small,
    medium,
    large,
    huge,
  };

  Stellar(std::string const& name, sizes s, sf::Color color): name(name), shape(5+2*s) {
    shape.setOrigin(shape.getRadius(), shape.getRadius());
    shape.setOutlineThickness(0);
    shape.setFillColor(color);
  }

  std::string name;
  sf::CircleShape shape;
};

struct StarSystem {
public:
  using grid_type = grid<Stellar>;
  using index_type = grid_type::key_type;
  using point_type = grid_type::indexed_type;

  explicit StarSystem(std::mt19937 & generator, index_type planets = 15):
    m_planets(planets),
    m_gen(generator)
  {
    recreate();
  }

  void recreate() {
    m_map.clear();
    m_map.set(0, sol);

    while (m_map.size() < m_planets) {
      auto i = index_distr(m_gen);
      // don't replace sol;
      if (m_map.contains(i)) continue;

      std::ostringstream name;
      name << "Sol " << m_map.size();
      set(i, {
        name.str(),
        Stellar::sizes::huge,
        sf::Color(105+10*colors(m_gen), 105+10*colors(m_gen), 105+10*colors(m_gen))
      });
    }
  }
  
  auto size() const { return m_map.size(); }
  index_type radius() const { return m_map.bounds().radius(); }
  index_type area() const { return m_map.area(); }
  
  decltype(auto) surface() const { return m_map.bounds(); }
  decltype(auto) positions() const { return m_map.positions(); }
  
  point_type index_to_position(index_type i) const {
    return m_map.position_at(i);
  }

  hex::xy index_to_xy(index_type i) const {
    return hex::FlatTop::to_xy(index_to_position(i));
  }

  decltype(auto) values() const { return m_map.mappings(); }
  decltype(auto) values() { return m_map.mappings(); }

private:
  bool set(index_type i, Stellar const& s) { return m_map.set(i, s) != nullptr; }
  
private:
  grid<Stellar> m_map{7};
  Stellar sol{"Sol", Stellar::sizes::huge, { 255, 255, 120 }};
  index_type m_planets;
  
  std::uniform_int_distribution<std::size_t> index_distr{ static_cast<index_type>(0), m_map.area() };
  std::uniform_int_distribution<sf::Uint8> colors { 0, 12 };
  std::mt19937 & m_gen;
};


template <typename T, bool Vector>
auto to_string(hex::basic_hex<T, Vector> const& p) {
  static std::ostringstream buffer;
  buffer.str("");
  buffer << ' ' << std::right << std::setw(3) << p.q() << (Vector ? ',' : ':') << std::left << std::setw(3) << p.r() << ' ';
  return buffer.str();
}

int main(int argc, char ** argv) {
  unsigned int number = 15;
  if (argc > 1) {
    auto n = read_int(argv[1]);
    number = std::clamp<StarSystem::index_type>(15, n, 50);
    std::cout << "Will generate " << number << " planets." << std::endl;
  }

  // Will be used to obtain a seed for the random number engine
  std::random_device rd;
  std::mt19937 gen(rd());

  StarSystem map(gen, number);
  
  // SFML part
  
  const auto local_dir = std::filesystem::path(argv[0]).parent_path();
  const auto current_dir = std::filesystem::current_path();
  
  sf::Font font_sansation;
  if (!font_sansation.loadFromFile(local_dir/"sansation.ttf")) {
    std::cout << "could not load sansation.ttf" << std::endl;
    return 1;
  }


  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  sf::RenderWindow window({800, 600}, "Random star system", sf::Style::Default, settings);
  window.setVerticalSyncEnabled(true);

 
  xy2sfml screener{ {300, 300}, {20, -20} };

  auto grid_cell = SFFlatTopHex(screener, sf::Color::Red);
  
  sf::Text txt_title("Stellar bodies", font_sansation, 20);
  txt_title.setFillColor(sf::Color::White);
  txt_title.setPosition({600, 30});

  sf::Text txt_planet("nothing", font_sansation, 12);
  txt_planet.setFillColor(sf::Color::White);

  // initialize placement of planets
  for (auto& [pos, stellar] : map.values()) {
    stellar.shape.setPosition(screener(map.index_to_xy(pos)));
  }
  
  while (window.isOpen()) {
      sf::Event event;
      while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();
        
        if (event.type == sf::Event::KeyPressed) {
          if (event.key.code == sf::Keyboard::Escape) {
            window.close();
          } else if (event.key.code == sf::Keyboard::Space) {
            map.recreate();
            for (auto& [pos, stellar] : map.values()) {
              stellar.shape.setPosition(screener(map.index_to_xy(pos)));
            }
          }
        }
      }

      window.clear(sf::Color::Black);
      
      window.draw(txt_title);
      
      for(auto const& pos : map.positions()) {
        grid_cell.setPosition( screener(hex::FlatTop::to_xy(pos)) );
        window.draw(grid_cell);
      }
      
      for (int i = 0; auto const& [pos, stellar] : map.values()) {
        window.draw(stellar.shape);
        txt_planet.setFillColor(stellar.shape.getFillColor());
        txt_planet.setString(to_string(map.index_to_position(pos)));
        txt_planet.setPosition(600.f, 60.f + 25.f*i);
        window.draw(txt_planet);
        
        txt_planet.setString(stellar.name);
        txt_planet.setPosition(650.f, 60.f + 25.f*i);
        window.draw(txt_planet);
        ++i;
      }

      window.display();
  }
  
  
  return 0;
}
