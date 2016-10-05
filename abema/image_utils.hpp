#include <cv.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class ImageUtils {
public:
  static void blend(cv::Mat src, cv::Mat dest) {
    std::vector<cv::Mat> src_channels;
    cv::Mat alpha_map, src_alpha, src_rgb;

    cv::split(src, src_channels);
    cv::normalize(src_channels[3], alpha_map, 0.0f, 1.0f, cv::NORM_MINMAX);

    cv::merge(std::vector<cv::Mat> {
      alpha_map,
      alpha_map,
      alpha_map
    }, src_alpha);

    cv::merge(std::vector<cv::Mat> {
      src_channels[0],
      src_channels[1],
      src_channels[2]
    }, src_rgb);

    dest = dest.mul(cv::Scalar::all(1) - src_alpha) + src_rgb.mul(src_alpha);
  }
};
