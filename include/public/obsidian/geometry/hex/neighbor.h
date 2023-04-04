#ifndef OBSIDIAN_GEOMETRY_HEX_NEIGHBOR_H
#define OBSIDIAN_GEOMETRY_HEX_NEIGHBOR_H

// see https://www.redblobgames.com/grids/hexagons/
// and https://www.redblobgames.com/grids/hexagons/implementation.html
// and https://www.redblobgames.com/grids/hexagons/directions.html

// in this library, cubic/axial coordinates are used (actually axial: s is not materialized)

// basic_hex is defined in a basis with 3 axis each being rotated by 120° from the previous.
// those axis are q, r and s. units would be uq, ur and us. literal operators would be _q, _r and _s
// a literal point can thus be 1_q or 2_r.  

// this files is limited to q/r/s space

#include <obsidian/geometry/hex/coordinates.h>
#include <obsidian/geometry/hex/rotation.h>

#include <array>

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

enum struct neighborhood {
  i = 0, // beetwen -r and +q
  j = 1, // beetwen +q and -s
  k = 2, // beetwen -s and +r
  i_neg = 3, // beetwen +r and -q 
  j_neg = 4, // beetwen -q and +s
  k_neg = 5, // beetwen +s and -r
};

constexpr neighborhood operator+(neighborhood n, int rotation) { return axismod6(n, rotation); }
constexpr neighborhood operator-(neighborhood n, int rotation) { return n + -rotation; }
constexpr neighborhood operator-(neighborhood n) { return n + 3; }
constexpr neighborhood& operator++(neighborhood& n) { return n = n + 1; }

constexpr std::array<neighborhood, 6> neighborhoods {
  neighborhood::i,
  neighborhood::j,
  neighborhood::k,
  neighborhood::i_neg,
  neighborhood::j_neg,
  neighborhood::k_neg,
};


// for (neighborhood dir = neighborhood::i; dir <= neighborhood::k_neg; ++dir) {

template <typename T> constexpr basic_vector<T> basic_i {1, 0};
template <typename T> constexpr basic_vector<T> basic_j = basic_i<T> * 1_ccw;
template <typename T> constexpr basic_vector<T> basic_k = basic_j<T> * 1_ccw;

template <typename T>
constexpr basic_vector<T> neighbor_vector(neighborhood n) { return basic_i<T> * counterclockwise(static_cast<int>(n)); }

template <typename T>
constexpr basic_point<T> neighbor(basic_point<T> const& p, neighborhood n) { return p + neighbor_vector<T>(n); }

template <typename T>
constexpr basic_point<T> operator|(basic_point<T> const& p, neighborhood n) { return neighbor(p, n); }

template <typename T>
constexpr basic_point<T>& operator|=(basic_point<T>& p, neighborhood n) { return p = neighbor(p, n); }


template <typename T, neighborhood N>
constexpr basic_vector<T> basic_neighbor = neighbor_vector<T>(N);

template <typename T, neighborhood N>
constexpr basic_point<T> neighbor(basic_point<T> const& p) { return p + basic_neighbor<T, N>; }



template <typename T>
constexpr basic_vector<T> basic_ij = basic_i<T> + basic_j<T>;

template <typename T>
constexpr basic_vector<T> diagonal_neighbor_vector(axis a) { return basic_ij<T> * counterclockwise(static_cast<int>(a)); }

template <typename T>
constexpr basic_point<T> diagonal_neighbor(basic_point<T> const& p, axis a) { return p + basic_diagonal_neighbor<T>(a); }


template <typename T, axis A>
constexpr basic_vector<T> basic_diagonal_neighbor = diagonal_neighbor_vector<T>(A);

template <typename T, axis A>
constexpr basic_point<T> diagonal_neighbor(basic_point<T> const& p) { return p + basic_diagonal_neighbor<T, A>; }



namespace integers {
constexpr vector i = basic_i<base_type>;
constexpr vector j = basic_j<base_type>;
constexpr vector k = basic_k<base_type>;

constexpr vector ij = basic_ij<base_type>;
}

namespace doubles {
constexpr vector i = basic_i<base_type>;
constexpr vector j = basic_j<base_type>;
constexpr vector k = basic_k<base_type>;

constexpr vector ij = basic_ij<base_type>;
}

} // namespace geometry::hex

#endif
