#ifndef OBSIDIAN_GEOMETRY_HEX_XY_H
#define OBSIDIAN_GEOMETRY_HEX_XY_H

#include <obsidian/geometry/hex/coordinates.h>

#include <type_traits>
#include <array>

// see https://www.redblobgames.com/grids/hexagons/
// and https://www.redblobgames.com/grids/hexagons/implementation.html
// and https://www.redblobgames.com/grids/hexagons/directions.html

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

/* pointy-top wheel:
    <-,1>     <0,1>
          +r
      -q   |   -s
<-,0>    > ⋅ <    <1,0>
      +s   |   +q
          -r
    <0,->     <1,->
*/

// x toward right, y toward up

static constexpr double outer_to_inner_ratio = 0.86602540378;//sqrt(3)/2
/*
let R = outer radius
let e = edge size = R (due to hexagon being made of equilateral triangles)
let r = R cos(30°) = R * sqrt(3)/2
let K = r/R = cos(30°) = sqrt(3)/2

so r = KR

x = R* (q*q2x + r*r2x)
y = R* (q*q2y + r*r2y)

*/
/*
     flat top             pointy top = flat top +30°_cw
          +−−−−−+            +   +
         /       \          / \ / \
  +−−−−−+   1,1   +        +   +   +       y
 /       \       /         |0,1|1,1|       ^
+   0,1   +−−−−−+          +   +   +       |
 \       /       \        / \ / \ /        |
  +−−−−−+   1,0   +      +   +   +       (0,0)−>x
 /       \       /       |0,0|1,0|
+   0,0   +−−−−−+        +   +   +
 \       /                \ / \ /
  +−−−−−+                  +   +

position of centers
<0,0> = R*( 0 , 0K)      <0,0> = R*(0K,  0 )
<1,0> = R*(3/2, 1K)      <1,0> = R*(2K,  0 )
<0,1> = R*( 0 , 2K)      <0,1> = R*(1K, 3/2)
<1,1> = R*(3/2, 3K)      <1,1> = R*(3K, 3/2)

q2x(0°) = 3/2            q2x(30°) = 2K
q2y(0°) = K              q2y(30°) = 0
r2x(0°) = 0              r2x(30°) = K
r2y(0°) = 2K             r2y(30°) = 3/2
*/
struct FlatTopNeighbor {
  static constexpr neighborhood upper_right = neighborhood::i;
  static constexpr neighborhood up = neighborhood::j;
  static constexpr neighborhood upper_left = neighborhood::k;
  static constexpr neighborhood lower_left = neighborhood::i_neg;
  static constexpr neighborhood bottom = neighborhood::j_neg;
  static constexpr neighborhood lower_right = neighborhood::k_neg;
};

struct PointyTopNeighbor {
  static constexpr neighborhood right = neighborhood::i;
  static constexpr neighborhood up_right = neighborhood::j;
  static constexpr neighborhood up_left = neighborhood::k;
  static constexpr neighborhood left = neighborhood::i_neg;
  static constexpr neighborhood down_left = neighborhood::j_neg;
  static constexpr neighborhood down_right = neighborhood::k_neg;
};

/*
  this is computed as basis change from FlatTop:
  (1,0) become (cos A, sin A) and (0,1) becomes (-sin A, cos A)
  x' = x cosA - y sinA
  y' = x sinA + y cosA

due to cos and sin not being constexpr, Basis is only available as FlatTop(A=0) and PointyTop(A=.5)
  static constexpr double q2x = q2flat_x*cos(-A * pi / 3) - q2flat_y*sin(-A * pi / 3);
  static constexpr double q2y = q2flat_x*sin(-A * pi / 3) + q2flat_y*cos(-A * pi / 3);
  static constexpr double r2x = r2flat_x*cos(-A * pi / 3) - r2flat_y*sin(-A * pi / 3);
  static constexpr double r2y = r2flat_x*sin(-A * pi / 3) + r2flat_y*cos(-A * pi / 3);
*/

// x to the right, y to up, 1 unit = 1 hex outer radius
struct xy { double x, y; };
constexpr xy operator+(xy const& a, xy const& b) { return {a.x + b.x, a.y + b.y}; }

template <bool FlatTop>
struct Orientation {
  // expects x to go right and y up
  // let R = outer radius
  // edge size = R (due to hexagon being made of equilateral triangles)
  // let r = R * cos(30°) = R * sqrt(3)/2
  // let K = r/R = cos(30°) = sqrt(3)/2

  using neighbors = std::conditional_t<FlatTop, FlatTopNeighbor, PointyTopNeighbor>;

  static constexpr double K = outer_to_inner_ratio;

