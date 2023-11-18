#include "Canvas2D.h"

#include "evol.h"

#include <optional>
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
  bool is_within(const Angle &angle) const;
  std::string toString() const;
  Angle get_angle(double factor) const;

private:
  int _area = 0;
  int _nb_angles = 0;
};

struct SourceCircle {
  Circle circle;
  AngleArea angle_area = {0, 6};
  friend constexpr auto operator<=>(const SourceCircle &,
                                    const SourceCircle &) = default;
};

struct AlreadyOptimized {
public:
  void add_circle(const Circle &circle);
  double area() const;
  const std::vector<Circle> &circles() const;

private:
  std::vector<Circle> _circles;
};

struct BubbleCircle {
  BubbleCircle() = default;
  BubbleCircle(const BubbleCircle &) = default;
  BubbleCircle &operator=(const BubbleCircle &) = default;
  BubbleCircle(BubbleCircle &&) = default;
  BubbleCircle &operator=(BubbleCircle &&) = default;
  BubbleCircle(const Circle &circle, const SourceCircle &source_circle);

  friend constexpr auto operator<=>(const BubbleCircle &,
                                    const BubbleCircle &) = default;
  void crossover(const BubbleCircle &other);
  void mutate(evol::Rng &rng,
              const evol::EvolutionCoordinator &evolCoordinator);
  std::string toString() const;
  double magnitude() const;

  const Circle &circle() const;

private:
  bool is_within_angle_of_source_circle() const;
  double get_radius(const Point &center) const;

  Circle _circle;
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
bubbles_algorithm(const Canvas2D &canvas, const Point &point,
                  const evol::partial::PartialEvolutionOptions &params);

} // namespace bubbles