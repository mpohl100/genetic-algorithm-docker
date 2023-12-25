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
  size_t N_rectangles_x = 2;
  size_t N_rectangles_y = 2;
  size_t canvas_width = 100;
  size_t canvas_height = 100;
  bool help = false;
  auto cli =
      Opt(tl_x, "tl_x")["-t"]["--tl-x"]("The top left x coordinate") |
      Opt(tl_y, "tl_y")["-u"]["--tl-y"]("The top left y coordinate") |
      Opt(rectangle_x,
          "rectangle_x")["-r"]["--rectangle-x"]("The rectangle x coordinate") |
      Opt(rectangle_y, "rectangle_y")["-s"]["--rectangle-y"]("The rectangle y "
                                                             "coordinate") |
      Opt(N_rectangles_x,
          "N_x")["-x"]["--N-x"]("The number of rectangles in x direction") |
      Opt(N_rectangles_y,
          "N_y")["-y"]["--N-y"]("The number of rectangles in y direction") |
      Opt(canvas_width,
          "canvas_width")["-w"]["--canvas-width"]("The width of the canvas") |
      Opt(canvas_height, "canvas_height")["-h"]["--canvas-height"]("The "
                                                                   "height of "
                                                                   "the "
                                                                   "canvas") |
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

  auto canvas = bubbles::Canvas2D(canvas_width, canvas_height);
  for (size_t i = 0; i < N_rectangles_x; ++i) {
    for (size_t j = 0; j < N_rectangles_y; ++j) {
      const auto rectangle = math2d::Rectangle{
          math2d::Point(tl_x + i * rectangle_x, tl_y + j * rectangle_y),
          math2d::Point(tl_x + (i + 1) * rectangle_x,
                        tl_y + (j + 1) * rectangle_y)};
      canvas.draw_rectangle(rectangle);
    }
  }
  const auto all_rectangles = bubbles::establishing_shot(canvas);
  for (const auto &rectangle : all_rectangles.rectangles) {
    canvas.draw_rectangle(rectangle.to_math2d_rectangle());
  }
  const auto canvas_pixels = canvas.getPixels();
  std::cout << canvas_pixels << '\n';
  std::cout << "All rectangles: " << all_rectangles.rectangles.size() << "\n";
  std::cout << '\n';
  return 0;
}
