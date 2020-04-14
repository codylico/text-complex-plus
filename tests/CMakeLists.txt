cmake_minimum_required(VERSION 3.0)
option(TCMPLX_ACCESS_P_BUILD_TESTS "build some tests" ON)

if (TCMPLX_ACCESS_P_BUILD_TESTS)
add_library(munit_plus STATIC
  munit-plus/munit.cpp munit-plus/munit.hpp)
endif (TCMPLX_ACCESS_P_BUILD_TESTS)
