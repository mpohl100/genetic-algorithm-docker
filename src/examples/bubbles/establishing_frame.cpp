#include "establishing_frame.h"

#include "Tiles.h"
#include "bubbles_swarm.h"
#include "math2d.h"

namespace bubbles {

Rectangle::Rectangle(const math2d::Rectangle &rectangle)
    : x{static_cast<int>(rectangle.lines()[0].start().x)},
      y{static_cast<int>(rectangle.lines()[0].start().y)},
      width{static_cast<int>(rectangle.lines()[1].end().x -
                             rectangle.lines()[0].start().x)},
      height{static_cast<int>(rectangle.lines()[1].end().y -
                              rectangle.lines()[0].start().y)} {}

math2d::Rectangle Rectangle::to_math2d_rectangle() const
{
  return math2d::Rectangle{math2d::Point{static_cast<math2d::number_type>(x),
                                         static_cast<math2d::number_type>(y)},
                           math2d::Point{static_cast<math2d::number_type>(x + width),
                                         static_cast<math2d::number_type>(y + height)}};
}

AllRectangles establishing_shot(const Canvas2D &canvas) {
  AllRectangles allRectangles;
  tiles::Tiles<math2d::Point> allTriedPoints{canvas.width(), canvas.height(),
                                             10};
  for (int x = 0; x < canvas.width(); ++x) {
    for (int y = 0; y < canvas.height(); ++y) {
      const auto point = math2d::Point{static_cast<math2d::number_type>(x),
                                       static_cast<math2d::number_type>(y)};
      if (allTriedPoints.contains(point)) {
        continue;
      }
      const auto already_optimized = bubbles_algorithm_slow(canvas, point);
      const auto add_tried_points = [&allTriedPoints](const auto &circle) {
        const auto rect = circle.bounding_box();
        const auto start_x = static_cast<int>(rect.lines()[0].start().x);
        const auto end_x = static_cast<int>(rect.lines()[1].end().x);
        const auto start_y = static_cast<int>(rect.lines()[0].start().y);
        const auto end_y = static_cast<int>(rect.lines()[1].end().y);
        for (int x = start_x; x <= end_x; ++x) {
          for (int y = start_y; y <= end_y; ++y) {
            allTriedPoints.addType(
                math2d::Point{static_cast<math2d::number_type>(x),
                              static_cast<math2d::number_type>(y)});
          }
        }
        return false;
      };
      already_optimized.for_each_tried_circle(add_tried_points);
      const auto bounding_box = already_optimized.bounding_box();
      if (bounding_box.area() > 5) // magic number at the moment
      {
        allRectangles.rectangles.push_back(Rectangle{bounding_box});
      }
    }
  }
  return allRectangles;
}

} // namespace bubbles