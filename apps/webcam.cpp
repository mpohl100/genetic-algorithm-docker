#include "examples/bubbles/detection/Detection.h"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <clara.hpp>

#include <iostream>

void readImageData(cv::VideoCapture &cap, cv::Mat &imgOriginal, int &retflag) {
  retflag = 1;
  bool bSuccess = cap.read(imgOriginal); // read a new frame from video
  if (!bSuccess)                         // if not success, break loop
  {
    std::cout << "Cannot read a frame from video stream" << std::endl;
    {
      retflag = 2;
      return;
    };
  }
}

int main(int argc, char **argv) {
  using namespace clara;

  int number_webcam = 0;
  int rings = 1;
  int gradient_threshold = 20;
  std::string path = "";
  bool help = false;
  auto cli = Opt(number_webcam, "number_webcam")["-n"]["--number-webcam"](
                 "The number of the webcam to use") |
             Opt(path, "path")["-p"]["--path"]("The path to the video file") |
             Opt(rings, "rings")["-r"]["--rings"](
                 "The number of rings to use for smoothing") |
             Opt(gradient_threshold, "threshold")["-t"]["--threshold"](
                 "The threshold to use for smoothing") |
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

  // cv::VideoCapture cap("D:\ToiletBank.mp4"); //capture the video from file
  cv::VideoCapture cap;
  if (path != "") {
    cap = cv::VideoCapture{path};
    if (!cap.isOpened()) // if not success, exit program
    {
      std::cout << "Cannot open the video file" << std::endl;
      return -1;
    }
  } else {
    cap = cv::VideoCapture{number_webcam};
    if (!cap.isOpened()) // if not success, exit program
    {
      std::cout << "Cannot open the webcam" << std::endl;
      return -1;
    }
  } // capture the video from web cam

  std::string original = "Original";
  std::string threshold = "Thresholded Image";
  std::string smoothed_angles = "Smoothed Angles";
  std::string smoothed_gradient = "Smoothed Gradient";
  namedWindow(original, cv::WINDOW_AUTOSIZE);
  namedWindow(threshold, cv::WINDOW_AUTOSIZE);
  namedWindow(smoothed_angles, cv::WINDOW_AUTOSIZE);
  namedWindow(smoothed_gradient, cv::WINDOW_AUTOSIZE);

  // int j = 0;
  // std::array<Result, N> previous;
  while (true) {
    cv::Mat imgOriginal;
    int retflag;
    readImageData(cap, imgOriginal, retflag);
    if (retflag == 2)
      break;
    cv::Mat contours = od::detect_angles(imgOriginal);
    cv::Mat gradient = od::detect_directions(imgOriginal);

    auto smoothed_contours_mat = od::smooth_angles(gradient, rings, true, gradient_threshold);
    auto smoothed_gradient_mat = od::smooth_angles(gradient, rings, false, gradient_threshold);

    // auto partials = smooth_results(calculate_orientation(gradient), 10);
    // for (const auto& partial : partials)
    //	cv::circle(contours, cv::Point(int(partial.point.x),
    // int(partial.point.y)), 5, cv::Scalar(0, 0, 256));
    // draw_bars(contours, partials);

    imshow(threshold, contours);   // show the thresholded image
    imshow(original, imgOriginal); // show the original image
    imshow(smoothed_angles, smoothed_contours_mat);   // the smoothed contours
    imshow(smoothed_gradient, smoothed_gradient_mat); // the smoothed gradient

    std::cout << "Frame processed!" << std::endl;

    if (cv::waitKey(30) == 27) // wait for 'esc' key press for 30ms. If 'esc'
                               // key is pressed, break loop
    {
      std::cout << "esc key is pressed by user" << std::endl;
      break;
    }
  }
  return 0;
}
