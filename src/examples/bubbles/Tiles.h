#pragma once

#include "math2d.h"

#include <functional>
#include <set>

namespace tiles {

struct Tile {
  std::set<math2d::Point> _points;
};

class Tiles {
public:
  Tiles() = default;
  Tiles(const Tiles &) = default;
  Tiles &operator=(const Tiles &) = default;
  Tiles(Tiles &&) = default;
  Tiles &operator=(Tiles &&) = default;
  Tiles(int xx, int yy, int N);

  void addPoint(const math2d::Point &point) {
    _tiles[getX(point.x)][getY(point.y)]._points.insert(point);
  }

  bool for_each(const math2d::Rectangle &rectangle,
                std::function<bool(math2d::Point)> func) const {
    size_t start_x = getX(rectangle.lines()[0].start().x);
    size_t end_x = getX(rectangle.lines()[1].end().x);
    size_t start_y = getY(rectangle.lines()[0].start().y);
    size_t end_y = getY(rectangle.lines()[1].end().y);
    for (size_t x = start_x; x <= end_x; ++x) {
      for (size_t y = start_y; y <= end_y; ++y) {
        for (const auto &point : _tiles[x][y]._points) {
          bool ret = func(point);
          if(ret){
            return true;
          }
        }
      }
    }
    return false;
  }

private:
  size_t getX(double x) const {
    double total_tiles_in_x_direction = _x / _N;
    return static_cast<size_t>(x / total_tiles_in_x_direction);
  }

  size_t getY(double y) const {
    double total_tiles_in_y_direction = _y / _N;
    return static_cast<size_t>(y / total_tiles_in_y_direction);
  }

  int _x = 0;
  int _y = 0;
  int _N = 0;
  std::vector<std::vector<Tile>> _tiles;
};

} // namespace tiles