#pragma once

#include "math2d.h"

#include <functional>
#include <set>
#include <stdexcept>

namespace tiles {

template <class T>
struct Tile {
  std::set<T> _points;
};

template<class T>
class Tiles {
public:
  Tiles() = default;
  ~Tiles() = default;
  Tiles(const Tiles &) = default;
  Tiles &operator=(const Tiles &) = default;
  Tiles(Tiles &&) = default;
  Tiles &operator=(Tiles &&) = default;

  Tiles(int xx, int yy, int N) : _x(xx), _y(yy), _N(N), _tiles{} {
  // allocate tiles
  for (size_t x = 0; x <= getX(xx); ++x) {
    std::vector<Tile<T>> line;
    for (size_t y = 0; y <= getY(yy); ++y) {
      line.emplace_back(Tile<T>{});
    }
    _tiles.emplace_back(line);
  }
}

  void addType(const T &t) {
    if constexpr (std::is_same_v<T, math2d::Point>){
      _tiles[getX(t.x)][getY(t.y)]._points.insert(t);
    }
    else if constexpr(std::is_same_v<T, math2d::Circle>){
      const auto bounding_box = t.bounding_box();
      size_t start_x = getX(bounding_box.lines()[0].start().x);
      size_t end_x = getX(bounding_box.lines()[1].end().x);
      size_t start_y = getY(bounding_box.lines()[0].start().y);
      size_t end_y = getY(bounding_box.lines()[1].end().y);
      for (size_t x = start_x; x <= end_x; ++x) {
        for (size_t y = start_y; y <= end_y; ++y) {
          _tiles[x][y]._points.insert(t);
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

  bool contains(const T &type){
    if constexpr (std::is_same_v<T, math2d::Point>){
      return _tiles[getX(type.x)][getY(type.y)]._points.contains(type);
    }
    else if constexpr(std::is_same_v<T, math2d::Circle>){
      const auto bounding_box = type.bounding_box();
      size_t start_x = getX(bounding_box.lines()[0].start().x);
      size_t end_x = getX(bounding_box.lines()[1].end().x);
      size_t start_y = getY(bounding_box.lines()[0].start().y);
      size_t end_y = getY(bounding_box.lines()[1].end().y);
      for (size_t x = start_x; x <= end_x; ++x) {
        for (size_t y = start_y; y <= end_y; ++y) {
          if(_tiles[x][y]._points.contains(type)){
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
    if(total_tiles_in_x_direction == 0){
      throw std::runtime_error("total_tiles_in_x_direction == 0");
    }
    return static_cast<size_t>(x / total_tiles_in_x_direction);
  }

  size_t getY(double y) const {
    double total_tiles_in_y_direction = _y / _N;
    if(total_tiles_in_y_direction == 0){
      throw std::runtime_error("total_tiles_in_y_direction == 0");
    }
    return static_cast<size_t>(y / total_tiles_in_y_direction);
  }

  int _x = 0;
  int _y = 0;
  int _N = 0;
  std::vector<std::vector<Tile<T>>> _tiles;
};

} // namespace tiles