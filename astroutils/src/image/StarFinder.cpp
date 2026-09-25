////////////////////////////////////////
// GENERATED ///////////////////////////
////////////////////////////////////////

#include <algorithm>
#include <astroutils/image/StarFinder.hpp>
#include <cmath>
#include <opencv2/imgproc.hpp>

namespace astroutils::stars {

cv::Mat StarFinder::preprocessImage(const cv::Mat &image) const {
  cv::Mat gray;
  // Detection and shape measurements use one intensity value per pixel. OpenCV
  // uses BGR ordering here; the alpha channel (when present) is ignored.
  if (image.channels() == 3 || image.channels() == 4) {
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
  } else {
    gray = image;
  }

  // Floating point keeps subsequent background subtraction and moment
  // calculations independent of the source image's integer bit depth.
  cv::Mat floatImg;
  gray.convertTo(floatImg, CV_32F);
  return floatImg;
}

cv::Mat StarFinder::createDetectionMask(const cv::Mat &floatImg,
                                        double &outBackground) const {
  cv::Scalar meanVal, stdDevVal;
  cv::meanStdDev(floatImg, meanVal, stdDevVal);

  // This is a global detection threshold: pixels sufficiently above the image
  // mean are grouped into candidate objects. The mean is also used later as
  // the baseline to subtract when measuring each candidate's shape and flux.
  outBackground = meanVal[0];
  double sigma = stdDevVal[0];
  double threshold = outBackground + options_.thresholdSigma * sigma;

  cv::Mat mask;
  cv::threshold(floatImg, mask, threshold, 255, cv::THRESH_BINARY);
  // Connected-components expects an 8-bit binary mask; thresholding a float
  // image above produces a float mask, so convert its zero/nonzero values.
  mask.convertTo(mask, CV_8U);
  return mask;
}

std::optional<Star> StarFinder::processComponent(const cv::Mat &floatImg,
                                                 double background,
                                                 const cv::Mat &stats,
                                                 int componentId) const {
  // Reject candidates by their pixel count before doing any more expensive
  // photometry. Component 0 is the background and is skipped by findStars().
  int area = stats.at<int>(componentId, cv::CC_STAT_AREA);
  if (area < options_.minArea || area > options_.maxArea) {
    return std::nullopt;
  }

  int left = stats.at<int>(componentId, cv::CC_STAT_LEFT);
  int top = stats.at<int>(componentId, cv::CC_STAT_TOP);
  int width = stats.at<int>(componentId, cv::CC_STAT_WIDTH);
  int height = stats.at<int>(componentId, cv::CC_STAT_HEIGHT);

  // Sources touching the configured image border margin are often truncated,
  // so their centers, fluxes, and shapes cannot be measured reliably.
  int margin = options_.starMargin;
  if (left < margin || top < margin ||
      (left + width) > (floatImg.cols - margin) ||
      (top + height) > (floatImg.rows - margin)) {
    return std::nullopt;
  }

  // The thresholded component box can omit the faint wings of a star. Include
  // a small surrounding region for centroid and second-moment calculations.
  int pad = 3;
  int roiX = std::max(0, left - pad);
  int roiY = std::max(0, top - pad);
  int roiW = std::min(floatImg.cols - roiX, width + 2 * pad);
  int roiH = std::min(floatImg.rows - roiY, height + 2 * pad);

  cv::Rect roiRect(roiX, roiY, roiW, roiH);
  cv::Mat roi = floatImg(roiRect);

  // Use only positive signal above the estimated background as moment weights;
  // darker pixels should not pull the centroid or shape away from the source.
  cv::Mat roiSub = roi - background;
  cv::threshold(roiSub, roiSub, 0, 0, cv::THRESH_TOZERO);

  // Spatial moments give the intensity-weighted centroid and covariance of the
  // source. A zero total weight means there is no usable signal in this ROI.
  cv::Moments m = cv::moments(roiSub, false);
  if (m.m00 <= 0.0) {
    return std::nullopt;
  }

  double cx = roiX + m.m10 / m.m00;
  double cy = roiY + m.m01 / m.m00;

  double mu20 = m.mu20 / m.m00;
  double mu02 = m.mu02 / m.m00;
  double mu11 = m.mu11 / m.m00;

  // The eigenvalues of the 2x2 central-moment covariance matrix are the
  // variances along the source's principal axes. They define its orientation
  // and widths independently of the image x/y axes.
  double diff = mu20 - mu02;
  double common = std::sqrt(diff * diff + 4.0 * mu11 * mu11);
  double lambda1 = (mu20 + mu02 + common) / 2.0;
  double lambda2 = (mu20 + mu02 - common) / 2.0;

  if (lambda1 <= 0.0 || lambda2 <= 0.0) {
    return std::nullopt;
  }

  double semiMajor = std::sqrt(std::max(0.0, lambda1));
  double semiMinor = std::sqrt(std::max(0.0, lambda2));

  // For a Gaussian profile, FWHM = sigma * sqrt(8 ln 2). Report the mean of
  // major/minor FWHM as the single scalar size, retaining each axis as well.
  constexpr double SIGMA_TO_FWHM = 2.3548200450309493;
  double fwhmMajor = semiMajor * SIGMA_TO_FWHM;
  double fwhmMinor = semiMinor * SIGMA_TO_FWHM;
  double fwhm = (fwhmMajor + fwhmMinor) / 2.0;

  double eccentricity = 0.0;
  if (semiMajor > 0.0) {
    // This is zero for a round profile and approaches one as the minor axis
    // becomes small relative to the major axis.
    double ratio = semiMinor / semiMajor;
    eccentricity = std::sqrt(std::max(0.0, 1.0 - ratio * ratio));
  }

  double orientation = 0.5 * std::atan2(2.0 * mu11, diff);

  double maxVal = 0.0;
  // Peak is the brightest original pixel in the padded ROI, while flux below
  // is the summed background-subtracted weight used for the moments.
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
    // Only the brightest requested number are needed. partial_sort orders that
    // prefix by flux without fully sorting candidates that will be discarded.
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

  // Pipeline: convert to detection intensities, threshold, label adjacent
  // foreground pixels, measure plausible components, and optionally cap count.
  cv::Mat floatImg = preprocessImage(image);

  double background = 0.0;
  cv::Mat mask = createDetectionMask(floatImg, background);

  cv::Mat labels, stats, centroids;
  // Eight-connectivity treats diagonal touching pixels as one candidate.
  // OpenCV returns one row of stats per component, including background row 0.
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
