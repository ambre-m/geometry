#ifndef OBSIDIAN_GEOMETRY_CORE_MAP_H
#define OBSIDIAN_GEOMETRY_CORE_MAP_H

#include <obsidian/geometry/core/surface.h>
#include <unordered_map>
#include <ranges>

namespace geometry::core {

// maps values to position in a map.
/*
unbound map: map anywhere to some value
bound map: restricted to a surface

indexed map: support is not point, but some linear index
*/


template <typename Key, typename Value>
class basic_sparse_map {
public:
  using key_type = Key;
  using value_type = Value;

  auto mappings() const { return std::views::all(m_content); }
  auto mappings() { return std::views::all(m_content); }

  auto values() const { return m_content | std::views::values; }
  auto keys() const { return m_content | std::views::keys; }

  auto size() const { return m_content.size(); }
  void clear() { m_content.clear(); }

  bool contains(key_type const& k) const {
    return m_content.find(k) != m_content.cend();
  }

protected:
  basic_sparse_map() = default;
  ~basic_sparse_map() = default;

  value_type const* optional(key_type const& p) const {
    auto const it = m_content.find(p);
    return it == m_content.end() ? nullptr : &it->second;
  }

  value_type* optional(key_type const& p) {
    auto const it = m_content.find(p);
    return it == m_content.end() ? nullptr : &it->second;
  }

  value_type& set(key_type const& p, value_type const& value) {
    return m_content.insert_or_assign(p, value).first->second;
  }

  value_type const& get(key_type const& p, value_type const& fallback) const {
    auto const v = optional(p);
    return v == nullptr ? fallback : *v;
  }

private:
  std::unordered_map<key_type, value_type> m_content;
};



// unbound specialization
template <typename Key, typename Value>
class sparse_map: public basic_sparse_map<Key, Value> {
private:
  using base = basic_sparse_map<Key, Value>;

public:
  using base::optional;
  using base::get;
  using base::set;
  using base::contains;
};


template <typename Bounds, typename Value>
class bounded_sparse_map:
  public basic_sparse_map<
    typename core::surface_traits<Bounds>::value_type,
    Value
  > {
private:
  using traits = surface_traits<Bounds>;
  using base = basic_sparse_map<typename traits::value_type, Value>;

public:
  using bounds_type = Bounds;
  using typename base::key_type;
  using typename base::value_type;

  bounded_sparse_map(bounds_type const& bounds): m_bounds{ bounds } {}


  auto const& bounds() const { return m_bounds; }
  
  auto area() const { return traits::size(m_bounds); }


  bool is_valid(key_type const& p) const {
    return traits::is_valid(m_bounds, p);
  }

  value_type const* optional(key_type const& p) const {
    return is_valid(p) ? base::optional(p) : nullptr;
  }

  value_type* optional(key_type const& p) {
    return is_valid(p) ? base::optional(p) : nullptr;
  }

  value_type* set(key_type const& p, value_type const& value) {
    if (!is_valid(p)) return nullptr;
    return & base::set(p, value);
  }

  // decide if this function shall return nullptr or fallback if p is invalid.
  value_type const* get(key_type const& p, value_type const& fallback) const {
    return is_valid(p) ? base::get(p, fallback) : nullptr;
  }

private:
  bounds_type m_bounds;
};


/*
IndexedBounds provides:
conversions between position (indexed_type) and index (index_type)
limit: operator()(indexed_type)

*/
template <typename IndexedBounds, typename Value>
class indexed_sparse_map:
  public basic_sparse_map<
    typename core::indexed_surface_traits<IndexedBounds>::index_type,
    Value
  > {
private:
  using traits = core::indexed_surface_traits<IndexedBounds>;
  using base = basic_sparse_map<typename traits::index_type, Value>;

public:
  using bounds_type = IndexedBounds;
  using indexed_type = typename IndexedBounds::value_type;
  using typename base::key_type;
  using index_type = IndexedBounds::index_type;
  using typename base::value_type;

  static_assert(std::is_same<index_type, key_type>::value, "key should be index");

  indexed_sparse_map(bounds_type const& bounds): m_bounds{ bounds } {}


  auto const& bounds() const { return m_bounds; }

  auto area() const { return traits::size(bounds()); }

  auto indices() const { return traits::indices(bounds()); }
  auto positions() const {
    return indices() | std::views::transform(
      [this](index_type i){ return traits::value_at(this->m_bounds, i); }
    ); }


  using base::contains;

  bool is_valid(key_type const& p) const {
    return traits::is_valid(bounds(), p);
  }

  value_type const* optional(key_type const& p) const {
    return is_valid(p) ? base::optional(p) : nullptr;
  }

  value_type* optional(key_type const& p) {
    return is_valid(p) ? base::optional(p) : nullptr;
  }

  value_type* set(key_type const& p, value_type const& value) {
    if (!is_valid(p)) return nullptr;
    return & base::set(p, value);
  }

  // decide if this function shall return nullptr or fallback if p is invalid.
  value_type const* get(key_type const& p, value_type const& fallback) const {
    return is_valid(p) ? base::get(p, fallback) : nullptr;
  }



  auto position_at(index_type i) const {
    return traits::value_at(bounds(), i);
  }

  auto index_of(indexed_type const& p) const {
    return traits::index_of(bounds(), p);
  }

  bool contains(indexed_type const& p) const {
    return contains(index_of(p));
  }

  bool is_valid(indexed_type const& p) const {
    return is_valid(index_of(p));
  }

  value_type const* optional(indexed_type const& p) const {
    return optional(index_of(p));
  }

  value_type* optional(indexed_type const& p) {
    return optional(index_of(p));
  }

  value_type const* get(indexed_type const& p, value_type const& fallback) const {
    return get(index_of(p), fallback);
  }

  value_type* set(indexed_type const& p, value_type const& value) {
    return set(index_of(p), value);
  }

private:
  bounds_type m_bounds;
};



} // namespace geometry::core

#endif