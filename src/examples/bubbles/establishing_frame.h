#pragma once

#include "Canvas2D.h"

#include "detection/Detection.h"

#include <vector>

namespace math2d {
class Rectangle;
}

namespace bubbles {

using Rectangle = od::Rectangle;

struct AllRectangles {
  std::vector<Rectangle> rectangles;
};

AllRectangles establishing_shot(const Canvas2D &canvas);

void establishing_shot_slices(AllRectangles &ret, const Canvas2D &canvas, const Rectangle& rectangle);

} // namespace bubbles