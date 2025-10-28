#include <opencv2/opencv.hpp>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <vector>
#include "stdafx.h"
#include "motion_detector.h"

using namespace cv;
using namespace std;

vector<RotatedRect> readCoordinates(const string& yaml_file) {
    vector<RotatedRect> parking_spots;
    try {
        YAML::Node data = YAML::LoadFile(yaml_file);
        if (!data["parking_spots"]) {
            cout << "Key 'parking_spots' not found in the YAML file." << endl;
            return parking_spots;
        }

        for (const auto& spot : data["parking_spots"]) {
            if (spot.size() == 5) {
                Point2f center(spot[0].as<float>(), spot[1].as<float>());
                Size2f size(spot[2].as<float>(), spot[3].as<float>());
                float angle = spot[4].as<float>();
                parking_spots.push_back(RotatedRect(center, size, angle));
            }
            else {
                cout << "Malformed spot detected, skipping: " << spot << endl;
            }
        }
    }
    catch (const YAML::Exception& e) {
        cout << "Error reading YAML file: " << e.what() << endl;
    }
    return parking_spots;
}
vector<vector<Point>> detectContours(Mat& frame, const Mat& ref_frame) {
    resize(frame, frame, ref_frame.size());
    Mat frame_delta, frame_delta_gray, frame_delta_blur, thresh;

    absdiff(frame, ref_frame, frame_delta);
    cvtColor(frame_delta, frame_delta_gray, COLOR_BGR2GRAY);
    GaussianBlur(frame_delta_gray, frame_delta_blur, Size(7, 7), 0);
    cv::threshold(frame_delta_blur, thresh, 30, 255, THRESH_BINARY);

    // Morphological operations for noise reduction
    Mat kernel_open = getStructuringElement(MORPH_RECT, Size(15, 15));
    Mat kernel_close = getStructuringElement(MORPH_RECT, Size(19, 19));
    morphologyEx(thresh, thresh, MORPH_OPEN, kernel_open);
    morphologyEx(thresh, thresh, MORPH_CLOSE, kernel_close);

    vector<vector<Point>> contours;
    findContours(thresh, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    return contours;
}


void monitorParking(const string& video_path, const string& yaml_file) {
    VideoCapture cap(video_path);
    if (!cap.isOpened()) {
        cout << "Error: Cannot open video file." << endl;
        return;
    }

    vector<RotatedRect> parking_spots = readCoordinates(yaml_file);
    Mat ref_frame = imread("C:/Users/HP/Desktop/parking_lot_2.png");
    if (ref_frame.empty()) {
        cout << "Error: Cannot load reference image." << endl;
        return;
    }

    while (cap.isOpened()) {
        Mat frame;
        if (!cap.read(frame)) {
            break;
        }
        // Process the frame once for contours (blue ellipses)
        vector<vector<Point>> contours = detectContours(frame, ref_frame);

        for (const auto& c : contours) {
            if (contourArea(c) < 500) {
                continue;
            }
            RotatedRect ellipse_rect = fitEllipse(c);
            ellipse(frame, ellipse_rect, Scalar(255, 0, 0), 2);
        }

        // Check parking spots and draw green/red ellipses
        for (size_t i = 0; i < parking_spots.size(); ++i) {
            const auto& spot = parking_spots[i];
            bool occupied = false;
            for (const auto& c : contours) {
                if (pointPolygonTest(c, spot.center, false) >= 0) {
                    occupied = true;
                    break;
                }
            }
            Scalar color = occupied ? Scalar(0, 0, 255) : Scalar(0, 255, 0);
            ellipse(frame, spot, color, 2);
        }

        imshow("Parking Lot", frame);
        if (waitKey(30) == 'q') {
            break;
        }
    }
    cap.release();
    destroyAllWindows();
}
