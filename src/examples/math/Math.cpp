#include "Math.h"

#include <cmath>

namespace math {

XCoordinate::XCoordinate(double x) : _x(x) {}

void XCoordinate::crossover(const XCoordinate &other) {
  _x = (_x + other.x()) / 2.0;
}

void XCoordinate::mutate(evol::Rng &rng,
                         const evol::EvolutionCoordinator &evolCoordinator) {
  const auto progress = evolCoordinator.progress();
  const int exponent = int(100 * (1 - progress)) / 10;
  const int baseNumber = std::pow(10, exponent);
  const auto randomNumber = rng.fetchUniform(-baseNumber, baseNumber, 1).top();
  _x += randomNumber / 10000.0;
}

std::string XCoordinate::toString() const { return "x: " + std::to_string(_x); }

double XCoordinate::magnitude() const {
  // no need to first take the power of two and then the square root
  return std::abs(_x);
}

double XCoordinate::x() const { return _x; }

double MathFunctionAdjust::score(XCoordinate xCoordinate,
                                 [[maybe_unused]] evol::Rng &rng) const {
  return 1.0 / std::pow(xCoordinate.x() - 2, 2.0);
}

double MathFunctionPartial::score(XCoordinate xCoordinate,
                                  [[maybe_unused]] evol::Rng &rng) const {
  return 1.0 / std::pow(xCoordinate.x() - 2, 2.0);
}

double MathFunction::score(XCoordinate xCoordinate,
                           [[maybe_unused]] evol::Rng &rng) const {
  return 1.0 / std::pow(xCoordinate.x() - 2, 2.0);
}

} // namespace math