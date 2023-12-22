#pragma once

#include "examples/bubbles/Tiles.h"
#include "examples/bubbles/math2d.h"
#include "value_test.h"

#include <cmath>
#include <set>
#include <string>
#include <vector>

namespace bubbles {

class Canvas2D {
public:
  Canvas2D() = default;
  Canvas2D(const Canvas2D &) = default;
  Canvas2D &operator=(const Canvas2D &) = default;
  Canvas2D(Canvas2D &&) = default;
  Canvas2D &operator=(Canvas2D &&) = default;
  Canvas2D(int xx, int yy, int N = 10);

  void draw_rectangle(const math2d::Rectangle &rectangle);
  void draw_circle(const math2d::Circle &circle);
  std::string getPixels() const;
  void draw_line(const math2d::Line &line);
  bool is_within(const math2d::Point &point) const;

  const std::set<math2d::Point> &points() const;
  const tiles::Tiles<math2d::Point>& tiles() const;
private:
  void draw_pixel(int x, int y, int value);
  [[maybe_unused]] int _x = 1;
  [[maybe_unused]] int _y = 1;
  std::vector<std::vector<int>> _pixels;
  std::set<math2d::Point> _points;
  tiles::Tiles<math2d::Point> _tiles;
};

} // namespace bubbles