#include "Canvas2D.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

using namespace math2d;

namespace bubbles {

Canvas2D::Canvas2D(int xx, int yy, int N)
    : _x(xx), _y(yy), _N(N), _pixels(xx, yy), _points(), _tiles{xx, yy, N} {}

void Canvas2D::draw_rectangle(const Rectangle &rectangle) {
  draw_line(rectangle.lines()[0]);
  draw_line(rectangle.lines()[1]);
  draw_line(rectangle.lines()[2]);
  draw_line(rectangle.lines()[3]);
}

class Pattern {
  int x, y;
  std::vector<char> pattern;

public:
  Pattern(int _x, int _y, const std::vector<char> &_pattern)
      : x(_x), y(_y), pattern(_pattern) {}
  Pattern() = default;
  Pattern(const Pattern &) = default;
  Pattern &operator=(const Pattern &) = default;
  Pattern(Pattern &&) = default;
  Pattern &operator=(Pattern &&) = default;

  int getX() const { return x; }

  int getY() const { return y; }

  const std::vector<char> &getPattern() const { return pattern; }

  static Pattern newPattern(int x, int y, const std::vector<char> &pattern) {
    return Pattern(x, y, pattern);
  }
};

void Canvas2D::draw_circle(const Circle &circle) {
  const Point circleCenter = circle.center();
  const auto circleRadius = circle.radius();
  constexpr double offset = 5.0;
  int start_x = static_cast<int>(circleCenter.x - circleRadius - offset);
  int end_x = static_cast<int>(circleCenter.x + circleRadius + offset);
  int start_y = static_cast<int>(circleCenter.y - circleRadius - offset);
  int end_y = static_cast<int>(circleCenter.y + circleRadius + offset);

  std::vector<std::vector<Pattern>> allPatterns;

  for (int y = start_y; y < end_y; ++y) {
    std::vector<Pattern> linePatterns;

    for (int x = start_x; x < end_x; ++x) {
      Pattern previousPattern;

      if (linePatterns.empty()) {
        previousPattern = Pattern::newPattern(x - 1, y, {'.'});
      } else {
        previousPattern = linePatterns.back();
      }

      const auto currentPoint = Point{static_cast<math2d::number_type>(x),
                                      static_cast<math2d::number_type>(y)};
      float distance = Vector{circleCenter.x - currentPoint.x,
                              circleCenter.y - currentPoint.y}
                           .magnitude();

      if (distance <= circleRadius) {
        linePatterns.push_back(Pattern::newPattern(
            x, y, {previousPattern.getPattern().back(), 'X'}));
      } else {
        linePatterns.push_back(Pattern::newPattern(
            x, y, {previousPattern.getPattern().back(), '.'}));
      }
    }

    if (!linePatterns.empty()) {
      allPatterns.push_back(linePatterns);
    }
  }

  std::vector<Pattern> previousPattern;

  for (const auto &linePatterns : allPatterns) {
    auto startPattern = std::find_if(
        linePatterns.begin(), linePatterns.end(), [](const Pattern &pattern) {
          return pattern.getPattern() == std::vector<char>{'.', 'X'};
        });

    if (startPattern == linePatterns.end()) {
      continue;
    }

    auto endPattern = std::find_if(
        linePatterns.begin(), linePatterns.end(), [](const Pattern &pattern) {
          return pattern.getPattern() == std::vector<char>{'X', '.'};
        });

    if (endPattern == linePatterns.end()) {
      continue;
    }

    bool previousPatternFound = true;

    auto previousStartPattern = std::find_if(
        previousPattern.begin(), previousPattern.end(),
        [](const Pattern &pattern) {
          return pattern.getPattern() == std::vector<char>{'.', 'X'};
        });

    if (previousStartPattern == previousPattern.end()) {
      previousPatternFound = false;
    }

    auto previousEndPattern = std::find_if(
        previousPattern.begin(), previousPattern.end(),
        [](const Pattern &pattern) {
          return pattern.getPattern() == std::vector<char>{'X', '.'};
        });

    if (previousEndPattern == previousPattern.end()) {
      previousPatternFound = false;
    }

    if (!previousPatternFound) {
      // First line
      for (int x = startPattern->getX(); x < endPattern->getX(); ++x) {
        draw_pixel(x, linePatterns[0].getY(), 2);
      }
    } else {
      int previousStartX = previousStartPattern->getX();
      int currentStartX = startPattern->getX();
      int previousEndX = previousEndPattern->getX();
      int currentEndX = endPattern->getX();

      if (previousStartX > currentStartX) {
        std::swap(previousStartX, currentStartX);
      }

      if (previousEndX > currentEndX) {
        std::swap(previousEndX, currentEndX);
      }

      for (int x = previousStartX; x < currentStartX + 1; ++x) {
        draw_pixel(x, linePatterns[0].getY(), 2);
      }

      for (int x = previousEndX; x < currentEndX + 1; ++x) {
        draw_pixel(x, linePatterns[0].getY(), 2);
      }
    }

    previousPattern = linePatterns;
  }
}

std::string Canvas2D::getPixels() const {
  std::string ret;
  for (size_t x = 0; x < _pixels.width(); ++x) {
    for (size_t y = 0; y < _pixels.height(); ++y) {
      const auto val = _pixels.get(x, y);
      if (val == 2) {
        ret += "O";
      } else if (val == 1) {
        ret += "X";
      } else {
        ret += ".";
      }
    }
    ret += "\n";
  }
  return ret;
}

bool Canvas2D::is_within(const Point &point) const {
  return point.x >= 0 && point.x < _x && point.y >= 0 && point.y < _y;
}

void Canvas2D::draw_line(const Line &line) {
  auto start = line.start();
  auto end = line.end();
  if (end.x < start.x) {
    std::swap(start, end);
  }
  constexpr bool dolog = false;
  int dX = end.x - start.x;
  int dY = end.y - start.y;
  Point current_point = start;
  // draw start and end point
  draw_pixel(start.x, start.y, 1);
  draw_pixel(end.x, end.y, 1);
  if (dX == 0) {
    if (dY >= 0) {
      for (size_t i = 0; i <= static_cast<size_t>(dY); ++i) {
        draw_pixel(current_point.x, current_point.y, 1);
        current_point.y++;
      }
    } else {
      for (size_t i = 0; i <= static_cast<size_t>(-dY); ++i) {
        draw_pixel(current_point.x, current_point.y, 1);
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
        draw_pixel(current_point.x, current_point.y, 1);
        current_point.x++;
      }
    } else {
      for (size_t i = 0; i <= static_cast<size_t>(-dX); ++i) {
        draw_pixel(current_point.x, current_point.y, 1);
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
      draw_pixel(current_point.x, current_point.y, 1);
      break;
    }
    if constexpr (dolog) {
      std::cout << "setting point to 1: x=" << current_point.x
                << "; y=" << current_point.y << "; dX=" << dX << "; dY=" << dY
                << '\n';
    }
    draw_pixel(current_point.x, current_point.y, 1);
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

const std::set<Point> &Canvas2D::points() const { return _points; }

const tiles::Tiles<math2d::Point> &Canvas2D::tiles() const { return _tiles; }

int Canvas2D::width() const { return _x; }

int Canvas2D::height() const { return _y; }

int Canvas2D::N() const { return _N; }

void Canvas2D::draw_pixel(int x, int y, int value) {
  if (x < 0 || x >= static_cast<int>(_pixels.width()) || y < 0 ||
      y >= static_cast<int>(_pixels.height())) {
    return;
  }
  _pixels.get(x, y) = value;
  _points.emplace(x, y);
  _tiles.addType(Point(x, y));
};

int Canvas2D::pixel(int x, int y) const { return _pixels.get(x, y); }

} // namespace bubbles