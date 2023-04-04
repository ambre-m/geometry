#ifndef OBSIDIAN_GEOMETRY_HEX_ROUND_H
#define OBSIDIAN_GEOMETRY_HEX_ROUND_H

// see https://www.redblobgames.com/grids/hexagons/
// and https://www.redblobgames.com/grids/hexagons/implementation.html
// and https://www.redblobgames.com/grids/hexagons/directions.html

#include <obsidian/geometry/hex/coordinates.h>
#include <cmath>
#include <type_traits>

namespace geometry::hex {

template <typename I = integers::base_type, typename D>
requires(std::is_integral_v<I> && std::is_floating_point_v<D>)
constexpr basic_point<I> round(basic_point<D> const& p) {
    const I q = I(std::round(p.q));
    const I r = I(std::round(p.r));
    const I s = I(std::round(p.s));

    const D dq = std::abs(p.q - q);
    const D dr = std::abs(p.r - r);
    const D ds = std::abs(p.s - s);

    if (dq > dr && dq > ds) {
      return basic_point<I>::rs(r, s);
    } else if (dr > ds) {
      return basic_point<I>::sq(s, q);
    } else {
      return basic_point<I>::qr(q, r);
    }
}

} // namespace geometry::hex

#endif
