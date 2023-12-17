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

cv::Mat smooth_angles(cv::Mat const &angles, int rings, bool onlyRecordAngles, int threshold) {
  cv::Mat result = angles.clone();
  std::vector<const cv::Vec3b *> rows;
  cv::Vec3b *resultRow = nullptr;
  for (int i = 0; i < 2 * rings + 1; ++i)
    rows.push_back(nullptr);
  for (int i = rings; i < angles.rows - rings; ++i) {
    resultRow = result.ptr<cv::Vec3b>(i);
    size_t index = 0;
    for (int j = i - rings; j < i + rings + 1; ++j)
      rows[index++] = angles.ptr<cv::Vec3b>(j);
    for (int j = rings; j < angles.cols - rings; ++j) {
      double sumAngle = 0;
      double sumLen = 0;

      for (int k = 0; k < int(rows.size()); ++k)
        for (int l = j - rings; l < j + rings + 1; ++l) {

          double len = double(rows[k][l][0]);
          if (len > 0) {
            sumLen += len;
            int angle = rows[k][l][1];
            if (angle > 0)
              sumAngle += len * angle;
            else
              sumAngle -= len * rows[k][l][2];
          }
        }

      int nb = 2 * rings + 1;
      nb *= nb; // squared
      if (sumLen / nb < threshold) {
        resultRow[j][0] = 255;
        resultRow[j][1] = 255;
        resultRow[j][2] = 255;
      } else {
        double angle = sumAngle / sumLen;
        double len = sumLen / nb;
        resultRow[j][0] = int(len);
        if (angle > 0) {
          if (!onlyRecordAngles) {
            resultRow[j][1] = int(angle);
            resultRow[j][2] = 0;
          } else {
            resultRow[j][0] = int(angle * 256.0 / 180.0);
            resultRow[j][1] = 0;
            resultRow[j][2] = 0;
          }
        } else {
          if (!onlyRecordAngles) {
            resultRow[j][1] = 0;
            resultRow[j][2] = int(-angle);
          } else {
            resultRow[j][0] = 0;
            int angleInt = int(-angle * 256.0 / 180.0);
            resultRow[j][1] = angleInt;
            resultRow[j][2] = 0;
          }
        }
      }
    }
  }
  return result;
}

} // namespace od