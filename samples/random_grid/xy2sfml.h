#ifndef XY2SFML_H
#define XY2SFML_H

#include <obsidian/geometry/hex/xy.h>
#include <SFML/Graphics.hpp>

template <>
struct geometry::screen::ScreenTraits<sf::Vector2f> {
  // each specialization shall provide
  using point = sf::Vector2f;
  using coord_type = float;
  
  static point make(double x, double y) {
    return {
      static_cast<float>(x),
      static_cast<float>(y)
    };
  }
  static float const& x(point const& p) { return p.x; }
  static float const& y(point const& p) { return p.y; }
};

using xy2sf = geometry::screen::ScreenTransformation<sf::Vector2f>;

#endif