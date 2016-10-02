// The contents of this file are in the public domain. See LICENSE_FOR_EXAMPLE_PROGRAMS.txt
/*

    This example program shows how to find frontal human faces in an image and
    estimate their pose.  The pose takes the form of 68 landmarks.  These are
    points on the face such as the corners of the mouth, along the eyebrows, on
    the eyes, and so forth.  
    

    This example is essentially just a version of the face_landmark_detection_ex.cpp
    example modified to use OpenCV's VideoCapture object to read from a camera instead 
    of files.


    Finally, note that the face detector is fastest when compiled with at least
    SSE2 instructions enabled.  So if you are using a PC with an Intel or AMD
    chip then you should enable at least SSE2 instructions.  If you are using
    cmake to compile this program you can enable them by using one of the
    following commands when you create the build project:
        cmake path_to_dlib_root/examples -DUSE_SSE2_INSTRUCTIONS=ON
        cmake path_to_dlib_root/examples -DUSE_SSE4_INSTRUCTIONS=ON
        cmake path_to_dlib_root/examples -DUSE_AVX_INSTRUCTIONS=ON
    This will set the appropriate compiler options for GCC, clang, Visual
    Studio, or the Intel compiler.  If you are using another compiler then you
    need to consult your compiler's manual to determine how to enable these
    instructions.  Note that AVX is the fastest but requires a CPU from at least
    2011.  SSE4 is the next fastest and is supported by most current machines.  
*/

#include <dlib/opencv.h>
#include <cv.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>

using namespace dlib;
using namespace std;

int main()
{
    double x, y, z, r;
    try
    {
        cv::VideoCapture cap(0);
        if (!cap.isOpened())
        {
            cerr << "Unable to connect to camera" << endl;
            return 1;
        }

        image_window win;

        // Load face detection and pose estimation models.
        frontal_face_detector detector = get_frontal_face_detector();
        shape_predictor pose_model;
        deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;

        cv::Mat abema_source = cv::imread("abema-nose.png", -1);

        // Grab and process frames until the main window is closed by the user.
        while(!win.is_closed())
        {
            // Grab a frame
            cv::Mat temp;
            cap >> temp;
            cv::flip(temp, temp, 1); // Mirror
            // Turn OpenCV's Mat into something dlib can deal with.  Note that this just
            // wraps the Mat object, it doesn't copy anything.  So cimg is only valid as
            // long as temp is valid.  Also don't do anything to temp that would cause it
            // to reallocate the memory which stores the image as that will make cimg
            // contain dangling pointers.  This basically means you shouldn't modify temp
            // while using cimg.
            cv_image<bgr_pixel> cimg(temp);

            // Detect faces 
            std::vector<rectangle> faces = detector(cimg);
            // Find the pose of each face.
            std::vector<full_object_detection> shapes;
            for (unsigned long i = 0; i < faces.size(); ++i)
                shapes.push_back(pose_model(cimg, faces[i]));

            if (shapes.size() == 0) continue;

            // Display it all on the screen
            win.clear_overlay();
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

            cv_image<bgr_pixel> dimg(temp + alpha);
            win.set_image(dimg);
            win.add_overlay(render_face_detections(shapes));
        }
    }
    catch(serialization_error& e)
    {
        cout << "You need dlib's default face landmarking model file to run this example." << endl;
        cout << "You can get it from the following URL: " << endl;
        cout << "   http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2" << endl;
        cout << endl << e.what() << endl;
    }
    catch(exception& e)
    {
        cout << e.what() << endl;
    }
}

