#include "establishing_frame.h"

#include "Tiles.h"
#include "bubbles_swarm.h"
#include "math2d.h"

#include <algorithm>
#include <optional>
#include <vector>

namespace bubbles {

AllRectangles establishing_shot(const Canvas2D &canvas) {
  constexpr auto debug = false;
  AllRectangles allRectangles;
  tiles::CircleTiles allTriedCircles{canvas.width(), canvas.height(),
                                     canvas.N()};

  for (int x = 0; x < canvas.width(); ++x) {
    for (int y = 0; y < canvas.height(); ++y) {
      if constexpr (debug) {
        std::cout << "x: " << x << " y: " << y << std::endl;
      }
      const auto point = math2d::Point{static_cast<math2d::number_type>(x),
                                       static_cast<math2d::number_type>(y)};
      const auto point_in_circle = [point](const math2d::Circle &circle) {
        const auto distance =
            math2d::Vector{point, circle.center()}.magnitude();
        return distance < circle.radius();
      };
      if (allTriedCircles.for_each(math2d::Rectangle{point, point},
                                   point_in_circle)) {
        continue;
      }
      const auto already_optimized =
          bubbles_algorithm_slow(canvas, point, allTriedCircles);
      const auto bounding_box = already_optimized.bounding_box();
      if (bounding_box.area() > 5) // magic number at the moment
      {
        const auto found_rectangle = math2d::expand_rectangle(bounding_box, 5);
        if constexpr (debug) {
          std::cout << "Found rectangle: " << found_rectangle.toString()
                    << std::endl;
        }
        allRectangles.rectangles.push_back(Rectangle{found_rectangle});
      }
    }
  }
  return allRectangles;
}

struct Slice {
  math2d::Point start = math2d::Point{0, 0};
  math2d::Point end = math2d::Point{0, 0};

  friend constexpr auto operator<=>(const Slice &lhs,
                                    const Slice &rhs) = default;

  bool touches(const Slice &other) const {
    return (start >= other.start && start <= other.end) ||
           (end >= other.start && end <= other.end) ||
           (other.start >= start && other.start <= end) ||
           (other.end >= start && other.end <= end);
  }
};

struct AnnotatedSlice {
  Slice slice;
  size_t line_number = 0;
  friend constexpr auto operator<=>(const AnnotatedSlice &lhs,
                                    const AnnotatedSlice &rhs) = default;
};

struct Slices {
  std::vector<std::vector<AnnotatedSlice>> slices;

  bool contains_slices() const {
    for (const auto &slice_line : slices) {
      if (!slice_line.empty()) {
        return true;
      }
    }
    return false;
  }

  AnnotatedSlice get_first_slice() {
    for (auto &slice_line : slices) {
      if (!slice_line.empty()) {
        auto slice = slice_line.back();
        slice_line.pop_back();
        return slice;
      }
    }
    return AnnotatedSlice{};
  }

  std::vector<AnnotatedSlice>
  get_touching_slices(const std::vector<AnnotatedSlice> &slices_of_object) {
    if (slices_of_object.empty()) {
      return {};
    }
    const auto last_slice = slices_of_object.back();
    const auto line_number = last_slice.line_number;
    if (line_number == slices.size() - 1) {
      return {};
    }
    auto &next_line = slices[line_number + 1];
    std::vector<AnnotatedSlice> ret;
    for (const auto &annotatedSlice : slices_of_object) {
      for (const auto &slice : next_line) {
        if (annotatedSlice.slice.touches(slice.slice)) {
          ret.push_back(slice);
        }
      }
    }
    const auto last = std::unique(ret.begin(), ret.end());
    ret.erase(last, ret.end());
    std::sort(ret.begin(), ret.end());
    std::vector<AnnotatedSlice> cleared_next_line;
    std::set_difference(next_line.begin(), next_line.end(), ret.begin(),
                        ret.end(), std::back_inserter(cleared_next_line));
    slices[line_number + 1] = cleared_next_line;
    return ret;
  }

