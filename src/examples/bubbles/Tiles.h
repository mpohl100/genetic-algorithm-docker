#pragma once

#include "Matrix.h"
#include "math2d.h"

#include <algorithm>
#include <functional>
#include <set>
#include <stdexcept>

namespace tiles {

template <class T> struct Tile {
  bool contains(const T &t) const {
    return std::find(_points.begin(), _points.end(), t) != _points.end();
  }
  const std::vector<T> &points() const { return _points; }
  void insert(const T &t) {
    const auto it = std::lower_bound(_points.begin(), _points.end(), t);
    _points.insert(it, t);
  }

private:
  std::vector<T> _points;
};

template <class T> class Tiles {
public:
  Tiles() = default;
  ~Tiles() = default;
  Tiles(const Tiles &) = default;
  Tiles &operator=(const Tiles &) = default;
  Tiles(Tiles &&) = default;
  Tiles &operator=(Tiles &&) = default;

  Tiles(int xx, int yy, int N)
      : _x(xx), _y(yy), _N(N), _tiles{getX(xx) + 1, getY(yy) + 1} {}

  void addType(const T &t) {
    if constexpr (std::is_same_v<T, math2d::Point>) {
      _tiles.get(getX(t.x), getY(t.y)).insert(t);
    } else if constexpr (std::is_same_v<T, math2d::Circle>) {
      const auto bounding_box = t.bounding_box();
      size_t start_x = getX(bounding_box.lines()[0].start().x);
      size_t end_x = getX(bounding_box.lines()[1].end().x);
      size_t start_y = getY(bounding_box.lines()[0].start().y);
      size_t end_y = getY(bounding_box.lines()[1].end().y);
      for (size_t x = start_x; x <= end_x; ++x) {
        for (size_t y = start_y; y <= end_y; ++y) {
          _tiles.get(x, y).insert(t);
        }
      }
    }
  }

  bool for_each(const math2d::Rectangle &rectangle,
                std::function<bool(T)> func) const {
    size_t start_x = getX(rectangle.lines()[0].start().x);
    size_t end_x = getX(rectangle.lines()[1].end().x);
    size_t start_y = getY(rectangle.lines()[0].start().y);
    size_t end_y = getY(rectangle.lines()[1].end().y);
    for (size_t x = start_x; x <= end_x; ++x) {
      for (size_t y = start_y; y <= end_y; ++y) {
        for (const auto &point : _tiles.get(x, y).points()) {
          bool ret = func(point);
          if (ret) {
            return true;
          }
        }
      }
    }
    return false;
  }

  bool contains(const T &type) {
    if constexpr (std::is_same_v<T, math2d::Point>) {
      return _tiles.get(getX(type.x), getY(type.y)).contains(type);
    } else if constexpr (std::is_same_v<T, math2d::Circle>) {
      const auto bounding_box = type.bounding_box();
      size_t start_x = getX(bounding_box.lines()[0].start().x);
      size_t end_x = getX(bounding_box.lines()[1].end().x);
      size_t start_y = getY(bounding_box.lines()[0].start().y);
      size_t end_y = getY(bounding_box.lines()[1].end().y);
      for (size_t x = start_x; x <= end_x; ++x) {
        for (size_t y = start_y; y <= end_y; ++y) {
          if (_tiles.get(x, y).contains(type)) {
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
    if (total_tiles_in_x_direction == 0) {
      throw std::runtime_error("total_tiles_in_x_direction == 0");
    }
    return static_cast<size_t>(x / total_tiles_in_x_direction);
  }

  size_t getY(double y) const {
    double total_tiles_in_y_direction = _y / _N;
    if (total_tiles_in_y_direction == 0) {
      throw std::runtime_error("total_tiles_in_y_direction == 0");
    }
    return static_cast<size_t>(y / total_tiles_in_y_direction);
  }

  int _x = 0;
  int _y = 0;
  int _N = 0;
  matrix::Matrix<Tile<T>> _tiles;
};


using CircleTiles = Tiles<math2d::Circle>;

} // namespace tiles