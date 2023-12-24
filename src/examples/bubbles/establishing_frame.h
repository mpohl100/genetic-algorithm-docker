#pragma once

#include "Canvas2D.h"

#include <vector>

namespace math2d {
    class Rectangle;
}

namespace bubbles {

struct Rectangle{
    Rectangle() = default;
    Rectangle(const Rectangle&) = default;
    Rectangle& operator=(const Rectangle&) = default;
    Rectangle(Rectangle&&) = default;
    Rectangle& operator=(Rectangle&&) = default;
    Rectangle(const math2d::Rectangle& rectangle);

    math2d::Rectangle to_math2d_rectangle() const;

    int x = 0;
    int y = 0;
    int width = 1;
    int height = 1;
};

struct AllRectangles{
    std::vector<Rectangle> rectangles;
};

AllRectangles establishing_shot(const Canvas2D& canvas);

}