#pragma once

#include "examples/bubbles/Canvas2D.h"
#include "opencv2/core/mat.hpp"
#include <cmath>
namespace od {

cv::Mat detect_directions(cv::Mat const &bgrImg);
// cv::Mat detect_edges_gray(cv::Mat const& bgrImg);
cv::Mat detect_angles(cv::Mat const &bgrImg);

cv::Mat smooth_angles(cv::Mat const &angles, int rings, bool onlyRecordAngles, int threshold);

bubbles::Canvas2D create_canvas(const cv::Mat &contours);

} // namespace od