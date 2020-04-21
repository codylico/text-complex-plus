cmake_minimum_required(VERSION 3.0)
option(TCMPLX_ACCESS_P_BUILD_TESTS "build some tests" ON)

if (TCMPLX_ACCESS_P_BUILD_TESTS)
add_library(munit_plus STATIC
  munit-plus/munit.cpp munit-plus/munit.hpp)

#BEGIN API test
  add_executable(tcmplx_accessP_test_api tca_api.cpp)
  target_link_libraries(tcmplx_accessP_test_api tcmplx_access_plus)
#END   API test

#BEGIN Util test
  add_executable(tcmplx_accessP_test_util tca_util.cpp)
  target_link_libraries(tcmplx_accessP_test_util
    munit_plus tcmplx_access_plus)
#END   Util test

#BEGIN OffTable test
  add_executable(tcmplx_accessP_test_offtable tca_offtable.cpp)
  target_link_libraries(tcmplx_accessP_test_offtable
    munit_plus tcmplx_access_plus)
#END   OffTable test
endif (TCMPLX_ACCESS_P_BUILD_TESTS)
