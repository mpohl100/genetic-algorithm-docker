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

void BubbleCircle::crossover([[maybe_unused]] const BubbleCircle &other) {}
void BubbleCircle::mutate(
    [[maybe_unused]] evol::Rng &rng,
    [[maybe_unused]] const evol::EvolutionCoordinator &evolCoordinator) {}

std::string BubbleCircle::toString() const { return ""; }

double BubbleCircle::magnitude() const { return 0.0; }

const Circle &BubbleCircle::circle() const { return _circle; }

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