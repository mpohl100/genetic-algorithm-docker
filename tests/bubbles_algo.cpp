#include <catch2/catch_all.hpp>

#include "examples/bubbles/bubbles_swarm.h"

#include <iostream>

namespace {

TEST_CASE("BubblesAlgo", "[bubbles_algo]") {

  SECTION("AngleArea") {
    // test angle area 0
    const auto angle_area_0 = bubbles::AngleArea{0, 6};
    const auto angle_30 = angle_area_0.get_angle(0.5);
    CHECK((angle_30.degrees() - 30.0) < 1e-7);
    CHECK(!angle_area_0.is_within(math2d::Angle{-1}));
    CHECK(angle_area_0.is_within(math2d::Angle{0}));
    CHECK(angle_area_0.is_within(math2d::Angle{60}));
    CHECK(!angle_area_0.is_within(math2d::Angle{61}));

    // test angle area 1
    const auto angle_area_1 = bubbles::AngleArea{1, 6};
    const auto angle_90 = angle_area_1.get_angle(0.5);
    CHECK((angle_90.degrees() - 90.0) < 1e-7);
    CHECK(!angle_area_1.is_within(math2d::Angle{59}));
    CHECK(angle_area_1.is_within(math2d::Angle{60}));
    CHECK(angle_area_1.is_within(math2d::Angle{120}));
    CHECK(!angle_area_1.is_within(math2d::Angle{121}));

    // test angle area 2
    const auto angle_area_2 = bubbles::AngleArea{2, 6};
    const auto angle_150 = angle_area_2.get_angle(0.5);
    CHECK((angle_150.degrees() - 150.0) < 1e-7);
    CHECK(!angle_area_2.is_within(math2d::Angle{119}));
    CHECK(angle_area_2.is_within(math2d::Angle{120}));
    CHECK(angle_area_2.is_within(math2d::Angle{180}));
    CHECK(!angle_area_2.is_within(math2d::Angle{181}));

    // test angle area 3
    const auto angle_area_3 = bubbles::AngleArea{3, 6};
    const auto angle_210 = angle_area_3.get_angle(0.5);
    CHECK((angle_210.degrees() - 210.0) < 1e-7);
    CHECK(!angle_area_3.is_within(math2d::Angle{179}));
    CHECK(angle_area_3.is_within(math2d::Angle{180}));
    CHECK(angle_area_3.is_within(math2d::Angle{240}));
    CHECK(!angle_area_3.is_within(math2d::Angle{241}));

    // test angle area 4
    const auto angle_area_4 = bubbles::AngleArea{4, 6};
    const auto angle_270 = angle_area_4.get_angle(0.5);
    CHECK((angle_270.degrees() - 270.0) < 1e-7);
    CHECK(!angle_area_4.is_within(math2d::Angle{239}));
    CHECK(angle_area_4.is_within(math2d::Angle{240}));
    CHECK(angle_area_4.is_within(math2d::Angle{300}));
    CHECK(!angle_area_4.is_within(math2d::Angle{301}));

    // test angle area 5
    const auto angle_area_5 = bubbles::AngleArea{5, 6};
    const auto angle_330 = angle_area_5.get_angle(0.5);
    CHECK((angle_330.degrees() - 330.0) < 1e-7);
    CHECK(!angle_area_5.is_within(math2d::Angle{299}));
    CHECK(angle_area_5.is_within(math2d::Angle{300}));
    CHECK(!angle_area_5.is_within(math2d::Angle{360}));
    CHECK(!angle_area_5.is_within(math2d::Angle{361}));
  }
#if 0
  SECTION("BubblesAlgoIntegration") {
    auto canvas = bubbles::Canvas2D(100, 100);
    const auto rectangle =
        math2d::Rectangle{math2d::Point(20, 20), math2d::Point(70, 70)};
    canvas.draw_rectangle(rectangle);
    const auto already_optimized =
        bubbles::bubbles_algorithm_slow(canvas, math2d::Point(50, 50));
    const auto ratio = already_optimized.area() / rectangle.area();
    CHECK(ratio >= 0.75);
    CHECK(ratio < 1);
    canvas.draw_circle(already_optimized.circles()[0]);
    const auto canvas_pixels = canvas.getPixels();
    // CHECK(canvas_pixels == std::string());
    const auto circle = already_optimized.circles()[0];
    std::cout << "center x: " << circle.center().x
              << " center y: " << circle.center().y
              << "; radius: " << circle.radius() << std::endl;
  }
#endif
}

} // namespace