  static constexpr double q2x = FlatTop ? 3/2.: 2*K;
  static constexpr double q2y = FlatTop ? K   : 0;
  static constexpr double r2x = FlatTop ? 0   : K;
  static constexpr double r2y = FlatTop ? 2*K : 3/2.;

  //constexpr double half_height = FlatTop ? K : 1;
  //constexpr double half_width  = FlatTop ? 1 : K;

  static constexpr double x2q = FlatTop ? 2/3.  : 2*K/3;
  static constexpr double x2r = FlatTop ? -1/3. : 0;
  static constexpr double y2q = FlatTop ? 0     : -1/3.;
  static constexpr double y2r = FlatTop ? 2*K/3 : 2/3.;

  template <typename T>
  static constexpr xy to_xy(basic_point<T> const& p) {
    return {
      p.q() * q2x + p.r() * r2x,
      p.q() * q2y + p.r() * r2y
    };
  }

  static constexpr basic_point<double> from_xy(double x, double y) {
    return {
      x * x2q + y * y2q,
      x * x2r + y * y2r
    };
  }
  static constexpr basic_point<double> from_xy(xy const& p) { return from_xy(p.x, p.y); }
  
  template <typename T>
  constexpr auto operator()(basic_point<T> const& p) const { return to_xy(p); }
  constexpr auto operator()(xy const& p) const { return from_xy(p); }

  static constexpr xy corner_offset(axis a, double size = 1) {
    switch (axismod6(a)) {
      default:
      case axis::q_pos: return FlatTop ? xy{  1 * size, 0 * size} : xy{ K * size,-.5 * size};
      case axis::s_neg: return FlatTop ? xy{ .5 * size, K * size} : xy{ K * size, .5 * size};
      case axis::r_pos: return FlatTop ? xy{-.5 * size, K * size} : xy{ 0 * size,  1 * size};
      case axis::q_neg: return FlatTop ? xy{ -1 * size, 0 * size} : xy{-K * size, .5 * size};
      case axis::s_pos: return FlatTop ? xy{-.5 * size,-K * size} : xy{-K * size,-.5 * size};
      case axis::r_neg: return FlatTop ? xy{ .5 * size,-K * size} : xy{ 0 * size, -1 * size};
    }
  }
  
  template <axis A>
  static constexpr xy corner_offset(double size = 1) { return corner_offset(A, size); }

  template <typename T>
  static constexpr
  std::array<xy, 6> corners(basic_point<T> const& p, double size = 1) {
    return corners(to_xy(p), size);
  }
    
  static constexpr
  std::array<xy, 6> corners(xy const& center, double size = 1) {
    return {
      center + corner_offset<axis::q_pos>(size),
      center + corner_offset<axis::s_neg>(size),
      center + corner_offset<axis::r_pos>(size),
      center + corner_offset<axis::q_neg>(size),
      center + corner_offset<axis::s_pos>(size),
      center + corner_offset<axis::r_neg>(size)
    };
  }

  static constexpr
  std::array<xy, 6> corners(double size = 1) { return corners(xy{0,0}, size); }
};

using FlatTop = Orientation<true>;
using PointyTop = Orientation<false>;

} // namespace geometry::hex

namespace geometry::screen {

// configuration point
template <typename Point2D>
struct ScreenTraits {
  // each specialization shall provide
  using point = Point2D;
  // using coord_type = ?;
  // static point make(double, double)
  // static value_type const& x(Point2D const&)
  // static value_type const& y(Point2D const&)
};

template <typename Point2D>
struct ScreenTransformation {
  using traits = ScreenTraits<Point2D>;
  using point = typename traits::point;
  using coord_type = typename traits::coord_type;
  
  hex::xy origin;
  hex::xy scale;
  
  auto operator()(hex::xy const& p) const { return to_screen(p); }
  
  hex::xy operator()(point const& p) const { return from_screen(p); }
  hex::xy operator()(coord_type x, coord_type y) const { return from_screen(x, y); }


  point to_screen_relative(hex::xy const& p) const {
    return traits::make(p.x * scale.x, p.y * scale.y);
  }

  point to_screen(hex::xy const& p) const {
    return traits::make(
      origin.x + p.x * scale.x,
      origin.y + p.y * scale.y
    );
  }
  
  hex::xy from_screen(point const& p) const {
    return from_screen(traits::x(p), traits::y(p));
  }
  
  hex::xy from_screen(coord_type x, coord_type y) const {
    return {
      (x - origin.x) / scale.x,
      (y - origin.y) / scale.y
    };
  }
};

} // namespace geometry::screen

#endif
