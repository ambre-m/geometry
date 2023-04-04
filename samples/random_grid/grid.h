#ifndef RANDOMGRID_GRID_H
#define RANDOMGRID_GRID_H

#include <obsidian/geometry/hex/coordinates.h>
#include <obsidian/geometry/hex/neighbor.h>

#include <obsidian/geometry/hex/disk.h>
#include <obsidian/geometry/hex/hash.h>

#include <unordered_map>
#include <vector>
#include <ranges>

template <
  typename Value,
  typename Underlying = int
>
class grid {
public:
  using radius_type = geometry::hex::disk_radius;
  using coord_type = Underlying;
  using value_type = Value;

  using point = geometry::hex::basic_point<coord_type>;
  using vector = geometry::hex::basic_vector<coord_type>;
  using index_type = geometry::hex::disk_index;

  explicit grid(radius_type radius):
    m_radius{ radius },
    m_surface{ geometry::hex::disk<coord_type>(radius) },
    m_content{}
  {}

  radius_type radius() const { return m_surface.radius(); }
  auto area() const { return m_surface.size(); }
  point position_by_index(index_type i) const { return m_surface.get(i); }
  auto surface() const { return m_surface.view(); }


  auto size() const { return m_content.size(); }
  auto const& values() const { return m_content; }
  auto& values() { return m_content; }
  
  void clear() { m_content.clear(); }
  
  bool is_valid(index_type i) const { return i < area(); }
  
  bool is_valid(point const& p) const {
    return p - geometry::hex::origin<coord_type> <= radius();
  }
  
  value_type const* optional(index_type i) const { return optional(position_by_index(i)); }
  value_type* optional(index_type i) { return optional(position_by_index(i)); }

  value_type const* optional(coord_type q, coord_type r) const { return optional(point{q, r}); }
  value_type* optional(coord_type q, coord_type r) { return optional(point{q, r}); }
  
  value_type const* optional(point const& p) const {
    auto it = m_content.find(p);
    return it == m_content.end() ? nullptr : &it->second;
  }
  
  value_type* optional(point const& p) {
    auto it = m_content.find(p);
    return it == m_content.end() ? nullptr : &it->second;
  }


  value_type const& get(index_type i, value_type const& fallback) {
    auto v = optional(i);
    return v == nullptr ? fallback : *v;
  }

  value_type const& get(coord_type q, coord_type r, value_type const& fallback) {
    auto v = optional(q,r);
    return v == nullptr ? fallback : *v;
  }

  value_type const& get(point const& p, value_type const& fallback) {
    auto v = optional(p);
    return v == nullptr ? fallback : *v;
  }

  value_type& set(point const& p, value_type const& value) {
    return m_content.insert_or_assign(p, value).first->second;
  }

  value_type& set(index_type i, value_type const& value) {
    return set(position_by_index(i), value);
  }
  
  bool contains(index_type i) const {
    return optional(i) != nullptr;
  }

private:  
  radius_type m_radius;
  geometry::hex::disk<coord_type> m_surface;
  std::unordered_map<point, Content> m_content;
};

#endif