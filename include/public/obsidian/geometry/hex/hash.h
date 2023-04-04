#ifndef OBSIDIAN_GEOMETRY_HEX_HASH_H
#define OBSIDIAN_GEOMETRY_HEX_HASH_H

#include <obsidian/geometry/hex/coordinates.h>

#include <utility>

// see https://www.redblobgames.com/grids/hexagons/
// and https://www.redblobgames.com/grids/hexagons/implementation.html
// and https://www.redblobgames.com/grids/hexagons/directions.html

template <typename T, bool Vector>
struct std::hash< geometry::hex::basic_hex<T, Vector> > {
  size_t operator()(geometry::hex::basic_hex<T, Vector> const& h) const {
    hash<T> hasher;
    auto hq = hasher(h.q());
    auto hr = hasher(h.r());
    return hq ^ hr;
  }
};

#endif
