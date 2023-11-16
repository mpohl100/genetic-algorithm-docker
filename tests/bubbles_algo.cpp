#include <catch2/catch_all.hpp>

#include "examples/bubbles/bubbles_swarm.h"

#include <iostream>

namespace {

TEST_CASE("BubblesAlgo", "[bubbles_algo]") {
  SECTION("BubblesAlgoIntegration") {
    auto canvas = bubbles::Canvas2D(100, 100);
    const auto rectangle =
        bubbles::Rectangle{bubbles::Point(20, 20), bubbles::Point(70, 70)};
    canvas.draw_rectangle(rectangle);
    auto evolParams = evol::partial::PartialEvolutionOptions{};
    evolParams.num_generations = 100;
    evolParams.log_level = 0;
    evolParams.num_parents = 2;
    evolParams.num_children = 20;
    evolParams.out = &std::cout;
    evolParams.min_magnitude = 0.9;
    evolParams.max_magnitude = 1.1;
    const auto already_optimized =
        bubbles::bubbles_algorithm(canvas, bubbles::Point(50, 50), evolParams);
    CHECK(already_optimized.area() / rectangle.area() >= 0.9);
    canvas.draw_circle(already_optimized.circles()[0]);
    const auto canvas_pixels = canvas.getPixels();
    // CHECK(canvas_pixels == std::string());
    const auto circle = already_optimized.circles()[0];
    std::cout << "center x: " << circle.center().x
              << " center y: " << circle.center().y
              << "; radius: " << circle.radius() << std::endl;
  }
}

} // namespace