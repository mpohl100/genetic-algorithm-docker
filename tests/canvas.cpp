#include <catch2/catch_all.hpp>

#include "examples/bubbles/Canvas2D.h"

#include <iostream>

namespace {

TEST_CASE("Canvas", "[canvas]") {
  SECTION("Canvas_Line") {
    auto canvas = bubbles::Canvas2D(10, 10);
    canvas.draw_line(bubbles::Line{bubbles::Point(1, 1), bubbles::Point(7, 3)});
    const auto canvas_pixels = canvas.getPixels();
    std::string result = "..........\n"
                         ".XX.......\n"
                         "..X.......\n"
                         "..X.......\n"
                         "..XX......\n"
                         "...X......\n"
                         "...X......\n"
                         "...X......\n"
                         "..........\n"
                         "..........\n";
    CHECK(canvas_pixels == result);
  }
  SECTION("Canvas_Rectangle") {
    auto canvas = bubbles::Canvas2D(5, 5);
    canvas.draw_rectangle(bubbles::Rectangle{bubbles::Point(1, 1), bubbles::Point(3, 3)});
    const auto canvas_pixels = canvas.getPixels();
    CHECK(canvas_pixels == std::string(".....\n.XXX.\n.X.X.\n.XXX.\n.....\n"));
  }
  SECTION("Canvas_Line_Fuzztest") {
    for (size_t i = 1; i < 9; ++i) {
      for (size_t j = 1; j < 9; ++j) {
        auto canvas = bubbles::Canvas2D(10, 10);
        canvas.draw_line(bubbles::Line{bubbles::Point(5, 5), bubbles::Point(i, j)});
        const auto canvas_pixels = canvas.getPixels();
        // std::cout << "i: " << i << " j: " << j << std::endl;
        // std::cout << "fixed x:" << 5 << " fixed y:" << 5 << std::endl;
        CHECK(canvas_pixels[5 * 11 + 5] == 'X');
        if (canvas_pixels[5 * 11 + 5] != 'X') {
          // std::cout << canvas_pixels << std::endl;
        }
        CHECK(canvas_pixels[i * 11 + j] == 'X');
        if (canvas_pixels[i * 11 + j] != 'X') {
          // std::cout << canvas_pixels << std::endl;
        }
      }
    }
  }
  SECTION("Canvas_Circle") {
    auto canvas = bubbles::Canvas2D(10, 10);
    canvas.draw_circle(bubbles::Circle{bubbles::Point(5, 5), 3});
    const auto canvas_pixels = canvas.getPixels();
    CHECK(canvas_pixels == std::string("..........\n"
                                       "..........\n"
                                       "....XXX...\n"
                                       "...X...X..\n"
                                       "..X.....X.\n"
                                       "..X.....X.\n"
                                       "..X.....X.\n"
                                       "...X...X..\n"
                                       "....XXX...\n"
                                       "..........\n"));
  }
  SECTION("Canvas_Angle"){
    // 0 degrees
    const auto line = bubbles::Line{bubbles::Point(0, 0), bubbles::Point(50, 0)};
    const auto line_0 = line;
    const auto angle_0 = bubbles::Angle{line, line_0};
    CHECK(angle_0.radians() == 0.0);

    // 90 degrees
    const auto line_90 = bubbles::Line{bubbles::Point(0, 0), bubbles::Point(0, 50)};
    const auto angle_90 = bubbles::Angle{line, line_90};
    CHECK(angle_90.radians() == M_PI / 2.0);

    // 180 degrees
    const auto line_180 = bubbles::Line{bubbles::Point(0, 0), bubbles::Point(-50, 0)};
    const auto angle_180 = bubbles::Angle{line, line_180};
    CHECK(angle_180.radians() == M_PI);

    // 270 degrees
    const auto line_270 = bubbles::Line{bubbles::Point(0, 0), bubbles::Point(0, -50)};
    const auto angle_270 = bubbles::Angle{line, line_270};
    CHECK(angle_270.radians() ==  - M_PI / 2.0);

    // 359 degrees
    const auto line_360 = bubbles::Line{bubbles::Point(0, 0), bubbles::Point(50, 1)};
    const auto angle_360 = bubbles::Angle{line, line_360};
    CHECK((angle_360.radians() - 0.019997334) < 1e-7);
  }
}

} // namespace