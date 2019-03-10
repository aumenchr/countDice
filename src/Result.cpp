#include "../include/Result.hpp"
#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace cv;

Result::Result(Mat image) {
  src_image = image;
  res_image = image;
}

void Result::setRegions(std::vector<Rect> input) {
  dieRegions = input;
}

std::vector<Rect> Result::getRegions() {
  return dieRegions;
}

Mat Result::getImage() {
  return res_image;
}

void Result::processResult() {
  // Variables for final answers
  size_t dice = dieRegions.size();
  size_t pips = 0;

  // VALIDATE EXPECTATIONS
  assert(dice == diePips.size());

  // Iterate over each detected die
  for (size_t die = 0; die < dice; die++) {
    // Draw a detection box
    rectangle(res_image, dieRegions[die], Scalar(0, 255, 0), 1, 16);
    // Identify number of pips and label accordingly
    size_t myPips = diePips[die].size();
    pips += myPips;
    std::stringstream text;
    text << myPips;
    Point text_loc = dieRegions[die].br();
    text_loc.x -= 12; // Should guarantee we don't draw out of window.
    putText(res_image, (const std::string)text.str(), dieRegions[die].br(), 0, 1, Scalar(0, 0, 0), 2);
  }
  // Identify total number of pips and label top-left corner of image accordingly
  std::stringstream text;
  text << "Total pips: " << pips;
  putText(res_image, (const std::string)text.str(), Point(10, 50), 0, 1, Scalar(0, 0, 0), 2);
}
