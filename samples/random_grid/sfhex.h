#ifndef SFHEX_H
#define SFHEX_H

#include "xy2sfml.h"
#include <SFML/Graphics.hpp>

class SFFlatTopHex : public sf::Drawable, public sf::Transformable {
public:
  SFFlatTopHex(xy2sfml const& screener, sf::Color color):
    m_vertices{sf::PrimitiveType::LineStrip, 7}
  {
    auto corners = geometry::hex::FlatTop::corners();
    for (int i = 0; auto const& c : corners) {
      auto & v = m_vertices[i++];
      v.position = screener.to_screen_relative(c);
      v.color = color;
    }
    m_vertices[6] = m_vertices[0];
  }

private:
  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // apply the entity's transform -- combine it with the one that was passed by the caller
    states.transform *= getTransform(); // getTransform() is defined by sf::Transformable

    // apply the texture
    //states.texture = &m_texture;

    // you may also override states.shader or states.blendMode if you want

    target.draw(m_vertices, states);
  }

  sf::VertexArray m_vertices;
};

#endif