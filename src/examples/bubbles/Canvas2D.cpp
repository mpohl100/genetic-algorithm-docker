#include "Canvas2D.h"

#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

namespace bubbles {

Point::Point(int xx, int yy) : x(xx), y(yy) {}

Line::Line(Point start, Point end) : _start(start), _end(end) {}

Rectangle::Rectangle(Point tl, Point br) 
: _lines()
{
  _lines.emplace_back(tl, Point(br.x, tl.y));
  _lines.emplace_back(Point(br.x, tl.y), br);
  _lines.emplace_back(br, Point(tl.x, br.y));
  _lines.emplace_back(Point(tl.x, br.y), tl);
}

std::vector<Line> Rectangle::lines() const {
  return _lines;
}

Circle::Circle(Point center, int radius) : _center(center), _radius(radius) {}

Point rotate_around(Point target, Point center, int angle) {
  // implement a method that rotates the point target around the point center
  // by the given angle
  int radius = std::sqrt(std::pow(target.x - center.x, 2) +
                         std::pow(target.y - center.y, 2));
  int angle_rad = angle * M_PI / 180;
  target.x = center.x + radius * std::cos(angle_rad);
  target.y = center.y + radius * std::sin(angle_rad);
  return target;
}

Canvas2D::Canvas2D(int xx, int yy)
    : _x(xx), _y(yy), _pixels(xx, std::vector<int>(yy, 0)) {}

void Canvas2D::draw_rectangle([[maybe_unused]] const Point &tl,
                              [[maybe_unused]] const Point &br) {
  draw_line(tl, Point(br.x, tl.y));
  draw_line(Point(br.x, tl.y), br);
  draw_line(br, Point(tl.x, br.y));
  draw_line(Point(tl.x, br.y), tl);
}

void Canvas2D::draw_circle(const Point &center, int radius) {
  int centerX = center.x;
  int centerY = center.y;

  for (int x = centerX - radius; x <= centerX + radius; x++) {
    for (int y = centerY - radius; y <= centerY + radius; y++) {
      // Use the circle equation to determine if a point is on the circle's
      // outline
      if (std::abs(std::pow(static_cast<int>(x) - centerX, 2) +
                   std::pow(static_cast<int>(y) - centerY, 2) -
                   std::pow(radius, 2)) < 1.5) {
        draw_pixel(x, y);
      }
    }
  }
}

std::string Canvas2D::getPixels() const {
  std::string ret;
  for (const auto &line : _pixels) {
    for (const auto &val : line) {
      if (val > 0) {
        ret += "X";
      } else {
        ret += ".";
      }
    }
    ret += "\n";
  }
  return ret;
}

void Canvas2D::draw_line(Point start, Point end) {
  if (end.x < start.x) {
    std::swap(start, end);
  }
  constexpr bool dolog = false;
  int dX = end.x - start.x;
  int dY = end.y - start.y;
  Point current_point = start;
  // draw start and end point
  draw_pixel(start.x, start.y);
  draw_pixel(end.x, end.y);
  if (dX == 0) {
    if (dY >= 0) {
      for (size_t i = 0; i <= static_cast<size_t>(dY); ++i) {
        draw_pixel(current_point.x, current_point.y);
        current_point.y++;
      }
    } else {
      for (size_t i = 0; i <= static_cast<size_t>(-dY); ++i) {
        draw_pixel(current_point.x, current_point.y);
        current_point.y--;
      }
    }
    return;
  }
  if (dY == 0) {
    if (dX >= 0) {
      if constexpr (dolog)
        std::cout << "x positive\n";
      for (size_t i = 0; i <= static_cast<size_t>(dX); ++i) {
        draw_pixel(current_point.x, current_point.y);
        current_point.x++;
      }
    } else {
      for (size_t i = 0; i <= static_cast<size_t>(-dX); ++i) {
        draw_pixel(current_point.x, current_point.y);
        current_point.x--;
      }
    }
    return;
  }

  const auto gradient = double(dY) / double(dX);
  if constexpr (dolog) {
    std::cout << "gradient = " << gradient << '\n';
    std::cout << "dX=" << dX << "; dY=" << dY << '\n';
  }
  enum class Direction {
    X,
    YUp,
    YDown,
  };
  const auto move_coord = [](int &d, Point &point, int &went,
                             Direction direction) {
    switch (direction) {
    case Direction::X: {
      d--;
      point.x++;
      went++;
      break;
    }
    case Direction::YUp: {
      d--;
      point.y++;
      went++;
      break;
    }
    case Direction::YDown: {
      d++;
      point.y--;
      went--;
      break;
    }
    }
  };
  const auto go_x = [&](double went_x, double went_y) {
    const auto deduce_current_gradient = [=]() {
      if (went_x == 0) {
        if (went_y > 0) {
          return std::numeric_limits<double>::max();
        } else if (went_y == 0) {
          return 0.0;
        } else {
          return -std::numeric_limits<double>::max();
        }
      }
      return went_y / went_x;
    };
    if constexpr (dolog) {
      std::cout << "went_x = " << went_x << "; went_y = " << went_y << "\n";
    }
    const auto current_gradient = deduce_current_gradient();
    if constexpr (dolog) {
      std::cout << "current gradient = " << current_gradient
                << "; gradient = " << gradient << '\n';
    }
    if (gradient >= 0) {
      if (current_gradient > gradient) {
        if constexpr (dolog) {
          std::cout << "go x\n";
        }
        return Direction::X;
      }
      if constexpr (dolog) {
        std::cout << "go y up\n";
      }
      return Direction::YUp;
    } else {
      if (current_gradient < gradient) {
        if constexpr (dolog) {
          std::cout << "go x\n";
        }
        return Direction::X;
      }
      if constexpr (dolog) {
        std::cout << "go y down\n";
      }
      return Direction::YDown;
    }
  };
  int went_x = 0;
  int went_y = 0;
  while (true) {
    if (dX == 0 && dY == 0) {
      if (current_point != end) {
        throw std::runtime_error("end point not hit in draw_line.");
      }
      draw_pixel(current_point.x, current_point.y);
      break;
    }
    if constexpr (dolog) {
      std::cout << "setting point to 1: x=" << current_point.x
                << "; y=" << current_point.y << "; dX=" << dX << "; dY=" << dY
                << '\n';
    }
    draw_pixel(current_point.x, current_point.y);
    const auto direction = go_x(went_x, went_y);
    switch (direction) {
    case Direction::X:
      move_coord(dX, current_point, went_x, direction);
      break;
    case Direction::YUp:
    case Direction::YDown:
      move_coord(dY, current_point, went_y, direction);
      break;
    }
  }
}

void Canvas2D::draw_pixel(int x, int y) {
  if (x < 0 || x >= static_cast<int>(_pixels.size()) || y < 0 ||
      y >= static_cast<int>(_pixels[0].size())) {
    return;
  }
  _pixels[x][y] = 1;
};

} // namespace bubbles