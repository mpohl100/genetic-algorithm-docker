#include "math2d.h"

#include <cmath>
#include <string>

namespace math2d{
    
Vector::Vector(double xx, double yy) : x(xx), y(yy) {}

Vector::Vector(const Point &start, const Point &end)
    : x(end.x - start.x), y(end.y - start.y) {}

Vector Vector::rotate(const Angle &angle) const {
  const auto radians = angle.radians();
  const auto cos_angle = std::cos(radians);
  const auto sin_angle = std::sin(radians);
  const auto dx = 0;
  const auto dy = 0;
  const auto x_rotated = ((x - dx) * cos_angle) - ((dy - y) * sin_angle) + dx;
  const auto y_rotated = dy - ((dy - y) * cos_angle) + ((x - dx) * sin_angle);
  return Vector{x_rotated, y_rotated};
}

Vector Vector::scale(double factor) const {
  return Vector{x * factor, y * factor};
}

double Vector::magnitude() const {
  return std::sqrt(std::pow(x, 2) + std::pow(y, 2));
}

Point Point::plus(const Vector &vec) const {
  return Point{x + vec.x, y + vec.y};
}

std::string Point::toString() const {
  return "Point{x: " + std::to_string(x) + "; y: " + std::to_string(y) + "}";
}

Line::Line(Point start, Point end) : _start(start), _end(end) {}

const Point &Line::start() const { return _start; }

const Point &Line::end() const { return _end; }

double Line::magnitude() const {
  return std::sqrt(std::pow(_end.x - _start.x, 2) +
                   std::pow(_end.y - _start.y, 2));
}

Rectangle::Rectangle(Point tl, Point br) : _lines() {
  _lines.emplace_back(tl, Point(br.x, tl.y));
  _lines.emplace_back(Point(br.x, tl.y), br);
  _lines.emplace_back(br, Point(tl.x, br.y));
  _lines.emplace_back(Point(tl.x, br.y), tl);
}

std::vector<Line> Rectangle::lines() const { return _lines; }

double Rectangle::area() const {
  return _lines[0].magnitude() * _lines[1].magnitude();
}

Circle::Circle(Point center, number_type radius)
    : _center(center), _radius(radius) {}

const Point &Circle::center() const { return _center; }

number_type Circle::radius() const { return _radius; }

double Circle::area() const { return M_PI * std::pow(_radius, 2); }

std::string Circle::toString() const {
  return "Circle{center: " + _center.toString() +
         "; radius: " + std::to_string(_radius) + "}";
}

}