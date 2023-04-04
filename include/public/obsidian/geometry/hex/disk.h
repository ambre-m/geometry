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



namespace details {
constexpr auto ring_index_range(ring_radius radius) {
  return std::views::iota(static_cast<ring_index>(0), ring_end_index(radius));
}

template <typename T>
basic_vector<T> vector_in_ring(ring_radius radius, ring_index i) {
  // for out of bound, return <0,0>.
  // for radius = 0 and i = 0, return <0, 0>
  if (i >= 6 * radius) return zero<T>;

  auto segment_start_dir = neighborhood::i + (i/radius);
  auto segment_dir = segment_start_dir + 2;
  
  return static_cast<T>(radius) * neighbor_vector<T>(segment_start_dir)
    + static_cast<T>(i%radius) * neighbor_vector<T>(segment_dir);
}

template <typename T>
basic_vector<T> vector_in_disk(disk_radius radius, disk_index index) {
  for (disk_radius r = 0; r <= radius; ++r) {
    disk_index const size = ring_size(r);
    if (index < size) {
      return vector_in_ring<T>(r, index);
    }
    index -= size;
  }

  // out of size
  return zero<T>;
}
} // namespace details


template <typename T>
auto ring_offsets(ring_radius radius) {
  return details::ring_index_range(radius)
  | std::views::transform([radius](ring_index i){ return details::vector_in_ring<T>(radius, i);});
}

template <typename T>
auto ring_around(basic_point<T> const& center, ring_radius radius) {
  return details::ring_index_range(radius)
  | std::views::transform([radius, center](ring_index i){ return center + details::vector_in_ring<T>(radius, i);});
}

template <typename T>
auto ring(ring_radius radius) {
  return ring_around(origin<T>, radius);
}

template <typename T, bool Vector = false>
class disk {
public:
  using value_type = basic_hex<T, Vector>;
  using radius_type = disk_radius;
  using index_type = disk_index;

private:
  static auto make_view(radius_type radius) {
    if constexpr (Vector) {
      return std::views::iota(radius_type(0), radius+1)
      | std::views::transform([](radius_type i) { return ring_offsets<T>(i); })
      | std::views::join;
    } else {
      return std::views::iota(radius_type(0), radius+1)
      | std::views::transform([](radius_type i) { return ring<T>(i); })
      | std::views::join;
    }
  }
  
public:
  using view_type = decltype( make_view(0) );

  disk(radius_type radius): m_view{ make_view(radius) }, m_radius(radius) {}
  
  radius_type radius() const { return m_radius; }
  index_type size() const { return disk_size(m_radius); }

  value_type get(index_type i) const {
    if constexpr (Vector) {
      return details::vector_in_disk<T>(m_radius, i);
    } else {
      return origin<T> + details::vector_in_disk<T>(m_radius, i);
    }
  }

  view_type view() const { return m_view; }

private:
  view_type m_view;
  radius_type m_radius;
};

namespace integers {
template <bool Vector = false>
using disk = hex::disk<base_type, Vector>;
}

namespace doubles {
template <bool Vector = false>
using disk = hex::disk<base_type, Vector>;
}
} // namespace geometry::hex

#endif
