#ifndef OBSIDIAN_GEOMETRY_CORE_SURFACE_H
#define OBSIDIAN_GEOMETRY_CORE_SURFACE_H

#include <ranges>

namespace geometry::core {

// surfaces used for maps.

/* a surface should have the following api:

struct Surface {
  using position_type = ?;
  
  bool is_valid(position_type const& p) const;
  
  maybe some iteration process
  
  
};

*/

template <typename Surface>
struct surface_traits {
  using surface_type = Surface;
  using value_type = typename surface_type::value_type;

  static constexpr bool is_valid(surface_type const& surface, value_type const& v) {
    return surface.is_valid(v);
  }
  

  static constexpr auto size(surface_type const& surface) {
    return surface.size();
  }
};


/* an indexed surface should have the following api:

struct Surface {
  using indexed_type = ?;
  using value_type = ?;
  
  
  value_type value_at(index_type i) const;
  index_type index_of(value_type const& v) const;
  index_type index_of(value_type && v) const;
  
  bool is_valid(index_type const& i) const;
  
  maybe some iteration process
  
  
};

*/

template <typename Surface>
struct indexed_surface_traits: surface_traits<Surface> {
  using surface_type = Surface;
  using typename surface_traits<Surface>::value_type;
  using index_type = typename surface_type::index_type;


  static constexpr bool is_valid(surface_type const& surface, index_type index) {
    return surface.is_valid(index);
  }

  static constexpr value_type value_at(surface_type const& surface, index_type index) {
    return surface.value_at(index);
  }

  static constexpr index_type index_of(surface_type const& surface, value_type const& v) {
    return surface.index_of(v);
  }
  
  static constexpr auto indices(surface_type const& surface) {
    return std::views::iota(static_cast<index_type>(0), surface_traits<Surface>::size(surface));
  }
};

} // namespace geometry::core

#endif