  Rectangle to_rectangle() const {
    int min_x = 10000000;
    int max_x = 0;
    int min_y = 10000000;
    int max_y = 0;
    for (const auto &slice_line : slices) {
      for (const auto &slice : slice_line) {
        min_x = std::min(min_x, static_cast<int>(slice.slice.start.x));
        max_x = std::max(max_x, static_cast<int>(slice.slice.end.x));
        min_y = std::min(min_y, static_cast<int>(slice.slice.start.y));
        max_y = std::max(max_y, static_cast<int>(slice.slice.end.y));
      }
    }
    return Rectangle{min_x, min_y, max_x - min_x, max_y - min_y};
  }
};

Slices deduce_slices(const Canvas2D &canvas, const Rectangle& rectangle) {
  Slices slices;
  std::optional<AnnotatedSlice> current_slice = std::nullopt;
  for (int y = od::row_min(0, rectangle); y < od::row_max(canvas.height(), rectangle); ++y) {
    auto current_line = std::vector<AnnotatedSlice>{};
    for (int x = od::col_min(0, rectangle); x < od::col_max(canvas.width(), rectangle); ++x) {
      const auto point = math2d::Point{static_cast<math2d::number_type>(x),
                                       static_cast<math2d::number_type>(y)};
      const auto current_pixel_value = canvas.pixel(x, y);
      if (current_pixel_value == 1) {
        if (current_slice.has_value()) {
          current_line.push_back(current_slice.value());
          current_slice = std::nullopt;
        }
      } else {
        if (!current_slice.has_value()) {
          current_slice =
              AnnotatedSlice{Slice{point, point}, static_cast<size_t>(y)};
        } else {
          current_slice->slice.end = point;
        }
      }
    }
    if (current_slice.has_value()) {
      current_line.push_back(current_slice.value());
      current_slice = std::nullopt;
    }
    slices.slices.push_back(current_line);
  }
  return slices;
}

std::vector<Slices> deduce_objects(Slices &slices) {
  std::vector<Slices> objects;
  while (slices.contains_slices()) {
    std::vector<AnnotatedSlice> current_slices;
    const auto first_slice = slices.get_first_slice();
    current_slices.push_back(first_slice);
    Slices current_object;
    current_object.slices.push_back(current_slices);
    while (!current_slices.empty()) {
      current_slices = slices.get_touching_slices(current_slices);
      current_object.slices.push_back(current_slices);
    }
    objects.push_back(current_object);
  }
  return objects;
}

AllRectangles deduce_rectangles(std::vector<Slices> objects) {
  AllRectangles ret;
  for (const auto &object : objects) {
    auto rectangle = object.to_rectangle();
    const auto expanded_rectangle =
        Rectangle{rectangle.x - 5, rectangle.y - 5, rectangle.width + 10,
                  rectangle.height + 10};
    ret.rectangles.push_back(expanded_rectangle);
  }
  return ret;
}

AllRectangles establishing_shot_slices(const Canvas2D &canvas, const Rectangle& rectangle) {
  constexpr auto debug = false;
  if constexpr (debug) {
    std::cout << "establishing_shot_slices" << std::endl;
    std::cout << "deducing slices ..." << std::endl;
  }
  auto slices = deduce_slices(canvas, rectangle);
  if constexpr (debug) {
    std::cout << "slices: " << std::endl;
    for (const auto &slice_line : slices.slices) {
      for (const auto &slice : slice_line) {
        std::cout << slice.slice.start.toString() << " "
                  << slice.slice.end.toString() << " | ";
      }
      std::cout << std::endl;
    }
    std::cout << "deducing objects ..." << std::endl;
  }
  const auto objects = deduce_objects(slices);
  if constexpr (debug) {
    std::cout << "deducing rectangles ..." << std::endl;
  }
  return deduce_rectangles(objects);
}

} // namespace bubbles