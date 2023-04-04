#ifndef OBSIDIAN_GEOMETRY_HEX_ROTATION_H
#define OBSIDIAN_GEOMETRY_HEX_ROTATION_H

// see https://www.redblobgames.com/grids/hexagons/
// and https://www.redblobgames.com/grids/hexagons/implementation.html
// and https://www.redblobgames.com/grids/hexagons/directions.html

// in this library, cubic/axial coordinates are used (actually axial: s is not materialized)

// basic_hex is defined in a basis with 3 axis each being rotated by 120° from the previous.
// those axis are q, r and s. units would be uq, ur and us. literal operators would be _q, _r and _s
// a literal point can thus be 1_q or 2_r.  

// this files is limited to q/r/s space

#include <obsidian/geometry/hex/coordinates.h>

namespace geometry::hex {
/*  flat top wheel:

             j
           <0,1>
          +r   -s
k = <-,1>  \   /  <1,0> = i
            \ /
       -q −− ⋅ −− +q
            / \
    <-,0>  /   \  <1,->
          +s   -r
           <0,->
*/

// counterclockwise rotation by steps of 60°
struct integral_rotation {
  constexpr explicit integral_rotation(int n): steps{modulo(n, 6)} {}

  constexpr integral_rotation operator-() const { return integral_rotation{-steps}; }

  integral_rotation& operator+=(int n) { steps = modulo(steps+n, 6); return *this;}
  integral_rotation& operator+=(integral_rotation const& other) { return operator+=(other.steps); }

  int steps;
};

constexpr integral_rotation counterclockwise(int n = 1) { return integral_rotation{n}; }
constexpr integral_rotation clockwise(int n = 1) { return integral_rotation{-n}; }

constexpr integral_rotation operator+(int n, integral_rotation r) { return integral_rotation{r.steps + n}; } 
constexpr integral_rotation operator+(integral_rotation r, int n) { return integral_rotation{r.steps + n}; }

constexpr integral_rotation operator+(integral_rotation r1, integral_rotation r2) { return integral_rotation{r1.steps + r2.steps}; }

// this is sensible to angle
template <typename T>
constexpr basic_vector<T> operator*(basic_vector<T> const& v, integral_rotation const& r) {
  /* cycle is
  < 1, 0> = < 1, 0,-1>

  < 0, 1> = < 0, 1,-1>
  <-1, 1> = <-1, 1, 0>
  <-1, 0> = <-1, 0, 1>
  < 0,-1> = < 0,-1, 1>
  < 1,-1> = < 1,-1, 0>
  < 1, 0> = < 1, 0,-1>
  */
  return basic_vector<T>{ v.get(axis::q_pos-r.steps), v.get(axis::r_pos-r.steps) };
}

template <typename T>
constexpr basic_vector<T> operator*(integral_rotation const& r, basic_vector<T> const& v) { return v * r; }

inline namespace literals {
constexpr auto operator""_ccw(unsigned long long n) { return counterclockwise(static_cast<int>(n%6)); }
constexpr auto operator""_cw(unsigned long long n) { return clockwise(static_cast<int>(n%6)); }
}

static_assert( basic_vector<int>{1,0} * 1_ccw == basic_vector<int>{0,1}, "<1,0> should rotate once to <0,1>" );

} // namespace geometry::hex

#endif
