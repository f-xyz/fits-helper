#pragma once

#include <astroutils/fs.hpp>
#include <string>
#include <string_view>

void createFitsFile(std::string_view path) {
  int status = 0;
  fitsfile *fptr = nullptr;
  const std::string ownedPath(path);
  fits_create_file(&fptr, ownedPath.c_str(), &status);

  long naxes[] = {2, 2};
  fits_create_img(fptr, USHORT_IMG, 2, naxes, &status);

  long firstpix[] = {1, 1};
  std::vector<unsigned short> pixels = {1, 2, 3, 4};
  const auto size = pixels.size();
  const auto data = pixels.data();
  fits_write_pix(fptr, TUSHORT, firstpix, size, data, &status);
  fits_close_file(fptr, &status);
}

std::vector<std::byte> createMemoryFitsFile() {
  const auto path = astroutils::fs::tmpFile();
  createFitsFile(path);

  auto bytes = astroutils::fs::readFile(path);
  std::filesystem::remove(path);

  return bytes;
}

// Helper: create a FITS file with a specific pixel type
template <typename T>
void createFitsFileTyped(std::string_view path, int fitsImgType,
                         int fitsIoType, cv::Size size,
                         std::vector<T> pixels) {
  int status = 0;
  fitsfile *fptr = nullptr;
  const std::string ownedPath(path);
  fits_create_file(&fptr, ownedPath.c_str(), &status);
  long naxes[] = {size.width, size.height};
  fits_create_img(fptr, fitsImgType, 2, naxes, &status);
  long firstpix[] = {1, 1};
  fits_write_pix(fptr, fitsIoType, firstpix, (long)pixels.size(), pixels.data(), &status);
  fits_close_file(fptr, &status);
}

// Helper: create a FITS file with BAYERPAT keyword
void createBayerFitsFile(std::string_view path, std::string_view bayerPat) {
  int status = 0;
  fitsfile *fptr = nullptr;
  const std::string ownedPath(path);
  const std::string ownedBayerPat(bayerPat);
  fits_create_file(&fptr, ownedPath.c_str(), &status);
  long naxes[] = {4, 4};
  fits_create_img(fptr, USHORT_IMG, 2, naxes, &status);
  fits_write_key(fptr, TSTRING, "BAYERPAT",
                 const_cast<char *>(ownedBayerPat.c_str()), nullptr, &status);
  long firstpix[] = {1, 1};
  std::vector<unsigned short> pixels(16, 128);
  fits_write_pix(fptr, TUSHORT, firstpix, 16, pixels.data(), &status);
  fits_close_file(fptr, &status);
}

// Helper: create a FITS file with a 0-axis primary HDU (no 2D image)
void createTableOnlyFitsFile(std::string_view path) {
  int status = 0;
  fitsfile *fptr = nullptr;
  const std::string ownedPath(path);
  fits_create_file(&fptr, ownedPath.c_str(), &status);
  fits_create_img(fptr, BYTE_IMG, 0, nullptr, &status);
  fits_close_file(fptr, &status);
}
