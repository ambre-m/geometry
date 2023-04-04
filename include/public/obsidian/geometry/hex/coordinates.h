#ifndef OBSIDIAN_GEOMETRY_HEX_COORDINATES_H
#define OBSIDIAN_GEOMETRY_HEX_COORDINATES_H

// see https://www.redblobgames.com/grids/hexagons/
// and https://www.redblobgames.com/grids/hexagons/implementation.html
// and https://www.redblobgames.com/grids/hexagons/directions.html

// in this library, cubic/axial coordinates are used (actually axial: s is not materialized)

// basic_hex is defined in a basis with 3 axis each being rotated by 120° from the previous.
// those axis are q, r and s. units would be uq, ur and us. literal operators would be _q, _r and _s
// a literal point can thus be 1_q or 2_r.  

// this files is limited to q/r/s space



/*
                                            A                   +−−−−−+                   A i = -2
                                            B                  /       \                  B
        -1  A           +−−−−−+             A           +−−−−−+   0,2   +−−−−−+           C
            B          /       \            B          /       \       /       \          D 
            A   +−−−−−+   0,1   +−−−−−+     A   +−−−−−+  -1,2   +−−−−−+   1,1   +−−−−−+   A i = -1
            B  /       \       /       \    B  /       \       /       \       /       \  B 
        0   e +  -1,1   +−−−−−+   1,0   +   e +  -2,2   +−−−−−+   0,1   +−−−−−+   2,0   + C 
            f  \       /       \       /    f  \       /       \       /       \       /  D 
            g   +−−−−−+   0,0   +−−−−−+     g   +−−−−−+  -1,1   +−−−−−+   1,0   +−−−−−+   A i = 0
            h  /       \       /       \    h  /       \       /       \       /       \  B 
        1   e +  -1,0   +−−−−−+   1,-1  +   e +  -2,1   +−−−−−+   0,0   +−−−−−+   2,-1  + C 
            S  \       /       \       /    f  \       /       \       /       \       /  D 
            T   +−−−−−+   0,-1  +−−−−−+     g   +−−−−−+  -1,0   +−−−−−+   1,-1  +−−−−−+   A i = 1
            S          \       /            h  /       \       /       \       /       \  B 
     finsih T           +−−−−−+             e +  -2,0   +−−−−−+   0,-1  +−−−−−+   2,-2  + C 
                                            S  \       /       \       /       \       /  D 
                                            T   +−−−−−+  -1,-1  +−−−−−+   1,-2  +−−−−−+   A i = 2
                                            S          \       /       \       /          B 
                                            T           +−−−−−+   0,-2  +−−−−−+           C 
                                            S                  \       /                  D 
                                            T                   +−−−−−+                   0 finish
*/


#include <type_traits>

namespace geometry::hex {

template <typename T>
constexpr T modulo(T x, T n) { return (x < 0) ? x % n + n : x % n; }

template <typename E, typename I = std::underlying_type<E>::type>
constexpr E axismod6(E e, I shift = 0) { return static_cast<E>( modulo(static_cast<I>(e) + shift, 6) );  }

// actually, only rotation is sensible to axis order.
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

enum struct axis {
  q_pos,
  s_neg,
  r_pos,
  q_neg,
  s_pos,
  r_neg,
};

constexpr axis operator+(axis a, int shift) { return axismod6(a, shift); }
constexpr axis operator-(axis a, int shift) { return a + -shift; }
constexpr axis operator-(axis a) { return a + 3; }

constexpr axis& operator++(axis & a) { return a = a + 1; }

// axial/cube shape
// Q+R+S = 0
// Q = -R -S
// R = -S -Q
// S = -Q -R
// S is computed.
template <typename T, bool vector>
class basic_hex {
public:

  using value_type = T;

  constexpr basic_hex(): m_q{0}, m_r{0} {}
  constexpr basic_hex(value_type q, value_type r): m_q{q}, m_r{r} {}

  static constexpr basic_hex qr(value_type q, value_type r) { return basic_hex{q, r}; }
  static constexpr basic_hex rs(value_type r, value_type s) { return basic_hex{-r-s, r}; }
  static constexpr basic_hex sq(value_type s, value_type q) { return basic_hex{q, -q-s}; }

  constexpr value_type q() const { return m_q; }
  constexpr value_type r() const { return m_r; }
  constexpr value_type s() const { return -m_q - m_r; }

  constexpr value_type get(axis a) const {
    switch (a) {
      default:
      case axis::q_pos: return  q();
      case axis::r_neg: return -r();
      case axis::s_pos: return  s();
      case axis::q_neg: return -q();
      case axis::r_pos: return  r();
      case axis::s_neg: return -s();
    }
  }

  template <axis A>
  constexpr value_type get() const { return get(A); }

private:  
  value_type m_q;
  value_type m_r;
};


template <typename T, bool vector>
constexpr bool operator == (basic_hex<T,vector> const& a, basic_hex<T,vector> const& b) {
  return a.q() == b.q() && a.r() == b.r();
}

template <typename T, bool vector>
constexpr bool operator != (basic_hex<T,vector> const& a, basic_hex<T,vector> const& b) {
  return !(a == b);
}



template <typename T>
using basic_point = basic_hex<T, false>;

template <typename T>
constexpr basic_point<T> origin{0, 0};


template <typename T>
using basic_vector = basic_hex<T, true>;

template <typename T>
constexpr basic_vector<T> zero{0, 0};




//vector algebra

template <typename T>
constexpr basic_vector<T> operator-(basic_vector<T> const& v) { return {-v.q(), -v.r()}; }


template <typename T>
constexpr basic_vector<T> operator+(basic_vector<T> const& a, basic_vector<T> const& b) { return {a.q() + b.q(), a.r() + b.r()}; }


template <typename T>
constexpr basic_vector<T> operator-(basic_vector<T> const& a, basic_vector<T> const& b) { return {a.q() - b.q(), a.r() - b.r()}; }

template <typename T>
constexpr basic_vector<T> operator*(basic_vector<T> const& v, T const& scale) { return {scale * v.q(), scale * v.r()}; }

template <typename T>
constexpr basic_vector<T> operator*(T const& scale, basic_vector<T> const& v) { return {scale * v.q(), scale * v.r()}; }

template <typename T>
constexpr basic_vector<T> operator/(basic_vector<T> const& v, T const& scale) { return {v.q() / scale, v.r() / scale}; }



// point algebra

template <typename T>
constexpr basic_point<T> operator+(basic_point<T> const& p, basic_vector<T> const& v) { return {p.q() + v.q(), p.r() + v.r()}; }

template <typename T>
constexpr basic_point<T> operator-(basic_point<T> const& p, basic_vector<T> const& v) { return {p.q() - v.q(), p.r() - v.r()}; }


template <typename T>
constexpr basic_vector<T> operator-(basic_point<T> const& a, basic_point<T> const& b) { return {a.q() - b.q(), a.r() - b.r()}; }

// distance measures are complex notion. here is "cell hops" distance


template <typename T>
constexpr auto length(basic_vector<T> const& v) {
  return (v.q() > v.r()) ?
    (v.q() > v.s() ? v.q() : v.s()):
    (v.r() > v.s() ? v.r() : v.s());
}


template <typename T>
constexpr auto distance(basic_point<T> const& a, basic_point<T> const& b) { return length(b - a); }

namespace integers {
using base_type = int;

using point = basic_point<base_type>;
using vector = basic_vector<base_type>;
}

namespace doubles {
using base_type = double;

using point = basic_point<base_type>;
using vector = basic_vector<base_type>;
}

} // namespace geometry::hex

#endif
