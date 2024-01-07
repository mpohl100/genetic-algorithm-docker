#include "webcam.h"

#include "examples/bubbles/establishing_frame.h"

#include "opencv2/imgproc/imgproc.hpp"

#include <string>

namespace webcam {

VideoCollector::VideoCollector(const std::string &path,
                               const std::string &appendedStr,
                               const cv::VideoCapture &input_cap)
    : _output_edges{get_video_name(path, appendedStr),
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

std::string VideoCollector::get_video_name(const std::string &path,
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

void read_image_data(cv::VideoCapture &cap, cv::Mat &imgOriginal,
                     int &retflag) {
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

FrameData::FrameData(const cv::Mat &imgOriginal)
    : contours{imgOriginal.clone()}, gradient{imgOriginal.clone()},
      smoothed_contours_mat{imgOriginal.clone()},
      smoothed_gradient_mat{imgOriginal.clone()},
      canvas{imgOriginal.cols, imgOriginal.rows}, all_rectangles{} {}

FrameData process_frame(const cv::Mat &imgOriginal,
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

std::vector<bubbles::Rectangle>
split_rectangle(const bubbles::Rectangle &rectangle, int nb_splits) {
  const auto width = rectangle.width;
  const auto height = rectangle.height;
  const auto x = rectangle.x;
  const auto y = rectangle.y;
  const auto width_per_thread = width / nb_splits;
  const auto height_per_thread = height / nb_splits;
  auto rectangles = std::vector<bubbles::Rectangle>{};
  for (auto i = 0; i < nb_splits; ++i) {
    for (auto j = 0; j < nb_splits; ++j) {
      rectangles.emplace_back(x + i * width_per_thread,
                              y + j * height_per_thread, width_per_thread,
                              height_per_thread);
    }
  }
  return rectangles;
}

FrameData process_frame_quadview(const cv::Mat &imgOriginal,
                                 const bubbles::Rectangle &rectangle,
                                 tf::Executor &executor, int rings,
                                 int gradient_threshold, int nb_splits) {
  auto frame_data = FrameData{imgOriginal};
  const auto rectangles = split_rectangle(rectangle, nb_splits);
  return process_frame(imgOriginal, rectangle, executor, rings,
                       gradient_threshold);
}

} // namespace webcam