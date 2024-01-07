#include "examples/bubbles/Canvas2D.h"
#include "examples/bubbles/establishing_frame.h"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <taskflow/taskflow.hpp>

#include <string>

namespace webcam {

class VideoCollector {
public:
  VideoCollector(const std::string &path, const std::string &appendedStr,
                 const cv::VideoCapture &input_cap);

  ~VideoCollector();

  void feed(const cv::Mat &frame);

private:
  std::string get_video_name(const std::string &path,
                             const std::string &appendedStr);
  // members
  cv::VideoWriter _output_edges;
};

void read_image_data(cv::VideoCapture &cap, cv::Mat &imgOriginal, int &retflag);

struct FrameData {
  cv::Mat contours;
  cv::Mat gradient;
  cv::Mat smoothed_contours_mat;
  cv::Mat smoothed_gradient_mat;
  bubbles::Canvas2D canvas;
  bubbles::AllRectangles all_rectangles;

  FrameData() = default;
  FrameData(const FrameData &) = delete;
  FrameData(FrameData &&) = default;
  FrameData &operator=(const FrameData &) = delete;
  FrameData &operator=(FrameData &&) = default;

  FrameData(const cv::Mat &imgOriginal);
};

std::pair<tf::Future<void>, tf::Taskflow>
process_frame(FrameData &frameData, const cv::Mat &imgOriginal,
              const bubbles::Rectangle &rectangle, tf::Executor &executor,
              int rings, int gradient_threshold);

FrameData process_frame_quadview(const cv::Mat &imgOriginal,
                                 const bubbles::Rectangle &rectangle,
                                 tf::Executor &executor, int rings,
                                 int gradient_threshold, int nb_splits = 2);

} // namespace webcam