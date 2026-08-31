#include "image/ImageStretcher.hpp"
#include "image/image.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>

namespace utils::image {

cv::Mat ImageStretcher::stretch(const ImageStretcherOptions &options) {
  cv::normalize(image, image, 0, 255, cv::NORM_MINMAX, CV_8U);
  cv::cvtColor(image, image, cv::COLOR_BGR2Lab);

  auto channels = utils::image::split(image);
  auto lightness = getLightness(channels[0]);

  if (options.type == ImageStretcherOptions::Type::CLAHE) {
    stretchClahe(channels[0], options.claheClipLimit, options.claheTileSize);
  } else if (options.type == ImageStretcherOptions::Type::Asinh) {
    stretchAsinh(channels[0], options.asinhFactor);
  } else {
    using namespace utils::image;
    auto norm = normalize(image);
    auto gray = lightness(norm);
    auto [min, max] = soft_range(gray, options.histogramTopBins);
    auto clamped = clamp(gray, min, max);
    auto stretched =
        clahe(clamped, options.claheClipLimit, options.claheTileSize);
  }

  scaleChroma(channels, lightness);

  if (options.denoiseH > 0) {
    cv::fastNlMeansDenoising(channels[0], channels[0], options.denoiseH, 7, 21);
  }

  cv::merge(channels, image);
  cv::cvtColor(image, image, cv::COLOR_Lab2BGR);

  return image;
}

std::vector<cv::Mat> ImageStretcher::split(const cv::Mat &image) {
  std::vector<cv::Mat> channels;
  cv::split(image, channels);
  return channels;
}

cv::Mat ImageStretcher::getLightness(const cv::Mat &lightness) {
  cv::Mat result = lightness.clone();
  result.convertTo(result, CV_32F);
  cv::max(result, 1.0, result);
  return result;
}

void ImageStretcher::stretchClahe(cv::Mat &image, double clipLimit,
                                  int tileSize) {
  auto clahe = cv::createCLAHE(clipLimit, cv::Size(tileSize, tileSize));
  clahe->apply(image, image);
}

void ImageStretcher::stretchAsinh(cv::Mat &image, float factor) {
  image.convertTo(image, CV_32F);

  double min, max;
  cv::minMaxLoc(image, &min, &max);
  image -= min;

  image.forEach<float>([factor](float &pixel, const int *) {
    pixel = std::asinh(factor * pixel) / std::asinh(factor);
  });

  cv::normalize(image, image, 0, 255, cv::NORM_MINMAX, CV_32F);
}

void ImageStretcher::scaleChroma(std::vector<cv::Mat> &channels,
                                 const cv::Mat &lightness) {
  for (int i = 0; i < 3; ++i) {
    channels[i].convertTo(channels[i], CV_32F);
    if (i > 0) {
      channels[i] -= 128.0;
    }
  }

  cv::Mat gain;
  cv::divide(channels[0], lightness, gain);
  cv::multiply(channels[1], gain, channels[1]);
  cv::multiply(channels[2], gain, channels[2]);

  for (int i = 0; i < 3; ++i) {
    if (i > 0) {
      channels[i] += 128.0;
    }
    channels[i].convertTo(channels[i], CV_8U);
  }
}

} // namespace utils::image