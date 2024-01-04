#include <catch2/catch_all.hpp>

#include "examples/webcam/webcam.h"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <taskflow/taskflow.hpp>

#include <iostream>

namespace {

TEST_CASE("Webcam", "[webcam]") {

  SECTION("WebcamProcessFrame") {
    tf::Executor executor(4);
    int rings = 1;
    int gradient_threshold = 20;
    const auto path = "../video/BillardTakeoff.mp4";

    auto cap = cv::VideoCapture{path};
    if (!cap.isOpened()) {
      std::cout << "!!! Input video could not be opened" << std::endl;
      throw std::runtime_error("Cannot open input video");
    }
    CHECK(cap.isOpened());

    cv::Mat imgOriginal;
    int retflag = -1;
    webcam::read_image_data(cap, imgOriginal, retflag);

    CHECK(retflag != 2);
    if (retflag == 2) {
      return;
    }

    const auto rectangle =
        bubbles::Rectangle{0, 0, imgOriginal.cols, imgOriginal.rows};
    const auto frame_data = webcam::process_frame(
        imgOriginal, rectangle, executor, rings, gradient_threshold);

    CHECK(frame_data.all_rectangles.rectangles.size() == 3335);
  }
}

} // namespace