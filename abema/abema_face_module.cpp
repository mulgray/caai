#include <iostream>

#include <node.h>
#include <v8.h>
#include <nan.h>

#include <cv.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "../dlib/opencv.h"
#include "../dlib/image_io.h"
#include "../dlib/image_transforms.h"
#include "../dlib/image_processing.h"
#include "../dlib/image_processing/frontal_face_detector.h"

using namespace dlib;
using namespace std;
using namespace v8;

class Profiler {
private:
  static std::string tag;
  static long start_tick;

public:
  static void start(std::string tag) {
    Profiler::tag = tag;
    start_tick = cv::getTickCount();
  }

  static void stop() {
    long end_tick = cv::getTickCount();
    double tick = (end_tick - start_tick) * 1000 / cv::getTickFrequency();
    std::cout << "PROFILER(" << tag << "): " << tick << " ms" << std::endl;
  }
};

std::string Profiler::tag;
long Profiler::start_tick;

class AbemaFace {
private:
  frontal_face_detector detector;
  shape_predictor pose_model;
  cv::VideoCapture cap;
  cv::Mat abema_source;

  cv::Mat result_image;

public:
  void init() {
    detector = get_frontal_face_detector();
    cap = cv::VideoCapture(0);
    if (!cap.isOpened()) {
      cerr << "Unable to connect to camera" << endl;
      return;
    }
    deserialize("assets/shape_predictor_68_face_landmarks.dat") >> pose_model;
    abema_source = cv::imread("assets/abema-nose.png", -1);
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
      int x = 0;
      int y = 0;
      double r = 0;
      double z = 0;
      for (unsigned long i = 0; i < shapes.size(); ++i) {
        const full_object_detection& d = shapes[0];
        // x, y: position of a nose
        // z: projected ear to ear distance of the face
        x = d.part(30).x();
        y = d.part(30).y();
        z = (d.part(16).x() - d.part(1).x()) / 600.0;
        r = (d.part(30).x() - d.part(28).x()) * 3.14;
      }

      cv::Mat abema_resized;
      cv::resize(abema_source, abema_resized, cv::Size(), z, z);
      cv::Point2d ctr(abema_resized.cols / 2, abema_resized.rows / 2);
      cv::Mat mv = cv::getRotationMatrix2D(ctr, r, 1.0);
      mv.at<double>(0, 2) += x - (abema_resized.cols / 2);
      mv.at<double>(1, 2) += y - (abema_resized.rows / 2);

      std::vector<cv::Mat> abemalpha;
      cv::split(abema_resized, abemalpha);
      cv::Mat alpha32f, abealpha;
      abemalpha[3].convertTo(alpha32f, CV_8UC1);
      cv::normalize(alpha32f, alpha32f, 0.0f, 1.0f, cv::NORM_MINMAX);
      cv::cvtColor(alpha32f, abealpha, CV_GRAY2BGR, 3);

      std::vector<cv::Mat> overlay =
        {abemalpha[0], abemalpha[1], abemalpha[2]};
      cv::Mat overlayTmp, overlay32fc3;
      cv::merge(overlay, overlayTmp);
      overlayTmp.convertTo(overlay32fc3, CV_8UC3);

      cv::Mat alpha(cv::Size(temp.cols, temp.rows), CV_8UC3,
                    cv::Scalar(0, 0, 0));
      cv::warpAffine(overlay32fc3.mul(abealpha), alpha, mv, alpha.size(),
                     cv::INTER_LINEAR, cv::BORDER_TRANSPARENT);

      temp += alpha;

      // make face image
      int min_x = 9999;
      int min_y = 9999;
      int max_x = 0;
      int max_y = 0;
      for (int i = 0; i < shapes[0].num_parts(); i++) {
        const full_object_detection& d = shapes[0];
        x = d.part(i).x();
        y = d.part(i).y();
        if (x < min_x) min_x = x;
        if (y < min_y) min_y = y;
        if (x > max_x) max_x = x;
        if (y > max_y) max_y = y;
      }
      result_image = cv::Mat(temp, cv::Rect(min_x, min_y, max_x - min_x, max_y - min_y));
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
  // NODE_SET_METHOD(exports, "draw", Method);
}

NODE_MODULE(addon, init)
