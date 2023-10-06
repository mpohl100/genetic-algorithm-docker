#include <catch2/catch_all.hpp>

#include "examples/pathfinder/Canvas2D.h"

namespace {

TEST_CASE("Canvas", "[canvas]"){
    SECTION("Canvas_Line"){
        auto canvas = path::Canvas2D(10,10);
        canvas.draw_line(path::Point(1,1), path::Point(7,3));
        const auto canvas_pixels = canvas.getPixels();
        CHECK(canvas_pixels == std::string());
    }
    SECTION("Canvas_Rectangle"){
        auto canvas = path::Canvas2D(5,5);
        canvas.draw_rectangle(path::Point(1,1), path::Point(3,3));
        const auto canvas_pixels = canvas.getPixels();
        CHECK(canvas_pixels == std::string(".....\n.XXX.\n.X.X.\n.XXX.\n.....\n"));
    }
}

}