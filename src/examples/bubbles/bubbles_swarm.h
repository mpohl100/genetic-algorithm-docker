#include "Canvas2D.h"
#include "Tiles.h"

#include "evol.h"

#include <optional>
#include <set>
#include <vector>

namespace bubbles {

struct AngleArea {
  AngleArea() = default;
  AngleArea(const AngleArea &) = default;
  AngleArea &operator=(const AngleArea &) = default;
  AngleArea(AngleArea &&) = default;
  AngleArea &operator=(AngleArea &&) = default;
  AngleArea(int area, int number_angles);
  friend constexpr auto operator<=>(const AngleArea &,
                                    const AngleArea &) = default;
  bool is_within(const math2d::Angle &angle) const;
  std::string toString() const;
  math2d::Angle get_angle(double factor) const;

private:
  int _area = 0;
  int _nb_angles = 0;
};

struct SourceCircle {
  math2d::Circle circle;
  AngleArea angle_area = {0, 6};
  friend constexpr auto operator<=>(const SourceCircle &,
                                    const SourceCircle &) = default;
};

struct AlreadyOptimized {
public:
  AlreadyOptimized() = default;
  AlreadyOptimized(const AlreadyOptimized &) = default;
  AlreadyOptimized &operator=(const AlreadyOptimized &) = default;
  AlreadyOptimized(AlreadyOptimized &&) = default;
  AlreadyOptimized &operator=(AlreadyOptimized &&) = default;
  AlreadyOptimized(int N, const Canvas2D& canvas) : _tiles{canvas.width(), canvas.height(), N}
  {
  }

  void add_circle(const math2d::Circle &circle);
  void add_tried_circle(const math2d::Circle &circle);
  double area() const;
  math2d::Rectangle bounding_box() const;
  const std::vector<math2d::Circle> &circles() const;

  bool contains(const math2d::Circle &circle) const;

private:
  std::vector<math2d::Circle> _circles;
  std::set<math2d::Circle> _sortedCircles;
  tiles::Tiles<math2d::Circle> _tiles;
};

struct BubbleCircle {
  BubbleCircle() = default;
  BubbleCircle(const BubbleCircle &) = default;
  BubbleCircle &operator=(const BubbleCircle &) = default;
  BubbleCircle(BubbleCircle &&) = default;
  BubbleCircle &operator=(BubbleCircle &&) = default;
  BubbleCircle(const math2d::Circle &circle, const SourceCircle &source_circle);

  friend constexpr auto operator<=>(const BubbleCircle &,
                                    const BubbleCircle &) = default;
  void crossover(const BubbleCircle &other);
  void mutate(evol::Rng &rng,
              const evol::EvolutionCoordinator &evolCoordinator);
  std::string toString() const;
  double magnitude() const;

  const math2d::Circle &circle() const;
  const SourceCircle &source_circle() const;

private:
  bool is_within_angle_of_source_circle() const;
  math2d::number_type get_radius(const math2d::Point &center) const;

  math2d::Circle _circle;
  SourceCircle _source_circle;
};
static_assert(evol::partial::PartialPhenotype<BubbleCircle, evol::Rng>);

class BubblesSwarm
    : public evol::partial::DefaultPartialChallenge<BubbleCircle, evol::Rng> {
public:
  BubblesSwarm() = default;
  BubblesSwarm(const BubblesSwarm &) = default;
  BubblesSwarm &operator=(const BubblesSwarm &) = default;
  BubblesSwarm(BubblesSwarm &&) = default;
  BubblesSwarm &operator=(BubblesSwarm &&) = default;
  BubblesSwarm(const AlreadyOptimized &already_optimized,
               const Canvas2D &canvas);

  double score(const BubbleCircle &bubble_circle, const evol::Rng &rng) const;

private:
  AlreadyOptimized _already_optimized;
  Canvas2D _canvas;
};
static_assert(
    evol::partial::PartialChallenge<BubblesSwarm, BubbleCircle, evol::Rng>);

AlreadyOptimized
bubbles_algorithm(const Canvas2D &canvas, const math2d::Point &point,
                  const evol::partial::PartialEvolutionOptions &params);

AlreadyOptimized
bubbles_algorithm_slow(const Canvas2D &canvas, const math2d::Point &point, tiles::CircleTiles& already_tried);

} // namespace bubbles