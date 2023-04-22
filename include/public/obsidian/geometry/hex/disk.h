#ifndef OBSIDIAN_GEOMETRY_HEX_DISK_H
#define OBSIDIAN_GEOMETRY_HEX_DISK_H

#include <obsidian/geometry/hex/coordinates.h>
#include <obsidian/geometry/hex/neighbor.h>

#include <vector>
#include <ranges>
#include <iterator>

// see https://www.redblobgames.com/grids/hexagons/
// and https://www.redblobgames.com/grids/hexagons/implementation.html
// and https://www.redblobgames.com/grids/hexagons/directions.html

namespace geometry::hex {

/*
official wheel
                  +-----+
                 /       \
          +-----+   0,2   +-----+  <1, 1> = <2,0> + <-1,1> = <0,0> + 2*|i> + 1*(|i>*2_ccw)
         /       \       /       \
  +-----+  -1,2   +-----+   1,1   +-----+
 /       \       /       \       /       \
+  -2,2   +-----+   0,1   +-----+   2,0   +
 \       /       \       /       \       /
  +-----+  -1,1   +-----+   1,0   +-----+
 /       \       /       \       /       \
+  -2,1   +-----+   0,0   +-----+   2,-1  +
 \       /       \       /       \       /
  +-----+  -1,0   +-----+   1,-1  +-----+
 /       \       /       \       /       \
+  -2,0   +-----+   0,-1  +-----+   2,-2  +
 \       /       \       /       \       /
  +-----+  -1,-1  +-----+   1,-2  +-----+
         \       /       \       /
          +-----+   0,-2  +-----+
                 \       /
                  +-----+
*/


using ring_index = std::size_t;
using ring_radius = std::size_t;

using disk_index = ring_index;
using disk_radius = ring_radius;

constexpr ring_index ring_size(ring_radius radius) { return radius == 0 ? 1 : 6 * radius; }
constexpr ring_index ring_end_index(ring_radius radius) { return ring_size(radius); }

constexpr disk_index disk_size(disk_radius radius) {
  // center case + sum<1,radius>(6*i)
  // = 1 + 6 * sum<1,radius>(i)
  // = 1 + 6 * (radius * (radius+1) / 2)
  return 1 + 3 * radius * (radius+1);
}

constexpr disk_index disk_end_index(disk_radius radius) { return disk_size(radius); }

namespace details {

constexpr auto ring_index_range(ring_radius radius) {
  return std::views::iota(static_cast<ring_index>(0), ring_end_index(radius));
}

constexpr auto disk_index_range(ring_radius radius) {
  return std::views::iota(static_cast<ring_index>(0), disk_end_index(radius));
}


template <typename T>
constexpr basic_vector<T> vector_in_ring(ring_radius radius, ring_index i) {
  // for out of bound, return <0,0>.
  // for radius = 0 and i = 0, return <0, 0>
  if (i >= 6 * radius) return zero<T>;

  auto segment_start_dir = neighborhood::i + (i/radius);
  auto segment_dir = segment_start_dir + 2;
  
  return static_cast<T>(radius) * neighbor_vector<T>(segment_start_dir)
    + static_cast<T>(i%radius) * neighbor_vector<T>(segment_dir);
}

template <typename T>
constexpr basic_vector<T> vector_in_disk(disk_radius radius, disk_index index) {
  for (disk_radius r = 0; r <= radius; ++r) {
    disk_index const size = ring_size(r);
    if (index < size) {
      return vector_in_ring<T>(r, index);
    }
    index -= size;
  }
  // out of bounds
  return zero<T>;
}

template <typename T>
constexpr disk_index disk_index_of(basic_vector<T> const& v) {
  auto const radius = length(v);
  if (radius == 0) return 0;
  
  // the point is in one of the segment.
  auto const offset = disk_size(radius - 1);

  if (-v.s() == radius) {
    return offset + v.r();
  }
  
  if (v.r() == radius) {
    return offset + radius - v.q();
  }
  
  if (-v.q() == radius) {
    return offset + 2 * radius + v.s();
  }
  
  if (v.s() == radius) {
    return offset + 3 * radius - v.r();
  }
  
  if (-v.r() == radius) {
    return offset + 4 * radius + v.q();
  }
  
  // so q == radius (offset+0 == <radius,0,-radius> was covered by -v.s() == radius)
  return offset + 5 * radius - v.s();
}

static_assert( vector_in_disk<int>(3, 10) == basic_vector<int>{-1,2}, "algorithmic error");
static_assert( disk_index_of(basic_vector<int>{-1,2}) == 10, "algorithmic error");

} // namespace details

template <typename T>
auto ring_around(basic_point<T> const& center, ring_radius radius) {
  return details::ring_index_range(radius)
  | std::views::transform([radius, center](ring_index i){ return center + details::vector_in_ring<T>(radius, i);});
}

template <typename T, bool Vector = false>
auto ring(ring_radius radius) {
  if constexpr (Vector) {
    return details::ring_index_range(radius)
    | std::views::transform([radius](ring_index i){ return details::vector_in_ring<T>(radius, i); });
  } else {
    return ring_around(origin<T>, radius);
  }
}


template <typename T, bool Vector = false>
class basic_disk {
public:
  using value_type = basic_hex<T, Vector>;
  using index_type = disk_index;

