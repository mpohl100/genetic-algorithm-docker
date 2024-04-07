#include "evol.h"

#include <string>

namespace math {

class XCoordinate {
public:
  XCoordinate(double x);
  XCoordinate() = default;
  XCoordinate(const XCoordinate &) = default;
  XCoordinate &operator=(const XCoordinate &) = default;
  XCoordinate(XCoordinate &&) = default;
  XCoordinate &operator=(XCoordinate &&) = default;
  void crossover(const XCoordinate &other);
  void mutate(evol::Rng &rng,
              const evol::EvolutionCoordinator &evolCoordinator);
  std::string toString() const;
  double magnitude() const;
  double x() const;

private:
  double _x = 0.0;
};
#if EVOL_USE_CONCEPTS == 1
static_assert(evol::partial::PartialPhenotype<XCoordinate, evol::Rng>);
#endif

class MathFunctionAdjust
    : public evol::adjust::DefaultAdjustChallenge<XCoordinate, evol::Rng> {
public:
  double score(XCoordinate xCoordinate, evol::Rng &rng) const;
};
#if EVOL_USE_CONCEPTS == 1
static_assert(evol::adjust::AdjustChallenge<MathFunctionAdjust, XCoordinate,
                                              evol::Rng>);
#endif

class MathFunctionPartial
    : public evol::partial::DefaultPartialChallenge<XCoordinate, evol::Rng> {
public:
  double score(XCoordinate xCoordinate, evol::Rng &rng) const;
};
#if EVOL_USE_CONCEPTS == 1
static_assert(evol::partial::PartialChallenge<MathFunctionPartial, XCoordinate,
                                              evol::Rng>);
#endif

class MathFunction : public evol::DefaultChallenge<XCoordinate, evol::Rng> {
public:
  double score(XCoordinate xCoordinate, evol::Rng &rng) const;
};
#if EVOL_USE_CONCEPTS == 1
static_assert(evol::Challenge<MathFunction, XCoordinate, evol::Rng>);
#endif

} // namespace math