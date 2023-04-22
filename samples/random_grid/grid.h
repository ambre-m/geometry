#ifndef RANDOMGRID_GRID_H
#define RANDOMGRID_GRID_H

#include <obsidian/geometry/core/map.h>
#include <obsidian/geometry/hex/disk.h>
#include <obsidian/geometry/hex/hash.h>

template <typename Value>
using grid = geometry::core::indexed_sparse_map<
  geometry::hex::disk<int>,
  Value
>;

#endif