@PACKAGE_INIT@

include(CMakeFindDependencyMacro)

find_dependency(OpenCV REQUIRED)
find_dependency(PkgConfig REQUIRED)
pkg_check_modules(CFITSIO REQUIRED IMPORTED_TARGET cfitsio)

include("${CMAKE_CURRENT_LIST_DIR}/astroutilsTargets.cmake")

check_required_components(astroutils)