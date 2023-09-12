#pragma once

#include <string>

namespace path{

struct Point{
    Point() = default;
    Point(const Point&) = default;
    Point& operator=(const Point&) = default;
    Point(Point&&) = default;
    Point& operator=(Point&&) = default;
    Point(int xx, int yy);
    int x = 0;
    int y = 0;
};

class Canvas2D{
public:
    Canvas2D() = default;
    Canvas2D(const Canvas2D&) = default;
    Canvas2D& operator=(const Canvas2D&) = default;
    Canvas2D(Canvas2D&&) = default;
    Canvas2D& operator=(Canvas2D&&) = default;
    Canvas2D(int xx, int yy);

    void draw_rectangle(const Point& tl, const Point& br);
    std::string getPixels() const; 
private:
    [[maybe_unused]] int _x = 1;
    [[maybe_unused]] int _y = 1;
};

}