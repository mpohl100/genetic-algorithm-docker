#include "Tiles.h"

namespace tiles {

Tiles::Tiles(int xx, int yy, int N) : _x(xx), _y(yy), _N(N), _tiles{} {
  // allocate tiles
  for (size_t x = 0; x <= getX(xx); ++x) {
    std::vector<Tile> line;
    for (size_t y = 0; y <= getY(yy); ++y) {
      line.emplace_back(Tile{});
    }
    _tiles.emplace_back(line);
  }
}

} // namespace tiles