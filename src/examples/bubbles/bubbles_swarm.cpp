#include "bubbles_swarm.h"

#include <queue>
#include <set>

namespace bubbles {

AngleArea::AngleArea(int area, int number_angles)
    : _area{area}, _nb_angles{number_angles} {}

bool AngleArea::is_within(const Angle &angle) const {
  const auto angle_1 = 360.0 / _nb_angles * _area;
  const auto angle_2 = 360.0 / _nb_angles * (_area + 1);
  return angle.degrees() >= angle_1 && angle.degrees() < angle_2;
}

Angle AngleArea::get_angle(double factor) const {
  double segment_angle = 360.0 / _nb_angles;
  double degrees = segment_angle * (static_cast<double>(_area) * factor);
  return Angle{degrees};
}

void AlreadyOptimized::add_circle(const Circle &circle) {
  _circles.push_back(circle);
}

double AlreadyOptimized::area() const {
  auto area = 0.0;
  for (const auto &circle : _circles) {
    area += circle.area();
  }
  return area;
}

const std::vector<Circle> &AlreadyOptimized::circles() const {
  return _circles;
}

BubbleCircle::BubbleCircle(const Circle &circle,
                           const SourceCircle &source_circle)
    : _circle{circle}, _source_circle{source_circle} {}

void BubbleCircle::crossover(const BubbleCircle &other) {
  _circle = Circle{Point{(_circle.center().x + other.circle().center().x) / 2,
                         (_circle.center().y + other.circle().center().y) / 2},
                   (_circle.radius() + other.circle().radius()) / 2};
}

void BubbleCircle::mutate(
    evol::Rng &rng,
    [[maybe_unused]] const evol::EvolutionCoordinator &evolCoordinator) {
  const auto random_number = rng.fetchUniform(0, 2, 1).top();
  // try out different valus here
  const auto random_mutation_value =
      static_cast<int>(rng.fetchNormal(0, 2, 1).top());
  switch (random_number) {
  case 0:
    _circle = Circle{
        Point{_circle.center().x + random_mutation_value, _circle.center().y},
        _circle.radius()};
    break;
  case 1:
    _circle = Circle{
        Point{_circle.center().x, _circle.center().y + random_mutation_value},
        _circle.radius()};
    break;
  case 2:
    _circle =
        Circle{_circle.center(), _circle.radius() + random_mutation_value};
    break;
  }
}

std::string BubbleCircle::toString() const {
  return "center {x: " + std::to_string(_circle.center().x) +
         " y: " + std::to_string(_circle.center().y) +
         "}, radius: " + std::to_string(_circle.radius());
}

double BubbleCircle::magnitude() const {
  return is_within_angle_of_source_circle() ? 1.0 : 0.0;
}

const Circle &BubbleCircle::circle() const { return _circle; }

Point get_mid_point(const Point &from, const Point &to) {
  return from.plus(Vector{from, to}.scale(0.5));
}

std::vector<Point> calculate_circle_intersection(const Circle &first,
                                                 const Circle &second) {
  std::vector<Point> intersectionPoints;

  // Calculate the distance between the centers of the circles
  double dist = Vector{first.center(), second.center()}.magnitude();

  // Check if the circles are completely separate
  if (dist > first.radius() + second.radius()) {
    // No intersection, return empty vector
    return intersectionPoints;
  }

  // Check if one circle is entirely inside the other
  if (dist < std::abs(first.radius() - second.radius())) {
    // No intersection, return empty vector
    return intersectionPoints;
  }

  // Calculate the intersection points using trigonometry
  double a = (std::pow(first.radius(), 2) - std::pow(second.radius(), 2) +
              std::pow(dist, 2)) /
             (2 * dist);
  double h = std::sqrt(std::pow(first.radius(), 2) - std::pow(a, 2));

  // Calculate the coordinates of the intersection points
  double x2 =
      first.center().x + a * (second.center().x - first.center().x) / dist;
  double y2 =
      first.center().y + a * (second.center().y - first.center().y) / dist;

  double intersectionX1 =
      x2 + h * (second.center().y - first.center().y) / dist;
  double intersectionY1 =
      y2 - h * (second.center().x - first.center().x) / dist;
  double intersectionX2 =
      x2 - h * (second.center().y - first.center().y) / dist;
  double intersectionY2 =
      y2 + h * (second.center().x - first.center().x) / dist;

  // Add the intersection points to the vector
  intersectionPoints.emplace_back(intersectionX1, intersectionY1);
  intersectionPoints.emplace_back(intersectionX2, intersectionY2);

  return intersectionPoints;
}

bool BubbleCircle::is_within_angle_of_source_circle() const {
  const auto thales_circle = Circle{
      get_mid_point(_circle.center(), _source_circle.circle.center()),
      static_cast<int>(Vector{_circle.center(), _source_circle.circle.center()}
                           .scale(0.5)
                           .magnitude())};
  const auto intersection_points =
      calculate_circle_intersection(_circle, thales_circle);
  if (intersection_points.size() < 2) {
    const auto angle = Angle{_circle.center().plus(Vector{
                                 static_cast<double>(_circle.radius()), 0.0}),
                             _circle.center(), _source_circle.circle.center()};
    return _source_circle.angle_area.is_within(angle) ? 1.0 : 0.0;
  }
  const auto angle_0 = Angle{
      _circle.center().plus(Vector{static_cast<double>(_circle.radius()), 0.0}),
      _circle.center(), intersection_points[0]};
  const auto angle_1 = Angle{
      _circle.center().plus(Vector{static_cast<double>(_circle.radius()), 0.0}),
      _circle.center(), intersection_points[1]};
  return (_source_circle.angle_area.is_within(angle_0) &&
          _source_circle.angle_area.is_within(angle_1))
             ? 1.0
             : 0.0;
}

Circle calculate_first_guess(const SourceCircle &sourceCircle) {
  const auto angle = sourceCircle.angle_area.get_angle(0.5);
  return Circle{
      sourceCircle.circle.center().plus(
          Vector{static_cast<double>(sourceCircle.circle.radius()) * 2.0, 0.0}
              .rotate(angle)),
      sourceCircle.circle.radius()};
}

std::optional<Circle>
calculate_next_circle(const SourceCircle &source_circle,
                      const AlreadyOptimized &already_optimized,
                      const Canvas2D &canvas,
                      const evol::partial::PartialEvolutionOptions &params) {
  const auto bubble_swarm = BubblesSwarm{already_optimized, canvas};
  auto rng = evol::Rng{};

  const auto result = evol::partial::evolution(
      BubbleCircle{calculate_first_guess(source_circle), source_circle},
      bubble_swarm, params, rng);
  if (result.fitness >= 0.0) {
    return result.winner.circle();
  }
  return std::nullopt;
}

std::vector<SourceCircle> deduce_next_sourve_circles(const Circle &circle) {
  std::vector<SourceCircle> source_circles;
  int nb_angles = 6;
  for (int i = 0; i < nb_angles; ++i) {
    source_circles.emplace_back(SourceCircle{circle, AngleArea{i, nb_angles}});
  }
  return source_circles;
}

AlreadyOptimized
bubbles_algorithm(const Canvas2D &canvas, const Point &point,
                  const evol::partial::PartialEvolutionOptions &params) {
  auto already_optimized = AlreadyOptimized{};
  auto queue = std::queue<SourceCircle>{};
  queue.emplace(SourceCircle{Circle{point, 1}, AngleArea{0, 6}});
  auto already_calculated = std::set<SourceCircle>{};
  while (!queue.empty()) {
    const auto next_circle =
        calculate_next_circle(queue.front(), already_optimized, canvas, params);
    if (next_circle) {
      already_optimized.add_circle(*next_circle);
#if 0
    const auto next_source_circles = deduce_next_sourve_circles(next_circle);
    for (const auto &next_source_circle : next_source_circles) {
      if (already_calculated.find(next_source_circle) ==
          already_calculated.end()) {
        queue.emplace(next_source_circle);
      }
    }

    already_calculated.insert(queue.front());
#endif
    }
    queue.pop();
  }
  return already_optimized;
}

BubblesSwarm::BubblesSwarm(const AlreadyOptimized &already_optimized,
                           const Canvas2D &canvas)
    : _already_optimized{already_optimized}, _canvas{canvas} {}

double BubblesSwarm::score(const BubbleCircle &bubble_circle,
                           [[maybe_unused]] const evol::Rng &rng) const {
  auto fitness = 0.0;
  // punish overlapping circles
  for (const auto &circle : _already_optimized.circles()) {
    const auto distance =
        Vector{bubble_circle.circle().center(), circle.center()}.magnitude();
    const auto radius_sum =
        static_cast<double>(bubble_circle.circle().radius() + circle.radius());
    if (distance > radius_sum) {
      break; // no reward for circles that don't intersect
    }
    fitness -= std::pow(radius_sum / distance, 2.0);
  }
  // punish filled pixels within the circle
  for (const auto &point : _canvas.points()) {
    const auto distance =
        Vector{bubble_circle.circle().center(), point}.magnitude();
    if (distance < bubble_circle.circle().radius()) {
      // containing a point within the circle is 20 times worse than overlapping
      // a different circle
      fitness -= 20 * std::pow(bubble_circle.circle().radius() / distance, 2.0);
    }
  }
  fitness += bubble_circle.circle().area();
  return fitness;
}

} // namespace bubbles