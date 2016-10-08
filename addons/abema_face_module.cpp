#include <iostream>
#include <cmath>

#include <node.h>
#include <v8.h>
#include <nan.h>

#include <cv.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "dlib/opencv.h"
#include "dlib/image_io.h"
#include "dlib/image_transforms.h"
#include "dlib/image_processing.h"
#include "dlib/image_processing/frontal_face_detector.h"

#include "image_utils.hpp"

using namespace dlib;
using namespace std;
using namespace v8;

class AbemaFace {
private:
  frontal_face_detector detector;
  shape_predictor pose_model;
  cv::VideoCapture cap;

  cv::Mat abema_nose;
  cv::Mat abema_head;
  cv::Mat result_image;

  void blend_nose(cv::Mat screen, int x , int y, double scale, double rotation) {
    cv::Mat abema_scaled;
    cv::resize(abema_nose, abema_scaled, cv::Size(), scale, scale, cv::INTER_CUBIC);
    cv::Point2d ctr(abema_scaled.cols / 2, abema_scaled.rows / 2);
    cv::Mat mv = cv::getRotationMatrix2D(ctr, rotation, 1);
    mv.at<double>(0, 2) += x - abema_scaled.cols / 2;
    mv.at<double>(1, 2) += y - abema_scaled.rows / 2;

    cv::Mat nose_screen(screen.size(), screen.type(), cv::Scalar(0, 0, 0));
    cv::warpAffine(abema_scaled, nose_screen, mv, nose_screen.size(),
                   cv::INTER_CUBIC, cv::BORDER_TRANSPARENT);

    ImageUtils::blend(nose_screen, screen);
  }

  void blend_head(cv::Mat screen, int x , int y, double scale, double rotation) {
    cv::Mat abema_scaled;
    cv::resize(abema_head, abema_scaled, cv::Size(), scale, scale, cv::INTER_CUBIC);

    cv::Point2d ctr(abema_scaled.cols / 2, abema_scaled.rows);
    cv::Mat mv = cv::getRotationMatrix2D(ctr, rotation, 1);
    mv.at<double>(0, 2) += x - abema_scaled.cols / 2;
    mv.at<double>(1, 2) += y - abema_scaled.rows / 1.25;

    cv::Mat head_screen(screen.size(), screen.type(), cv::Scalar(0, 0, 0));
    cv::warpAffine(abema_scaled, head_screen, mv, head_screen.size(),
                   cv::INTER_CUBIC, cv::BORDER_TRANSPARENT);

    ImageUtils::blend(head_screen, screen);
  }

public:
  void init() {
    detector = get_frontal_face_detector();

    cap = cv::VideoCapture(0);
    if (!cap.isOpened()) {
      cerr << "Unable to connect to camera" << endl;
      return;
    }

    deserialize("assets/shape_predictor_68_face_landmarks.dat") >> pose_model;
    abema_nose = cv::imread("assets/abema-nose.png", -1);
    abema_head = cv::imread("assets/abema-head.png", -1);
  }

  void draw(unsigned char* array, int width, int height) {
    cv::Mat temp;

    cap >> temp;
    cv::flip(temp, temp, 1);
    cv_image<bgr_pixel> cimg(temp);

    // Detect faces
    std::vector<rectangle> faces = detector(cimg);

    // Find the pose of each face.
    std::vector<full_object_detection> shapes;
    for (unsigned long i = 0; i < faces.size(); ++i) {
      shapes.push_back(pose_model(cimg, faces[i]));
    }

    if (shapes.size() > 0) {
      for (unsigned long i = 0; i < shapes.size(); ++i) {
        const full_object_detection& d = shapes[i];
        int x = d.part(30).x();
        int y = d.part(30).y();
        double z = (d.part(16).x() - d.part(1).x()) / 600.0;
        double rad = -atan2(d.part(28).y() - d.part(9).y(),
                            d.part(28).x() - d.part(9).x()) - 3.14 / 2.0;
        double r = rad * 180 / 3.14;

        int brow_left_x = d.part(20).x();
        int brow_left_y = d.part(20).y();
        int brow_right_x = d.part(25).x();
        int brow_right_y = d.part(25).y();

        blend_nose(temp, x, y, z / 5.0, r);
        blend_head(temp,
                   (brow_left_x + brow_right_x) / 2,
                   (brow_left_y + brow_right_y) / 2,
                   z / 2.0, r);

        // crop face around area
        double crop_size = 1500 * z;
        cv::Rect crop_rect(x - crop_size / 2,
                           y - crop_size / 2 - crop_size / 5,
                           crop_size, crop_size);
        cv::Rect bounds(0, 0, temp.size().width, temp.size().height);
        crop_rect = crop_rect & bounds;
        result_image = cv::Mat(temp, crop_rect);

        // draw crop rectangle
        cv::rectangle(temp,
                      crop_rect.tl() - cv::Point(1, 1),
                      crop_rect.br() + cv::Point(1, 1),
                      cv::Scalar(74, 149, 60), 1);
      }
    }

    // resize output
    cv::Mat resized(height, width, temp.type());
    cv::resize(temp, resized, resized.size(), cv::INTER_CUBIC);

    // copy output to array
    for (int i = 0; i < width; i++) {
      for (int j = 0; j < height; j++) {
        int dest_index = (i + width * j) * 4;
        int src_index = (j * resized.cols + i) * 3;
        array[dest_index + 0] = resized.data[src_index + 2];
        array[dest_index + 1] = resized.data[src_index + 1];
        array[dest_index + 2] = resized.data[src_index + 0];
        array[dest_index + 3] = 255;
      }
    }
  }

  void save(std::string filename) {
    cv::resize(result_image, result_image, cv::Size(900, 900), cv::INTER_CUBIC);
    cv::imwrite(filename, result_image);
  }
};

// export as node module

AbemaFace abema_face;

void draw(const Nan::FunctionCallbackInfo<Value>& args) {
  Local<Uint8ClampedArray> array = args[0].As<Uint8ClampedArray>();
  unsigned char* ptr = (unsigned char*)array->Buffer()->GetContents().Data();
  int width = args[1]->NumberValue();
  int height = args[2]->NumberValue();
  abema_face.draw(ptr, width, height);
}

void save(const Nan::FunctionCallbackInfo<Value>& args) {
  v8::String::Utf8Value str(args[0]->ToString());
  char *cstr = *str;
  abema_face.save(std::string(cstr));
}

void init(Local<Object> exports) {
  abema_face.init();

  exports->Set(Nan::New("draw").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(draw)->GetFunction());
  exports->Set(Nan::New("save").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(save)->GetFunction());
}

NODE_MODULE(addon, init)
