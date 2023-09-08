#include "evol.h"

#include <string>

namespace math{

class XCoordinate{
public:
    XCoordinate(double x);
    XCoordinate() = default;
    XCoordinate(const XCoordinate&) = default;
    XCoordinate& operator=(const XCoordinate&) = default;
    XCoordinate(XCoordinate&&) = default;
    XCoordinate& operator=(XCoordinate&&) = default;
    void crossover(const XCoordinate& other);
    void mutate(evol::Rng& rng);
    std::string toString() const;
    double magnitude() const;
    double x() const;
private:
    double _x = 0.0;
};
static_assert(evol::partial::PartialChromosome<XCoordinate, evol::Rng>);

class MathFunction : public evol::partial::DefaultPartialChallenge<XCoordinate, evol::Rng>{
public:
    double score(XCoordinate xCoordinate, evol::Rng& rng) const;
};
static_assert(evol::partial::PartialChallenge<MathFunction, XCoordinate, evol::Rng>);

}