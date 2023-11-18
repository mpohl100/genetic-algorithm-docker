#pragma once

#include <set>
#include <string>
#include <vector>

namespace bubbles {

struct Vector;

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

struct Angle {
  Angle() = default;
  Angle(const Angle &) = default;
  Angle &operator=(const Angle &) = default;
  Angle(Angle &&) = default;
  Angle &operator=(Angle &&) = default;
  Angle(double degrees);
  Angle(const Point &p1, const Point &center, const Point &p2);
  Angle(const Line &line1, const Line &line2);

  double degrees() const;
  double radians() const;

private:
  double radians_from_vectors(const Vector &v1, const Vector &v2) const;
  double _radians = 0;
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
  Circle(Point center, int radius);

  const Point &center() const;
  int radius() const;
  double area() const;
  std::string toString() const;
  friend constexpr auto operator<=>(const Circle &, const Circle &) = default;

private:
  Point _center;
  int _radius;
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