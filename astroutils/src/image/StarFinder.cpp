#include <algorithm>
#include <astroutils/image/StarFinder.hpp>
#include <cmath>
#include <opencv2/imgproc.hpp>

namespace astroutils::stars {

cv::Mat StarFinder::preprocessImage(const cv::Mat &image) const {
  cv::Mat gray;
  if (image.channels() == 3 || image.channels() == 4) {
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
  } else {
    gray = image;
  }

  cv::Mat floatImg;
  gray.convertTo(floatImg, CV_32F);
  return floatImg;
}

cv::Mat StarFinder::createDetectionMask(const cv::Mat &floatImg,
                                        double &outBackground) const {
  cv::Scalar meanVal, stdDevVal;
  cv::meanStdDev(floatImg, meanVal, stdDevVal);

  outBackground = meanVal[0];
  double sigma = stdDevVal[0];
  double threshold = outBackground + options_.thresholdSigma * sigma;

  cv::Mat mask;
  cv::threshold(floatImg, mask, threshold, 255, cv::THRESH_BINARY);
  mask.convertTo(mask, CV_8U);
  return mask;
}

std::optional<Star> StarFinder::processComponent(const cv::Mat &floatImg,
                                                 double background,
                                                 const cv::Mat &stats,
                                                 int componentId) const {
  int area = stats.at<int>(componentId, cv::CC_STAT_AREA);
  if (area < options_.minArea || area > options_.maxArea) {
    return std::nullopt;
  }

  int left = stats.at<int>(componentId, cv::CC_STAT_LEFT);
  int top = stats.at<int>(componentId, cv::CC_STAT_TOP);
  int width = stats.at<int>(componentId, cv::CC_STAT_WIDTH);
  int height = stats.at<int>(componentId, cv::CC_STAT_HEIGHT);

  int margin = options_.starMargin;
  if (left < margin || top < margin ||
      (left + width) > (floatImg.cols - margin) ||
      (top + height) > (floatImg.rows - margin)) {
    return std::nullopt;
  }

  int pad = 3;
  int roiX = std::max(0, left - pad);
  int roiY = std::max(0, top - pad);
  int roiW = std::min(floatImg.cols - roiX, width + 2 * pad);
  int roiH = std::min(floatImg.rows - roiY, height + 2 * pad);

  cv::Rect roiRect(roiX, roiY, roiW, roiH);
  cv::Mat roi = floatImg(roiRect);

  cv::Mat roiSub = roi - background;
  cv::threshold(roiSub, roiSub, 0, 0, cv::THRESH_TOZERO);

  cv::Moments m = cv::moments(roiSub, false);
  if (m.m00 <= 0.0) {
    return std::nullopt;
  }

  double cx = roiX + m.m10 / m.m00;
  double cy = roiY + m.m01 / m.m00;

  double mu20 = m.mu20 / m.m00;
  double mu02 = m.mu02 / m.m00;
  double mu11 = m.mu11 / m.m00;

  double diff = mu20 - mu02;
  double common = std::sqrt(diff * diff + 4.0 * mu11 * mu11);
  double lambda1 = (mu20 + mu02 + common) / 2.0;
  double lambda2 = (mu20 + mu02 - common) / 2.0;

  if (lambda1 <= 0.0 || lambda2 <= 0.0) {
    return std::nullopt;
  }

  double semiMajor = std::sqrt(std::max(0.0, lambda1));
  double semiMinor = std::sqrt(std::max(0.0, lambda2));

  constexpr double SIGMA_TO_FWHM = 2.3548200450309493;
  double fwhmMajor = semiMajor * SIGMA_TO_FWHM;
  double fwhmMinor = semiMinor * SIGMA_TO_FWHM;
  double fwhm = (fwhmMajor + fwhmMinor) / 2.0;

  double eccentricity = 0.0;
  if (semiMajor > 0.0) {
    double ratio = semiMinor / semiMajor;
    eccentricity = std::sqrt(std::max(0.0, 1.0 - ratio * ratio));
  }

  double orientation = 0.5 * std::atan2(2.0 * mu11, diff);

  double maxVal = 0.0;
  cv::minMaxLoc(roi, nullptr, &maxVal);

  Star star;
  star.center = cv::Point2f(static_cast<float>(cx), static_cast<float>(cy));
  star.fwhm = fwhm;
  star.eccentricity = eccentricity;
  star.fwhmX = fwhmMajor;
  star.fwhmY = fwhmMinor;
  star.flux = m.m00;
  star.peak = maxVal;
  star.semiMajor = semiMajor;
  star.semiMinor = semiMinor;
  star.orientation = orientation;
  star.bbox = cv::Rect(left, top, width, height);

  return star;
}

void StarFinder::limitStars(std::vector<Star> &stars) const {
  if (options_.maxStars > 0 &&
      stars.size() > static_cast<size_t>(options_.maxStars)) {
    std::partial_sort(
        stars.begin(), stars.begin() + options_.maxStars, stars.end(),
        [](const Star &a, const Star &b) { return a.flux > b.flux; });
    stars.resize(options_.maxStars);
  }
}

std::vector<Star> StarFinder::findStars(const cv::Mat &image) const {
  if (image.empty()) {
    return {};
  }

  cv::Mat floatImg = preprocessImage(image);

  double background = 0.0;
  cv::Mat mask = createDetectionMask(floatImg, background);

  cv::Mat labels, stats, centroids;
  int numComponents = cv::connectedComponentsWithStats(mask, labels, stats,
                                                       centroids, 8, CV_32S);

  std::vector<Star> stars;
  stars.reserve(numComponents);

  for (int i = 1; i < numComponents; ++i) { // Skip background component 0
    auto starOpt = processComponent(floatImg, background, stats, i);
    if (starOpt.has_value()) {
      stars.push_back(*starOpt);
    }
  }

  limitStars(stars);
  return stars;
}

} // namespace astroutils::stars
