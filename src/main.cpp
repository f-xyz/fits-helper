#include "Config.h"
#include "App.h"
#include "math/SharpnessEstimator.h"
#include <stacktrace>
#include <csignal>

void onSegfault(int signal) {
  std::println("Segmentation fault:");
  std::println("{}", std::stacktrace::current());
  std::signal(signal, SIG_DFL);
  std::raise(signal);
}

int main(const int argc, const char **argv) {
  setenv("QT_QPA_PLATFORM", "xcb", 1); // Fixes QT windows on Wayland
  std::signal(SIGSEGV, onSegfault);
  std::println("FITS Helper\n");

  //////////////////////////////////////

  Config config(argc, argv);
  SharpnessEstimatorGaussian estimator;
  App app(config, estimator);

  app.estimate();
  app.printSpark();
  app.percentiles();

  return 0;
}