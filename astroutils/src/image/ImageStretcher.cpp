#include <astroutils/image/ImageStretcher.hpp>
#include <astroutils/image/image.hpp>
#include <opencv2/photo.hpp>

namespace astroutils::image {

cv::Mat ImageStretcher::stretch(const cv::Mat &image) {
  if (image.empty()) {
    return {};
  }

  cv::Mat color;
  if (image.channels() == 1) {
    cv::Mat raw;
    image.convertTo(raw, CV_16U);
    cv::cvtColor(raw, color, cv::COLOR_BayerRG2BGR_EA);
  } else {
    color = image;
  }

  cv::Mat input;
  color.convertTo(input, CV_32F);

  cv::Mat norm = getNormalizedLab(input);
  std::vector<cv::Mat> channels = astroutils::image::split(norm);
  cv::Mat lightness = getLightness(channels[0]);

  for (const auto &type : options.types) {
    switch (type) {
    case ImageStretcherOptions::Type::CLAHE:
      channels[0] = stretchClahe(channels[0], options.claheClipLimit, options.claheTileSize);
      break;
    case ImageStretcherOptions::Type::Asinh:
      channels[0] = stretchAsinh(channels[0], options.asinhFactor);
      break;
    case ImageStretcherOptions::Type::Histogram:
      using namespace astroutils::image;
      cv::Mat norm = normalize(channels[0]);
      auto [min, max] = soft_range(norm, options.histogramTopBins);
      channels[0] = clamp(norm, min, max);
      break;
    }
  }

  if (options.scaleChroma) {
    scaleChroma(channels, lightness);
  }

  if (options.denoiseH > 0) {
    cv::fastNlMeansDenoising(channels[0], channels[0], options.denoiseH, 7, 21);
  }

  cv::Mat result;
  cv::merge(channels, result);
  cv::cvtColor(result, result, cv::COLOR_Lab2BGR);

  return result;
}

cv::Mat ImageStretcher::getNormalizedLab(const cv::Mat &image) {
  cv::Mat result;
  cv::normalize(image, result, 0, 255, cv::NORM_MINMAX, CV_8U);
  cv::cvtColor(result, result, cv::COLOR_BGR2Lab);
  return result;
}

cv::Mat ImageStretcher::getLightness(const cv::Mat &lightness) {
  cv::Mat result = lightness.clone();
  result.convertTo(result, CV_32F);
  cv::max(result, 1.0, result);
  return result;
}

cv::Mat ImageStretcher::stretchClahe(cv::Mat &image, double clipLimit, int tileSize) {
  cv::Mat result;
  const auto clahe = cv::createCLAHE(clipLimit, cv::Size(tileSize, tileSize));
  clahe->apply(image, result);
  return result;
}

cv::Mat ImageStretcher::stretchAsinh(cv::Mat &image, float factor) {
  cv::Mat result;

  image.convertTo(result, CV_32F);

  double min, max;
  cv::minMaxLoc(result, &min, &max);
  result -= min;

  result.forEach<float>([factor](float &pixel, const int *) {
    pixel = std::asinh(factor * pixel) / std::asinh(factor);
  });

  cv::normalize(result, result, 0, 255, cv::NORM_MINMAX, CV_32F);
  result.convertTo(result, CV_8U);

  return result;
}

void ImageStretcher::scaleChroma(std::vector<cv::Mat> &channels, const cv::Mat &lightness) {
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

} // namespace astroutils::image
