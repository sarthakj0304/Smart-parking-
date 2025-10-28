#include <opencv2/opencv.hpp>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <vector>
#include <fstream>
#include "stdafx.h"
#include "motion_detector.h"

using namespace cv;
using namespace std;

vector<RotatedRect> parking_spots;
vector<Point> coordinates;
Mat image, image_copy;
string window_name = "Parking areas selector";
int click_count = 0;

// Mouse callback function to handle on-click events
void onMouse(int event, int x, int y, int, void*) {
    if (event == EVENT_LBUTTONDOWN) {
        coordinates.push_back(Point(x, y));
        click_count++;

        // Draw a circle at the clicked point
        circle(image_copy, Point(x, y), 3, Scalar(0, 0, 255), -1);

        if (click_count == 5) { // Requires 5 points for an ellipse
            RotatedRect ellipse_rect = fitEllipse(coordinates);
            ellipse(image_copy, ellipse_rect, Scalar(0, 255, 0), 1);
            parking_spots.push_back(ellipse_rect);
            coordinates.clear();
            click_count = 0;
        }
        imshow(window_name, image_copy);
    }
}

// Function to display the bounded area in each parking spot
void updateDisplay(Mat& image) {
    for (size_t i = 0; i < parking_spots.size(); i++) {
        const auto& spot = parking_spots[i];
        ellipse(image, spot, Scalar(255, 0, 0), 1);

        // Place text at the center of each parking spot
        putText(image, "Spot " + to_string(i + 1), spot.center, FONT_HERSHEY_COMPLEX, 0.5, Scalar(255, 255, 255), 1);
    }
}

// Function to save parking spots to YAML file
void saveToYAML(const vector<RotatedRect>& parking_spots, const string& yaml_file = "parking_spots.yaml") {
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "parking_spots" << YAML::Value << YAML::BeginSeq;

    for (const auto& spot : parking_spots) {
        out << YAML::Flow << YAML::BeginSeq << spot.center.x << spot.center.y << spot.size.width << spot.size.height << spot.angle << YAML::EndSeq;
    }

    out << YAML::EndSeq << YAML::EndMap;

    ofstream fout(yaml_file);
    if (!fout.is_open()) {
        cerr << "Error: Unable to open file for writing: " << yaml_file << endl;
        return;
    }
    fout << out.c_str();
    fout.close();
}

int main() {
    string image_path = "C:/Users/HP/Desktop/parking_lot_2.png";
    image = imread(image_path);

    if (image.empty()) {
        cout << "Error loading the image" << endl;
        return -1;
    }

    image_copy = image.clone();
    namedWindow(window_name);
    setMouseCallback(window_name, onMouse);

    while (true) {
        imshow(window_name, image_copy);
        char key = (char)waitKey(10);

        if (key == 'r') {
            image_copy = image.clone();
            coordinates.clear();
            parking_spots.clear();
            cout << "Reset" << endl;
        }
        else if (key == 's') {
            cout << "Saved" << endl;
            saveToYAML(parking_spots);
            imwrite("Parking_spots_marked.png", image_copy);
            break;
        }

        else if (key == 'q') {
            cout << "Quit" << endl;
            break;
        }
    }

    destroyAllWindows();
    monitorParking("C:/Users/HP/Desktop/parking_lot_2.mp4", "parking_spots.yaml");
    return 0;
}