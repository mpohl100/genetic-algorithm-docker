#include "examples/bubbles/establishing_frame.h"

#include <clara.hpp>

#include <chrono>
#include <iostream>
#include <vector>

int main(int argc, char **argv) {
  using namespace clara;

  int tl_x = 20;
  int tl_y = 20;
  int rectangle_x = 50;
  int rectangle_y = 50;
  bool help = false;
  auto cli =
      Opt(tl_x, "tl_x")["-t"]["--tl-x"]("The top left x coordinate") |
      Opt(tl_y, "tl_y")["-u"]["--tl-y"]("The top left y coordinate") |
      Opt(rectangle_x, "rectangle_x")["-r"]["--rectangle-x"](
          "The rectangle x coordinate") |
      Opt(rectangle_y, "rectangle_y")["-s"]["--rectangle-y"]("The rectangle y "
                                                              "coordinate") |
      Help(help);

  auto result = cli.parse(Args(argc, argv));
  if (!result) {
    std::cerr << "Error in command line: " << result.errorMessage() << '\n';
    exit(1);
  }
  if (help) {
    std::cout << cli;
    exit(0);
  }

  auto canvas = bubbles::Canvas2D(100, 100);
  const auto rectangle =
      math2d::Rectangle{math2d::Point(tl_x, tl_y), math2d::Point(tl_x + rectangle_x, tl_y + rectangle_y)};
  canvas.draw_rectangle(rectangle);
  const auto all_rectangles = bubbles::establishing_shot(canvas);
  for(const auto& rectangle : all_rectangles.rectangles) {
    canvas.draw_rectangle(rectangle.to_math2d_rectangle());
  }
  const auto canvas_pixels = canvas.getPixels();
  std::cout << "All rectangles: " << all_rectangles.rectangles.size() << "\n";
  std::cout << '\n';
  std::cout << canvas_pixels << '\n';
  return 0;
}
