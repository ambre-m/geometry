#ifndef OBSIDIAN_GEOMETRY_HEX_INTERPOLATION_H
#define OBSIDIAN_GEOMETRY_HEX_INTERPOLATION_H

#include <obsidian/geometry/hex/coordinates.h>

#include <type_traits>

// see https://www.redblobgames.com/grids/hexagons/
// and https://www.redblobgames.com/grids/hexagons/implementation.html
// and https://www.redblobgames.com/grids/hexagons/directions.html

namespace interpolation {

template <typename T, typename Scale = double>
auto linear(T && a, T && b, Scale t) {
  // idea is: a * (1-t) + b * t;
  // but better precision is achieved with 
  return a + (b - a) * t; 
}

}

namespace geometry::hex {

// 3.2 Line drawing
To draw a line, I linearly interpolate between two hexes, and then round it to the nearest hex.
To linearly interpolate between hex coordinates I linearly interpolate each of the components (q, r, s) independently:


// TODO: define point * double and vector * double
// this can't be just interpolation::linear since point * double is not defined.
// this can change, but 
template <typename T, typename Scale = double>
basic_point<decltype(std::declval(T) * std::declval(Scale))>
linear(basic_point<T> const& a, basic_point<T> const& b, double t) {
  return {
    interpolation::linear(a.q(), b.q(), t),
    interpolation::linear(a.r(), b.r(), t),
    interpolation::linear(a.s(), b.s(), t),
  };
}

// Line drawing is not too bad once I have linear interpolation:
template <typename T>
std::vector<basic_point<T>> linedraw(basic_point<T> const& a, basic_point<T> const& b) {
  int const N = distance(a, b);
  double const step = 1.0 / max(N, 1);
  std::vector<basic_point<T>> results {};
  for (int i = 0; i <= N; i++) {
    results.push_back(round(lerp(a, b, i * step)));
  }
  return results;
}

// I needed to stick that max(N, 1) bit in there to handle lines with length 0 (when A == B).

/*
Sometimes the hex_lerp will output a point that’s on an edge.
On some systems, the rounding code will push that to one side or the other, somewhat unpredictably and inconsistently.
To make it always push these points in the same direction, add an “epsilon” value to a.
This will “nudge” things in the same direction when it’s on an edge, and leave other points unaffected.
*/
template <typename T>
std::vector<basic_point<T>> linedraw_nudge(basic_point<T> a, basic_point<T> b) {
static constexpr basic_point<double> nudge {1e-6, 1e-6}; 
  auto const a_nudge = a + nudge;
  auto const b_nudge = b + nudge;
  int const N = distance(a, b);
  double const step = 1.0 / max(N, 1);
  std::vector<basic_point<T>> results {};
  for (int i = 0; i <= N; i++) {
    results.push_back( round( lerp(a_nudge, b_nudge, step * i) ) );
  }
  return results;
}

// The nudge is not always needed. You might try without it first.

} // namespace geometry::hex

#endif
