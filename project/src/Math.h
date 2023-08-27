#include "evol/EvolutionConcepts.h"

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
    void mutate();
    std::string toString() const;
    double x() const;
private:
    double _x = 0.0;
};
static_assert(evol::Chromosome<XCoordinate>);

class MathFunction{
public:
    double score(XCoordinate xCoordinate) const;
};
static_assert(evol::Challenge<MathFunction, XCoordinate>);

}