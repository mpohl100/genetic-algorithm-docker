#include "bubbles_swarm.h"

namespace bubbles {

BubblesSwarm bubbles_algorithm([[maybe_unused]] const Canvas2D &canvas,
                               [[maybe_unused]] const Point &point) {
  return BubblesSwarm{};
}

BubblesSwarm::BubblesSwarm(const AlreadyOptimized &already_optimized)
    : _circle{}, _already_optimized{already_optimized} {}

double BubblesSwarm::area() const { return 0.0; }

} // namespace bubbles