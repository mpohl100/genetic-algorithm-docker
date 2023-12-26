#include "examples/bubbles/detection/Detection.h"
#include "examples/bubbles/establishing_frame.h"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <clara.hpp>

#include <iostream>
#include <stdexcept>

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

class VideoCollector {
public:
  VideoCollector(const std::string &path, const std::string &appendedStr,
                 const cv::VideoCapture &input_cap)
      : _output_edges{getVideoName(path, appendedStr),
                      static_cast<int>(input_cap.get(cv::CAP_PROP_FOURCC)),
                      input_cap.get(cv::CAP_PROP_FPS),
                      cv::Size(input_cap.get(cv::CAP_PROP_FRAME_WIDTH),
                               input_cap.get(cv::CAP_PROP_FRAME_HEIGHT))} {
    if (!_output_edges.isOpened()) {
      std::cout << "!!! Output video edgescould not be opened" << std::endl;
      throw std::runtime_error("Cannot open output video for edges");
    }
  }

  ~VideoCollector() { _output_edges.release(); }

  void feed(const cv::Mat &frame) { _output_edges.write(frame); }

private:
  std::string getVideoName(const std::string &path,
                           const std::string &appendedStr) {
    // Find the position of the last '/' character in the path
    size_t lastSlashPos = path.find_last_of('/');

    // Extract the <my_name> part from the path
    std::string myName = path.substr(lastSlashPos + 1,
                                     path.find_last_of('.') - lastSlashPos - 1);

    // Construct the new video name by appending _<appendedStr> to <my_name>
    std::string newVideoName =
        path.substr(0, lastSlashPos + 1) + myName + "_" + appendedStr + ".mp4";

    return newVideoName;
  };
  // members
  cv::VideoWriter _output_edges;
};

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

  auto collectorEdges = VideoCollector{path, "edge", cap};
  auto collectorSmoothed = VideoCollector{path, "smoothed", cap};
  auto collectorResult = VideoCollector{path, "result", cap};
  auto collectorGradientResult = VideoCollector{path, "result_gradient", cap};

  std::string original = "Original";
  std::string threshold = "Thresholded Image";
  std::string smoothed_angles = "Smoothed Angles";
  std::string smoothed_gradient = "Smoothed Gradient";
  // namedWindow(original, cv::WINDOW_AUTOSIZE);
  // namedWindow(threshold, cv::WINDOW_AUTOSIZE);
  // namedWindow(smoothed_angles, cv::WINDOW_AUTOSIZE);
  // namedWindow(smoothed_gradient, cv::WINDOW_AUTOSIZE);

  int i = 0;
  while (true) {
    cv::Mat imgOriginal;
    int retflag;
    readImageData(cap, imgOriginal, retflag);
    if (retflag == 2)
      break;
    cv::Mat contours = od::detect_angles(imgOriginal);
    cv::Mat gradient = od::detect_directions(imgOriginal);

    auto smoothed_contours_mat =
        od::smooth_angles(gradient, rings, true, gradient_threshold);
    auto smoothed_gradient_mat =
        od::smooth_angles(gradient, rings, false, gradient_threshold);

    const auto canvas = od::create_canvas(smoothed_contours_mat);

    const auto all_rectangles = bubbles::establishing_shot_slices(canvas);

    // draw all rectangles on copy of imgOriginal
    auto imgOriginalResult = imgOriginal.clone();
    for (const auto &rectangle : all_rectangles.rectangles) {
      int rectX = std::max(0, rectangle.x);
      int rectY = std::max(0, rectangle.y);
      int rectWidth = std::min(imgOriginalResult.cols - rectX, rectangle.width);
      int rectHeight = std::min(imgOriginalResult.rows - rectY, rectangle.height);
      const auto cv_rectangle = cv::Rect{rectX, rectY, rectWidth, rectHeight};
      cv::rectangle(imgOriginalResult, cv_rectangle, cv::Scalar(0, 255, 0), 2);
    }

    auto imgGradientResult = gradient.clone();
    for (const auto &rectangle : all_rectangles.rectangles) {
      int rectX = std::max(0, rectangle.x);
      int rectY = std::max(0, rectangle.y);
      int rectWidth = std::min(imgGradientResult.cols - rectX, rectangle.width);
      int rectHeight = std::min(imgGradientResult.rows - rectY, rectangle.height);
      const auto cv_rectangle = cv::Rect{rectX, rectY, rectWidth, rectHeight};
      cv::rectangle(imgGradientResult, cv_rectangle, cv::Scalar(0, 255, 0), 2);
    }
    // imshow(threshold, contours);   // show the thresholded image
    // imshow(original, imgOriginal); // show the original image
    // imshow(smoothed_angles, smoothed_contours_mat);   // the smoothed
    // contours imshow(smoothed_gradient, smoothed_gradient_mat); // the
    // smoothed gradient

    collectorEdges.feed(gradient);
    collectorSmoothed.feed(smoothed_contours_mat);
    collectorResult.feed(imgOriginalResult);
    collectorGradientResult.feed(imgGradientResult);

    std::cout << "Frame " << ++i << " processed!" << std::endl;

    if (cv::waitKey(30) == 27) // wait for 'esc' key press for 30ms. If 'esc'
                               // key is pressed, break loop
    {
      std::cout << "esc key is pressed by user" << std::endl;
      break;
    }
  }
  return 0;
}
