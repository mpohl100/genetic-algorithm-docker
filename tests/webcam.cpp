#include <catch2/catch_all.hpp>

#include "examples/webcam/webcam.h"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>

namespace {

TEST_CASE("Webcam", "[webcam]") {

  SECTION("WebcamProcessFrame") {
    const auto path = "../video/BillardTakeoff.mp4";
    auto cap = cv::VideoCapture{path};
    if (!cap.isOpened()) {
      std::cout << "!!! Input video could not be opened" << std::endl;
      throw std::runtime_error("Cannot open input video");
    }
    CHECK(cap.isOpened());
    CHECK(false);
  }
}

} // namespace