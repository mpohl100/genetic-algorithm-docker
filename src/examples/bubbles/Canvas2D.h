#pragma once

#include <string>
#include <vector>

namespace bubbles{

struct Point{
    Point() = default;
    Point(const Point&) = default;
    Point& operator=(const Point&) = default;
    Point(Point&&) = default;
    Point& operator=(Point&&) = default;
    Point(int xx, int yy);

    friend constexpr auto operator<=>(const Point&, const Point&) = default;

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
    void draw_line(Point start, Point end);
private:
    [[maybe_unused]] int _x = 1;
    [[maybe_unused]] int _y = 1;
    std::vector<std::vector<int>> _pixels;
};

}