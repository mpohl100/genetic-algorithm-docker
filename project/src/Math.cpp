#include "Math.h"

#include "evol/Rng.h"

#include <cmath>

namespace math{

XCoordinate::XCoordinate(double x)
    : _x(x)
{}

void XCoordinate::crossover(const XCoordinate& other)
{
    _x = (_x + other.x()) / 2.0;
}

void XCoordinate::mutate()
{
    static thread_local auto rng = evol::Rng{};
    const auto randomNumber = rng.fetchUniform(-10, 10, 1).top();
    _x += randomNumber / 10.0;
}

std::string XCoordinate::toString() const
{
    return "x: " + std::to_string(_x);
}

double XCoordinate::magnitude() const
{
    // no need to first take the power of two and then the square root
    return std::abs(_x);
}


double XCoordinate::x() const
{
    return _x; 
}

double MathFunction::score(XCoordinate xCoordinate) const
{
    return 1.0 / std::pow(xCoordinate.x() - 2, 2.0);
}

}