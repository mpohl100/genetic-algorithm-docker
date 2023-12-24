#pragma once

#include "Canvas2D.h"

#include <vector>

namespace bubbles {

struct Rectangle{
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