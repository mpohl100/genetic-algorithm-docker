#pragma once

#include <string>
#include <vector>

namespace bubbles {

struct Point {
  Point() = default;
  Point(const Point &) = default;
  Point &operator=(const Point &) = default;
  Point(Point &&) = default;
  Point &operator=(Point &&) = default;
  Point(int xx, int yy);

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

  friend constexpr auto operator<=>(const Line &, const Line &) = default;
private:
  Point _start;
  Point _end;
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

  friend auto operator<=>(const Circle &, const Circle &) = default;

  std::vector<Point> points() const;
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

  void draw_rectangle(const Point &tl, const Point &br);
  void draw_circle(const Point &m, int radius);
  std::string getPixels() const;
  void draw_line(Point start, Point end);

private:
  void draw_pixel(int x, int y);
  [[maybe_unused]] int _x = 1;
  [[maybe_unused]] int _y = 1;
  std::vector<std::vector<int>> _pixels;
};

} // namespace bubbles