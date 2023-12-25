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
  constexpr auto debug = false;
  AllRectangles allRectangles;
  tiles::CircleTiles allTriedCircles{canvas.width(), canvas.height(), canvas.N()};

  for (int x = 0; x < canvas.width(); ++x) {
    for (int y = 0; y < canvas.height(); ++y) {
      if constexpr (debug) {
        std::cout << "x: " << x << " y: " << y << std::endl;
      }
      const auto point = math2d::Point{static_cast<math2d::number_type>(x),
                                       static_cast<math2d::number_type>(y)};
      const auto point_in_circle = [point](const math2d::Circle &circle) {
        const auto distance = math2d::Vector{point, circle.center()}.magnitude();
        return distance < circle.radius();
      };
      if (allTriedCircles.for_each(math2d::Rectangle{point, point}, point_in_circle)) {
        continue;
      }
      const auto already_optimized = bubbles_algorithm_slow(canvas, point, allTriedCircles);
      const auto bounding_box = already_optimized.bounding_box();
      if (bounding_box.area() > 5) // magic number at the moment
      {
        const auto found_rectangle = math2d::expand_rectangle(bounding_box, 5);
        if constexpr (debug){
            std::cout << "Found rectangle: " << found_rectangle.toString() << std::endl;
        }
        allRectangles.rectangles.push_back(Rectangle{found_rectangle});
      }
    }
  }
  return allRectangles;
}

} // namespace bubbles