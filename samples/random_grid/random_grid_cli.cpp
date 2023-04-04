#include <obsidian/geometry/hex/coordinates.h>
#include <obsidian/geometry/hex/round.h>
#include <obsidian/geometry/hex/rotation.h>
#include <obsidian/geometry/hex/neighbor.h>

#include <obsidian/geometry/hex/disk.h>

#include <obsidian/geometry/hex/hash.h>
#include <obsidian/geometry/hex/xy.h>

#include <unordered_map>
#include <random>
#include <algorithm>

#include <string>

#include <iostream>
#include <iomanip>
#include <sstream>

#include "grid.h"

void usage(char const* executable_name) {
  std::cout << executable_name << " <radius> [number = <radius>*2]\n"
    << "\tfills an hex-disk of given radius with several random letters."
    << std::endl;
}

unsigned int read_int(char const* s) {
  std::istringstream stream(s);
  unsigned int i = 0;
  stream >> i;
  return i;
}


namespace hex = geometry::hex;

template <typename T>
std::ostream& operator << (std::ostream& o, hex::basic_point<T> const& p) {
  return o << '<' << std::setw(3) << p.q() << ',' << std::setw(3) << p.r() << '>';
}

template <typename T>
std::ostream& operator << (std::ostream& o, hex::basic_vector<T> const& p) {
  return o << '<' << std::setw(3) << p.q() << '|' << std::setw(3) << p.r() << '>';
}

using grid_type = grid<char>;

using point = grid_type::point;
using vector = grid_type::vector;


std::ostream& print_offset(int n) {
  for (int i = 0; i < n; ++i) std::cout << ' ';
  return std::cout;
}

// formatter used as Formatter(stream, point) << "other text".
// it shall output 9 chars
template <typename Formatter>
void print(grid_type const& g, Formatter formatter) {
  constexpr auto width = 10;

  constexpr char const* angle =   "+";
  constexpr char const* hline =   "-----";

  constexpr char const* trim    =   "+-----+";
  constexpr char const* advance =  "       ";
  constexpr char const* upper   = "/       \\";
  constexpr char const* lower   ="\\       /";

  constexpr vector right_shift{2, -1};
  constexpr vector halfline_shift{1, -1};
  
  point ref{0, g.radius()};

  // widening loop
  for (auto line = 0; line <= g.radius(); ++line) {
    auto const offset = (g.radius() - line) * (width - 2);

    //A: line with labels
    auto & labels = print_offset(offset+2) << trim;
    for (auto i = 0; i < line; ++i) formatter(labels, ref + i * right_shift) << trim;
    labels << '\n';
    
    //B: shape line
    auto & shape = print_offset(offset+1) << upper;
    for (auto i = 0; i < line; ++i) shape << advance << upper;
    shape << '\n';

    if (line > 0) ref|=-hex::neighborhood::i;
  }
  
  // middle loop (<= so that e is done one more time)
  for (auto line = 0; line <= g.radius(); ++line) {
    //e: label first
    auto & half_line = print_offset(0);
    for (auto i = 0; i <= g.radius(); ++i) {
      formatter(half_line << angle, ref + i * right_shift) << angle;
      if (i < g.radius()) half_line << hline;
    }
    half_line << '\n';

    if (line == g.radius()) break;
    
    //f: lower shape
    auto & lower_shape = print_offset(1) << lower;
    for (auto i = 0; i < g.radius(); ++i) lower_shape << advance << lower;
    lower_shape << '\n';
    
    //g: label middle
    auto & true_line = print_offset(2) << trim;
    for (auto i = 0; i < g.radius(); ++i) formatter(true_line, ref + halfline_shift + i * right_shift) << trim;
    true_line << '\n';
    
    //h: higher shape
    auto & higher_shape = print_offset(1) << upper;
    for (auto i = 0; i < g.radius(); ++i) higher_shape << advance << upper;
    higher_shape << '\n';
    
    ref|=-hex::neighborhood::j;
  }
  
  // narrowing loop
  
  for (auto line = 0; line <= g.radius(); ++line) {
    ref|=-hex::neighborhood::k;
    auto const offset = line * (width - 2);

    //S: shape line
    auto & shape = print_offset(offset+1) << lower;
    for (auto i = 0; i < g.radius() - line; ++i) shape << advance << lower;
    shape << '\n';

    //T: line with labels
    auto & labels = print_offset(offset+2) << trim;
    for (auto i = 0; i < g.radius() - line; ++i) formatter(labels, ref + i * right_shift) << trim;
    labels << '\n';
  }
}

int main(int argc, char ** argv) {
  constexpr unsigned int minimal_radius = 1;
  constexpr unsigned int maximal_radius = 7;

  if (argc < 2) {
    std::cout << "no radius provided." << std::endl;
    usage(argv[0]);
    return 1;
  }
  
  auto radius = read_int(argv[1]);
  radius = std::clamp(minimal_radius, radius, maximal_radius);
  
  unsigned int number = radius * 2;
  if (argc > 2) {
    auto n = read_int(argv[2]);
    std::cout << "asked for " << n << " values" << std::endl;
    number = std::clamp(radius * 2, n, 3 * radius * (radius-1));
  }
  
  std::cout << "preparing a disk of radius " << radius << std::endl;

  grid_type map{radius};


  std::cout << "filling " << number << " random letters" << std::endl;

  // Will be used to obtain a seed for the random number engine
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<std::size_t> index(0, map.area());
  std::uniform_int_distribution<char> letters('a', 'z');
  
  while (map.size() < number) {
    map.set( map.position_by_index(index(gen)), letters(gen) );
  }
  
  std::cout << "map filled with " << map.size() << " values.\n";
  /*for (auto const& [k, v] : map.values()) {
    std::cout << "\n\t" << k << "\t: " << v;
  }
  std::cout << std::endl;
  //*/
  
  // print grid
  
  print(map, [&map](std::ostream & o, point const& p) -> std::ostream& {
    return o << "    " << map.get(p, ' ') << "    ";
  });
  
  /*
  auto printer = [](std::ostream & o, point const& p) -> std::ostream& {
    return o << ' ' << std::right << std::setw(3) << p.q() << ',' << std::left << std::setw(3) << p.r() << ' ';
  };
  
  for (int i = 0; i <= 3; ++i) grid{i}.print(printer);
  //*/
  
  return 0;
}
