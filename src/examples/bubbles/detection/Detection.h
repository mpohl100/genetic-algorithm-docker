#pragma once

#include "Rectangle.h"
#include "examples/bubbles/Canvas2D.h"
#include "opencv2/core/mat.hpp"
#include <cmath>

namespace od {

void detect_directions(cv::Mat &ret, cv::Mat const &bgrImg, const Rectangle& rectangle);
// cv::Mat detect_edges_gray(cv::Mat const& bgrImg);
void detect_angles(cv::Mat &ret, cv::Mat const &bgrImg, const Rectangle& rectangle);

void smooth_angles(cv::Mat &result, cv::Mat const &angles, int rings, bool onlyRecordAngles, int threshold, const Rectangle& rectangle);

bubbles::Canvas2D create_canvas(const cv::Mat &contours, const Rectangle& rectangle);

} // namespace od