  using radius_type = disk_radius;
  using vector_type = basic_vector<T>;


private:
  static auto make_view(radius_type radius) {
    return details::disk_index_range(radius);
  }

public:
  using view_type = decltype( make_view(0) );

  basic_disk(radius_type radius): m_view{ make_view(radius) }, m_radius(radius) {}
  
  radius_type radius() const { return m_radius; }
  index_type size() const { return disk_size(radius()); }

  bool is_valid(index_type index) const {
    return index < size();
  }
  

  value_type value_at(index_type i) const {
    if constexpr (Vector) {
      return details::vector_in_disk<T>(radius(), i);
    } else {
      return origin<T> + details::vector_in_disk<T>(radius(), i);
    }
  }

  index_type index_of(value_type const& v) {
    if constexpr (Vector) {
      return details::disk_index_of(v);
    } else {
      return details::disk_index_of(v - origin<T>);
    }
  }

  view_type view() const { return m_view; }

private:
  view_type m_view;
  radius_type m_radius;
};



template <disk_radius Radius, typename T, bool Vector = false>
class basic_fixed_disk {
public:
  using value_type = basic_hex<T, Vector>;
  using index_type = disk_index;

  using radius_type = disk_radius;
  using vector_type = basic_vector<T>;

  static constexpr radius_type radius() { return Radius; }
  static constexpr index_type size() { return disk_size(radius()); }

private:
  static auto make_view() {
    return details::disk_index_range(radius());
  }

public:
  using view_type = decltype( make_view() );


  static constexpr bool is_valid(index_type index) {
    return index < size();
  }

  constexpr value_type value_at(index_type i) const {
    if constexpr (Vector) {
      return vector_in_disk<T>(radius(), i);
    } else {
      return origin<T> + vector_in_disk<T>(radius(), i);
    }
  }

  constexpr index_type index_of(value_type const& v) {
    if constexpr (Vector) {
      return details::disk_index_of(v);
    } else {
      return details::disk_index_of(v - origin<T>);
    }
  }

  view_type view() const { return make_view(); }
};



template <typename T>
using disk = basic_disk<T, false>;

template <typename T>
using offsets_disk = basic_disk<T, true>;


template <disk_radius Radius, typename T>
using fixed_disk = basic_fixed_disk<Radius, T, false>;

template <disk_radius Radius, typename T>
using offsets_fixed_disk = basic_fixed_disk<Radius, T, true>;



namespace integers {
template <bool Vector = false>
using basic_disk = hex::basic_disk<base_type, Vector>;

template <disk_radius Radius, bool Vector = false>
using basic_fixed_disk = hex::basic_fixed_disk<Radius, base_type, Vector>;


using disk = basic_disk<false>;
using offsets_disk = basic_disk<true>;

template <disk_radius Radius>
using fixed_disk = basic_fixed_disk<false>;

template <disk_radius Radius>
using offsets_fixed_disk = basic_fixed_disk<true>;
}

namespace doubles {
template <bool Vector = false>
using basic_disk = hex::basic_disk<base_type, Vector>;

template <disk_radius Radius, bool Vector = false>
using basic_fixed_disk = hex::basic_fixed_disk<Radius, base_type, Vector>;


using disk = disk<base_type>;
using offsets_disk = offsets_disk<base_type>;

template <disk_radius Radius>
using fixed_disk = basic_fixed_disk<false>;

template <disk_radius Radius>
using offsets_fixed_disk = basic_fixed_disk<true>;
}

} // namespace geometry::hex

#endif
