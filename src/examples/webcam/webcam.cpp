#include "webcam.h"

#include "opencv2/imgproc/imgproc.hpp"

#include <string>

namespace webcam {

VideoCollector::VideoCollector(const std::string &path,
                               const std::string &appendedStr,
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

VideoCollector::~VideoCollector() { _output_edges.release(); }

void VideoCollector::feed(const cv::Mat &frame) { _output_edges.write(frame); }

std::string VideoCollector::getVideoName(const std::string &path,
                                         const std::string &appendedStr) {
  // Find the position of the last '/' character in the path
  size_t lastSlashPos = path.find_last_of('/');

  // Extract the <my_name> part from the path
  std::string myName =
      path.substr(lastSlashPos + 1, path.find_last_of('.') - lastSlashPos - 1);

  // Construct the new video name by appending _<appendedStr> to <my_name>
  std::string newVideoName =
      path.substr(0, lastSlashPos + 1) + myName + "_" + appendedStr + ".mp4";

  return newVideoName;
};

FrameData processFrame(const cv::Mat &imgOriginal,
                       const bubbles::Rectangle &rectangle,
                       tf::Executor &executor, int rings,
                       int gradient_threshold) {
  auto frame_data = FrameData{};

  const auto create_task_flow = [&](const bubbles::Rectangle &rectangle) {
    const auto calcGradient = [&, rectangle]() {
      frame_data.gradient = od::detect_directions(imgOriginal, rectangle);
      // std::cout << "gradient processed" << std::endl;
    };

    const auto calcSmoothedContours = [&, rectangle, rings,
                                       gradient_threshold]() {
      frame_data.smoothed_contours_mat = od::smooth_angles(
          frame_data.gradient, rings, true, gradient_threshold, rectangle);
      // std::cout << "smoothed contours processed" << std::endl;
    };
    [[maybe_unused]] const auto calcSmoothedGradient = [&]() {
      frame_data.smoothed_gradient_mat = od::smooth_angles(
          frame_data.gradient, rings, false, gradient_threshold, rectangle);
      // std::cout << "smoothed gradient processed" << std::endl;
    };

    const auto populateCanvas = [&, rectangle]() {
      frame_data.canvas =
          od::create_canvas(frame_data.smoothed_contours_mat, rectangle);
      // std::cout << "canvas processed" << std::endl;
    };
    const auto calcAllRectangles = [&, rectangle]() {
      frame_data.all_rectangles =
          bubbles::establishing_shot_slices(frame_data.canvas, rectangle);
      // std::cout << "all rectangles processed" << std::endl;
    };

    tf::Taskflow taskflow;
    // const auto calcCountoursTask = executor.emplace(calcCountours);
    auto calcGradientTask = taskflow.emplace(calcGradient);
    auto calcSmoothedContoursTask = taskflow.emplace(calcSmoothedContours);
    // auto calcSmoothedGradientTask = taskflow.emplace(calcSmoothedGradient);
    auto populateCanvasTask = taskflow.emplace(populateCanvas);
    auto calcAllRectanglesTask = taskflow.emplace(calcAllRectangles);

    calcSmoothedContoursTask.succeed(calcGradientTask);
    populateCanvasTask.succeed(calcSmoothedContoursTask);
    calcAllRectanglesTask.succeed(populateCanvasTask);

    return taskflow;
  };

  auto taskflow = create_task_flow(rectangle);

  executor.run(taskflow).wait();

  return frame_data;
}

} // namespace webcam