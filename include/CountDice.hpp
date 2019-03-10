#ifndef COUNTDICE_H
#define COUNTDICE_H

#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;

const bool DEBUG = false;
const bool LOG = false;

const size_t DICE_AREA_MIN = 19600;
const size_t DICE_AREA_MAX = 42600;
const double ALLOWABLE_ERROR_RATE = 0.1;
const size_t NUM_TEST_FILES = 6;

const char TEST_FILES[NUM_TEST_FILES][26] = {
  "./img/dice1.png",
  "./img/dice2.png",
  "./img/dice3.png",
  "./img/dice4.png",
  "./img/dice5.png",
  "./img/dice6.png"
};

void displayImage(char* path);
void displayImage(Mat image, const char* windowName = "Display Image");

std::vector<Rect> processImage(Mat image);
std::vector<KeyPoint> processDieImage(Mat image);
 #endif
