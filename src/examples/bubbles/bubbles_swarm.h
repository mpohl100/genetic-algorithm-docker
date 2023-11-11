#include "Canvas2D.h"

#include "evol.h"

#include <optional>
#include <vector>

namespace bubbles {

class BubblesSwarm {
public:
  struct AlreadyOptimized {
    std::vector<Circle> circles;
    struct SourceCirle {
      size_t index = 0;
      int quadrant = 0;
      friend constexpr auto operator<=>(const SourceCirle &,
                                        const SourceCirle &) = default;
    };
    std::optional<SourceCirle> source_circle = std::nullopt;
  };

  BubblesSwarm() = default;
  BubblesSwarm(const BubblesSwarm &) = default;
  BubblesSwarm &operator=(const BubblesSwarm &) = default;
  BubblesSwarm(BubblesSwarm &&) = default;
  BubblesSwarm &operator=(BubblesSwarm &&) = default;
  BubblesSwarm(const AlreadyOptimized &already_optimized);
  double area() const;

private:
  Circle _circle;
  AlreadyOptimized _already_optimized;
};

BubblesSwarm bubbles_algorithm(const Canvas2D &canvas, const Point &point);

} // namespace bubbles