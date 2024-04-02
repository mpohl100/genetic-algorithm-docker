#pragma once

#include "Canvas2D.h"
#include "math2d.h"

#include <vector>

namespace bubbles {

struct AllRectangles {
  std::vector<math2d::Rectangle> rectangles;
};

AllRectangles establishing_shot(const Canvas2D &canvas);

} // namespace bubbles