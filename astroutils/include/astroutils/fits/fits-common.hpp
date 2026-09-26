#pragma once

#include <fitsio.h>
#include <utility>

namespace astroutils::fits {

struct FitsImageParams {
  int bitsPerPixel = 0;
  int nDimensions = 0;
  long dimensions[2] = {0, 0};
  char bayerPattern[FLEN_VALUE] = {0};
};

struct FitsDataType {
  int cvType;
  int fitsType;
};

class FitsFilePtr {
public:
  fitsfile *ptr = nullptr;

  explicit FitsFilePtr(fitsfile *fptr = nullptr) : ptr(fptr) {}

  FitsFilePtr(const FitsFilePtr &) = delete;
  FitsFilePtr &operator=(const FitsFilePtr &) = delete;

  FitsFilePtr(FitsFilePtr &&that) noexcept
      : ptr(std::exchange(that.ptr, nullptr)) {}

  FitsFilePtr &operator=(FitsFilePtr &&that) noexcept {
    if (this != &that) {
      close();
      ptr = std::exchange(that.ptr, nullptr);
    }
    return *this;
  }

  ~FitsFilePtr() { close(); }

private:
  void close() {
    if (ptr) {
      int status = 0;
      fits_close_file(ptr, &status);
    }
  }
};

} // namespace astroutils::fits