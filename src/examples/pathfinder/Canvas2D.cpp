#include "Canvas2D.h"

namespace path{

Point::Point(int xx, int yy)
: x(xx)
, y(yy)
{}

Canvas2D::Canvas2D(int xx, int yy)
: _x(xx)
, _y(yy)
{}

void Canvas2D::draw_rectangle([[maybe_unused]] const Point& tl, [[maybe_unused]] const Point& br)
{

}

std::string Canvas2D::getPixels() const
{
    return "";
}

}