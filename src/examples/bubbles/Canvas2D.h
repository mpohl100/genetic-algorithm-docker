#pragma once

#include <cmath>
#include <set>
#include <string>
#include <vector>

namespace bubbles {

struct Vector;
template <int min_dergrees, int max_degrees> struct RegionedAngle;
using Angle = RegionedAngle<-180, 180>;

struct Point {
  Point() = default;
  Point(const Point &) = default;
  Point &operator=(const Point &) = default;
  Point(Point &&) = default;
  Point &operator=(Point &&) = default;
  Point(int xx, int yy);

  Point plus(const Vector &vec) const;
  std::string toString() const;
  friend constexpr auto operator<=>(const Point &, const Point &) = default;

  int x = 0;
  int y = 0;
};

class Line {
public:
  Line() = default;
  Line(const Line &) = default;
  Line &operator=(const Line &) = default;
  Line(Line &&) = default;
  Line &operator=(Line &&) = default;
  Line(Point start, Point end);

  const Point &start() const;
  const Point &end() const;
  double magnitude() const;

  friend constexpr auto operator<=>(const Line &, const Line &) = default;

private:
  Point _start;
  Point _end;
};

struct Vector {
  Vector() = default;
  Vector(const Vector &) = default;
  Vector &operator=(const Vector &) = default;
  Vector(Vector &&) = default;
  Vector &operator=(Vector &&) = default;
  Vector(double xx, double yy);
  Vector(const Point &start, const Point &end);

  friend constexpr auto operator<=>(const Vector &, const Vector &) = default;

  Vector rotate(const Angle &angle) const;
  Vector scale(double factor) const;
  double magnitude() const;

  double x = 0;
  double y = 0;
};

template <typename Angle>
concept regioned_angle = std::regular<Angle> && requires(Angle a) {
  { a.radians() } -> std::convertible_to<double>;
  { a.degrees() } -> std::convertible_to<double>;
  { Angle::min_degrees_value() } -> std::convertible_to<double>;
  { Angle::max_degrees_value() } -> std::convertible_to<double>;
};

template <int min_degrees, int max_degrees> struct RegionedAngle {
  RegionedAngle() = default;
  RegionedAngle(const RegionedAngle &) = default;
  RegionedAngle &operator=(const RegionedAngle &) = default;
  RegionedAngle(RegionedAngle &&) = default;
  RegionedAngle &operator=(RegionedAngle &&) = default;

  RegionedAngle(const regioned_angle auto &other) {
    _radians = other.radians();
    move_to_range();
  }

  RegionedAngle(double degrees) {
    _radians = degrees / 180.0 * M_PI;
    move_to_range();
  }

  RegionedAngle(const Point &p1, const Point &center, const Point &p2) {
    const auto v1 = Vector{center, p1};
    const auto v2 = Vector{center, p2};
    _radians = radians_from_vectors(v1, v2);
    move_to_range();
  }

  RegionedAngle(const Line &line1, const Line &line2) {
    const auto v1 = Vector{line1.start(), line1.end()};
    const auto v2 = Vector{line2.start(), line2.end()};
    _radians = radians_from_vectors(v1, v2);
    move_to_range();
  }

  double degrees() const {
    const auto degrees = _radians * 180.0 / M_PI;
    return degrees;
  }
  double radians() const { return _radians; }
  double min_degrees_value() const { return min_degrees; }
  double max_degrees_value() const { return max_degrees; }

private:
  double radians_from_vectors(const Vector &v1, const Vector &v2) const {
    const auto dot_product = v1.x * v2.x + v1.y * v2.y;
    const auto magnitude_product = v1.magnitude() * v2.magnitude();
    const auto cross_product = v1.x * v2.y - v1.y * v2.x;
    const auto cos_angle = dot_product / magnitude_product;
    const auto angle = std::acos(cos_angle);
    return cross_product < 0 ? -angle : angle;
  }
  void move_to_range() {
    while (_radians < min_degrees / 180.0 * M_PI) {
      _radians += 2 * M_PI;
    }
    while (_radians > max_degrees / 180.0 * M_PI) {
      _radians -= 2 * M_PI;
    }
  }
  double _radians = 0;
};

class Rectangle {
public:
  Rectangle() = default;
  Rectangle(const Rectangle &) = default;
  Rectangle &operator=(const Rectangle &) = default;
  Rectangle(Rectangle &&) = default;
  Rectangle &operator=(Rectangle &&) = default;
  Rectangle(Point tl, Point br);

  friend auto operator<=>(const Rectangle &, const Rectangle &) = default;

  std::vector<Line> lines() const;
  double area() const;

  std::vector<Line> _lines;
};

class Circle {
public:
  Circle() = default;
  Circle(const Circle &) = default;
  Circle &operator=(const Circle &) = default;
  Circle(Circle &&) = default;
  Circle &operator=(Circle &&) = default;
  Circle(Point center, size_t radius);

  const Point &center() const;
  int radius() const;
  double area() const;
  std::string toString() const;
  friend constexpr auto operator<=>(const Circle &, const Circle &) = default;

private:
  Point _center;
  size_t _radius;
};

class Canvas2D {
public:
  Canvas2D() = default;
  Canvas2D(const Canvas2D &) = default;
  Canvas2D &operator=(const Canvas2D &) = default;
  Canvas2D(Canvas2D &&) = default;
  Canvas2D &operator=(Canvas2D &&) = default;
  Canvas2D(int xx, int yy);

  void draw_rectangle(const Rectangle &rectangle);
  void draw_circle(const Circle &circle);
  std::string getPixels() const;
  void draw_line(const Line &line);

  const std::set<Point> &points() const;

private:
  void draw_pixel(int x, int y);
  [[maybe_unused]] int _x = 1;
  [[maybe_unused]] int _y = 1;
  std::vector<std::vector<int>> _pixels;
  std::set<Point> _points;
};

} // namespace bubbles