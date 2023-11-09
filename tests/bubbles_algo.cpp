#include <catch2/catch_all.hpp>

#include "examples/bubbles/bubbles_swarm.h"

#include <iostream>

namespace {

TEST_CASE("BubblesAlgo", "[bubbles_algo]") {
  SECTION("BubblesAlgoIntegration") {
    auto canvas = bubbles::Canvas2D(100, 100);
    const auto rectangle = bubbles::Rectangle{bubbles::Point(20, 20), bubbles::Point(70, 70)};
    canvas.draw_rectangle(rectangle);
    const auto bubbles_swarm = bubbles::bubbles_algorithm(canvas, bubbles::Point(50, 50));
    CHECK(bubbles_swarm.area() / rectangle.area() >= 0.9);
  }
}

} // namespace