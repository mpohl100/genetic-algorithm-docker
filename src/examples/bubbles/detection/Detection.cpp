#include "Detection.h"
#include "DetectionImpl.h"

#include "opencv2/imgproc.hpp"
#include <iostream>

namespace od {
cv::Mat detect_directions(cv::Mat const &bgrImg) {
  return detail::detect_edges<detail::DetectionType::Gradient>(bgrImg);
}

// cv::Mat detect_edges_gray(cv::Mat const& bgrImg)
//{
//	return detail::detect_edges<detail::DetectionType::Edge>(bgrImg);
// }

cv::Mat detect_angles(cv::Mat const &bgrImg) {
  return detail::detect_edges<detail::DetectionType::Angle>(bgrImg);
}
} // namespace od