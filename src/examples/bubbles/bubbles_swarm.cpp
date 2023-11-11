#include "bubbles_swarm.h"

#include <queue>
#include <set>

namespace bubbles {

AngleArea::AngleArea(int area, int number_angles)
    : area{area}, nb_angles{number_angles} {}

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

bool BubbleCircle::is_within_angle_of_source_circle() const {
  [[maybe_unused]] const auto thales_circle = Circle{
      get_mid_point(_circle.center(), _source_circle.circle.center()),
      static_cast<int>(Vector{_circle.center(), _source_circle.circle.center()}
                           .scale(0.5)
                           .magnitude())};
  const auto intersection_points = std::vector<Point>{};
  return false;
}

Circle calculate_next_circle(const SourceCircle &source_circle,
                             const AlreadyOptimized &already_optimized,
                             const Canvas2D &canvas) {
  const auto bubble_swarm = BubblesSwarm{already_optimized, canvas};
  auto evolParam = evol::partial::PartialEvolutionOptions{};
  evolParam.num_generations = 100;
  evolParam.log_level = 0;
  evolParam.num_parents = 2;
  evolParam.num_children = 20;
  evolParam.min_magnitude = 0.9;
  evolParam.max_magnitude = 1.1;
  evolParam.out = &std::cout;
  auto rng = evol::Rng{};

  const auto result = evol::partial::evolution(
      BubbleCircle{source_circle.circle, source_circle}, bubble_swarm,
      evolParam, rng);
  return result.winner.circle();
}

std::vector<SourceCircle> deduce_next_sourve_circles(const Circle &circle) {
  std::vector<SourceCircle> source_circles;
  int nb_angles = 6;
  for (int i = 0; i < nb_angles; ++i) {
    source_circles.emplace_back(SourceCircle{circle, AngleArea{i, nb_angles}});
  }
  return source_circles;
}

AlreadyOptimized bubbles_algorithm(const Canvas2D &canvas, const Point &point) {
  auto already_optimized = AlreadyOptimized{};
  auto queue = std::queue<SourceCircle>{};
  queue.emplace(SourceCircle{Circle{point, 1}, AngleArea{0, 6}});
  auto already_calculated = std::set<SourceCircle>{};
  while (!queue.empty()) {
    const auto next_circle =
        calculate_next_circle(queue.front(), already_optimized, canvas);
    already_optimized.add_circle(next_circle);

    const auto next_source_circles = deduce_next_sourve_circles(next_circle);
    for (const auto &next_source_circle : next_source_circles) {
      if (already_calculated.find(next_source_circle) ==
          already_calculated.end()) {
        queue.emplace(next_source_circle);
      }
    }

    already_calculated.insert(queue.front());
    queue.pop();
  }
  return already_optimized;
}

BubblesSwarm::BubblesSwarm(const AlreadyOptimized &already_optimized,
                           const Canvas2D &canvas)
    : _already_optimized{already_optimized}, _canvas{canvas} {}

double BubblesSwarm::score([[maybe_unused]] const BubbleCircle &bubble_circle,
                           [[maybe_unused]] const evol::Rng &rng) const {
  return 0.0;
}

} // namespace bubbles