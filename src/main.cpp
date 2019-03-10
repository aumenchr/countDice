#include <stdio.h>
#include <stdexcept>
#include <sstream>
#include <vector>
#include "../include/Result.hpp"
#include "../include/CountDice.hpp"
#include <opencv2/opencv.hpp>

using namespace cv;

int main (int argc, char** argv) {
  // Get images, either from prestored files, or argument paths
  /*
   *  Note:: No specifications were provided as to what input parameters would be.
   *    I chose to allow no parameters to process the test images provided;
   *      but anything else passed should be an absolute path to an image.
   */
  std::vector<Mat> images;
  if (argc >= 2) {
    /*
     *    VALIDATE ARGUMENTS!!!                                                 TODO
     */
    for (size_t i = 1; i < argc; i++) {
      Mat image;
      image = imread(argv[i], IMREAD_COLOR);
      images.push_back(image);
      if (LOG) printf("Added %s to processing queue.\n", argv[i]);
    }
  } else {
    /*
     *    VALIDATE TEST FILES EXIST!!!                                          TODO
     */
    for (size_t i = 0; i < NUM_TEST_FILES; i++) {
      Mat image;
      image = imread(TEST_FILES[i], IMREAD_COLOR);
      images.push_back(image);
      if (LOG) printf("Added %s to processing queue.\n", TEST_FILES[i]);
    }
  }

  // Process the images
  std::vector<Result*> results;
  for (size_t i = 0; i < images.size(); i++) {
    if (LOG) printf("Processing image %i\n", i);
    try {
      Result* aResult = new Result(images[i]);
      aResult->setRegions(processImage(images[i])); // Find dice
      for (size_t j = 0; j < aResult->getRegions().size(); j++) {
        aResult->diePips.push_back(processDieImage((images[i])(aResult->getRegions()[j]))); // Find pips
      }
      results.push_back(aResult);
    } catch (std::exception& e) {
      printf("%s\n", e.what());
      return -1;
    }
    if (LOG) printf("Finished image %i\n", i);
  }

  // Create the finished products
  size_t fileNum = 0;
  for (std::vector<Result*>::iterator it = results.begin(); it != results.end(); it++){
    (*it)->processResult(); // Make modifications to final image
    Mat finalProduct = (*it)->getImage(); // Get the image
    displayImage(finalProduct, "Detected Dice"); // Display it via imshow()       REQUIREMENT 1
    std::stringstream path;
    path << "output_file_" << fileNum++ << ".png";
    imwrite((const std::string)path.str(), finalProduct); // Write image to file  REQUIREMENT 2
  }

  // Cleanup memory
  while (!results.empty()) {
    delete results[0];
    results.erase(results.begin());
  }
  return 0;
}

std::vector<Rect> processImage(Mat image) {
  if (!image.data) {
    throw std::runtime_error("processImage() was passed an image with no data.");
  }
  if (DEBUG) displayImage(image, "Source");
  Mat gray;
  cvtColor(image, gray, COLOR_BGR2GRAY);
  if (DEBUG) displayImage(gray, "Grayscale");

  Mat bw;
  threshold(gray, bw, 225, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
  if (DEBUG) displayImage(bw, "Black and White");

  Mat edges;
  Canny(bw, edges, 2, 2*3, 3, false);
  if (DEBUG) displayImage(edges, "Edge Detection");

  int size = 2;
  Mat element = getStructuringElement(MORPH_ELLIPSE, Size(2*size+1, 2*size+1), Point(size, size));
  Mat dilated;
  dilate(edges, dilated, element);
  if (DEBUG) displayImage(dilated, "Bolder Edges");

  std::vector<std::vector<Point> > dC;
  std::vector<Vec4i> dH;
  findContours(dilated, dC, dH, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
  if (DEBUG) displayImage(dilated, "Contours");

  if (LOG) printf("contours(%u) hierarchy(%u) [", dC.size(), dH.size());
  std::vector<Rect> regions;
  for (size_t dI = 0; dI < dC.size(); dI++) {
    Rect diceBounds = boundingRect( Mat(dC[dI]));
    int area = diceBounds.area();
    if (area > DICE_AREA_MIN and area < DICE_AREA_MAX) {
      if (std::abs(diceBounds.height - diceBounds.width)/((diceBounds.height + diceBounds.width)/2) < ALLOWABLE_ERROR_RATE) {
        if (LOG) printf(" (%i, %i)(%i x %i)", area, diceBounds.area(), diceBounds.height, diceBounds.width);
        regions.push_back(diceBounds);
      }
    }
  }
  if (LOG) printf(" ]\n");
  return regions;
}

// Expects an image with a single die, outputs a set of keypoints denoting the pips.
std::vector<KeyPoint> processDieImage(Mat image) {
  if (!image.data) {
    throw std::runtime_error("processDieImage() was passed an image with no data.");
  }
  if (DEBUG) displayImage(image, "Die Source");

  Mat gray;
  cvtColor(image, gray, COLOR_BGR2GRAY);
  if (DEBUG) displayImage(gray, "Grayscale");

  Mat bw;
  threshold(gray, bw, 215, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
  if (DEBUG) displayImage(bw);

  int size = 2;
  Mat element = getStructuringElement(MORPH_ELLIPSE, Size(2*size+1, 2*size+1), Point(size, size));
  Mat eroded;
  erode(bw, eroded, element);
  if (DEBUG) displayImage(eroded);

  SimpleBlobDetector::Params p;
  p.filterByInertia = true;
  p.minInertiaRatio = 0.5;
  std::vector<KeyPoint> keypoints;
  SimpleBlobDetector blobDetector(p);
  blobDetector.detect(eroded, keypoints);
  return keypoints;
}

// Displays the image provided in a window titled as directed.
void displayImage(Mat image, const char* windowName) {
  if (!image.data) {
    printf("No image data \n");
    return;
  }
  namedWindow(windowName, WINDOW_AUTOSIZE);
  imshow(windowName, image);
  waitKey(0);
}

// Unused function to display an image from a provided path; used during testing.
void displayImage(char* path) {
    Mat image;
    image = imread(path, 1);
    displayImage(image);
}
