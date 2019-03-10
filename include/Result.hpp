#ifndef RESULT_H
#define RESULT_H

/*
 *  Storage container for information necessary for final output.
 */

 #include <opencv2/opencv.hpp>
class Result {
  cv::Mat src_image;                                  // Starting image, unadulterated
  cv::Mat res_image;                                  // To be final image (after processResult())
  std::vector<cv::Rect> dieRegions;                   // A recentangle depecting where a die was detected

public:
  std::vector<std::vector<cv::KeyPoint> > diePips;    // A set of keypoints representing pips on die

  Result(cv::Mat image);                              // Constructor
  void setRegions(std::vector<cv::Rect> input);       // Setter for dieRegions since it is private
  std::vector<cv::Rect> getRegions();                 // Getter for dieRegions since it is private
  cv::Mat getImage();                                 // Getter for res_image since it is private
  void processResult();                               // Processing function for adapting final image to specifications
};

#endif
