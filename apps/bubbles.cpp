#include "examples/bubbles/bubbles_swarm.h"

#include <clara.hpp>

#include <chrono>
#include <iostream>
#include <vector>

int main(int argc, char **argv) {
  using namespace clara;

  int start_x = 50;
  int start_y = 50;
  int tl_x = 20;
  int tl_y = 20;
  int rectangle_x = 50;
  int rectangle_y = 50;
  int canvas_width = 100;
  int canvas_height = 100;
  bool help = false;
  auto cli =
      Opt(start_x, "start_x")["-x"]["--start-x"]("The start x coordinate") |
      Opt(start_y, "start_y")["-y"]["--start-y"]("The start y coordinate") |
      Opt(tl_x, "tl_x")["-t"]["--tl-x"]("The top left x coordinate") |
      Opt(tl_y, "tl_y")["-u"]["--tl-y"]("The top left y coordinate") |
      Opt(rectangle_x,
          "rectangle_x")["-r"]["--rectangle-x"]("The rectangle x coordinate") |
      Opt(rectangle_y, "rectangle_y")["-s"]["--rectangle-y"]("The rectangle y "
                                                             "coordinate") |
      Opt(canvas_width, "canvas_width")["-w"]["--canvas-width"]("The width of "
                                                                "the canvas") |
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
  auto tried_circles = tiles::CircleTiles(canvas.width(), canvas.height(), 10);
  const auto rectangle =
      math2d::Rectangle{math2d::Point(tl_x, tl_y),
                        math2d::Point(tl_x + rectangle_x, tl_y + rectangle_y)};
  canvas.draw_rectangle(rectangle);
  const auto already_optimized = bubbles::bubbles_algorithm_slow(
      canvas, math2d::Point(start_x, start_y), tried_circles);
  for (const auto &circle : already_optimized.circles()) {
    canvas.draw_circle(circle);
  }
  const auto canvas_pixels = canvas.getPixels();
  std::cout << '\n';
  std::cout << canvas_pixels << '\n';
  return 0;
}
