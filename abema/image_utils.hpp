#include <cv.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class ImageUtils {
public:
  static void blend(cv::Mat src, cv::Mat dest) {
    std::vector<cv::Mat> src_channels;
    cv::Mat alpha_map;
    cv::Mat src_rgb;

    cv::split(src, src_channels);

    src_channels[3].convertTo(alpha_map, CV_32FC1);
    alpha_map /= 255.0;

    cv::merge(std::vector<cv::Mat> {
      src_channels[2],
      src_channels[1],
      src_channels[0]
    }, src_rgb);

    for (int i = 0; i < dest.rows; i++) {
      for (int j = 0; j < dest.cols; j++) {
        float a = alpha_map.at<float>(i, j);
        for (int k = 0; k < 3; k++) {
          dest.at<cv::Vec3b>(i, j)[k] =
            dest.at<cv::Vec3b>(i, j)[k] * (1 - a) +
            src_rgb.at<cv::Vec3b>(i, j)[k] * a;
        }
      }
    }
  }
};
