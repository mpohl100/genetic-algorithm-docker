#include "bubbles_swarm.h"

namespace bubbles {

AngleArea::AngleArea(int area, int number_angles)
    : area{area}, nb_angles{number_angles} {}

BubbleCircle::BubbleCircle(const Circle &circle,
                           const SourceCircle &source_circle)
    : _circle{circle}, _source_circle{source_circle} {}

void BubbleCircle::crossover([[maybe_unused]] const BubbleCircle &other) {}
void BubbleCircle::mutate(
    [[maybe_unused]] evol::Rng &rng,
    [[maybe_unused]] const evol::EvolutionCoordinator &evolCoordinator) {}

std::string BubbleCircle::toString() const { return ""; }

double BubbleCircle::magnitude() const { return 0.0; }

BubblesSwarm bubbles_algorithm([[maybe_unused]] const Canvas2D &canvas,
                               [[maybe_unused]] const Point &point) {
  return BubblesSwarm{};
}

BubblesSwarm::BubblesSwarm(const AlreadyOptimized &already_optimized,
                           const Canvas2D &canvas)
    : _already_optimized{already_optimized}, _canvas{canvas} {}

double BubblesSwarm::area() const { return 0.0; }

double BubblesSwarm::score([[maybe_unused]] const BubbleCircle &bubble_circle,
                           [[maybe_unused]] const evol::Rng &rng) const {
  return 0.0;
}

} // namespace bubbles