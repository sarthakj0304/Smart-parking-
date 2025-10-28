
#  Smart Parking System

An intelligent parking management system built using **Python** and **OpenCV** to monitor parking lot occupancy in **real-time**. This system processes video feeds to detect available and occupied slots, providing an efficient solution for parking automation and management.

## Features

  * **Real-Time Occupancy Detection:** Analyzes **pre-recorded video** frames to determine the current status of each parking slot.
  * **Two-Step Setup:** Requires a **manual coordinate marking step** before detection can begin.
  * **Contour Detection:** Works by analyzing **frame differences** to detect movement (contours) within defined parking areas.
  * **Visual Output:** Slots turn **red** when occupied and **green** when free.
  * **Performance:** Achieved a **15% improvement** in parking space detection accuracy through optimized coordinate integration and analysis.

## 💻 Tech Stack

| Category | Technology |
| :--- | :--- |
| **Core Language** | C++ |
| **Computer Vision** | OpenCV |

-----

## ⚙️ Installation and Running

### 1\. Prerequisites

You must have C++ installed.



### 2\. Step-by-Step Execution

The system requires two distinct steps to run the analysis on a pre-made video:

#### **Step A: Mark Coordinates (One-Time Setup)**

This script must be run first to manually define and save the boundaries of all parking slots.

```bash
python coordinates_mark.cpp
```

  * **Action:** Use your mouse to mark the corners of each parking space on a sample frame.
  * **Result:** A configuration file containing the exact coordinates is saved for use in the next step.

#### **Step B: Run Video Feed and Get Output**

This script loads the coordinates and runs the real-time detection on your video file.



  * **Action:** The video will start processing.
  * **Result:** The output window displays the video with visual feedback (green/red boxes) showing occupancy status.

-----

##  How it Works

The system operates using background subtraction and contour analysis for reliable detection of movement and occupancy:

1.  **Configuration Load:** The system loads the **manually marked coordinates** from the configuration file.
2.  **Frame Analysis:** The `feed` script analyzes the video frame by frame.
3.  **Contour Detection:** The primary mechanism involves taking the difference between the current frame and a pre-defined background frame. **Contour detection** is then applied to these differences to isolate parts of the image that have moved (i.e., a car entering or leaving).
4.  **Occupancy Determination:** For each defined slot coordinate:
      * The system checks the area inside the slot boundaries for significant movement/contours.
      * If the contour area exceeds a set threshold, the slot is marked as **Occupied** (and turns **red**).
      * Otherwise, the slot remains **Free** (and stays **green**).

-----

##  License

This project is intended for educational and non-commercial use. All rights reserved.
