cmake_minimum_required(VERSION 3.0)
option(TCMPLX_ACCESS_P_BUILD_TESTS "build some tests" ON)

if (TCMPLX_ACCESS_P_BUILD_TESTS)
add_library(munit_plus STATIC
  munit-plus/munit.cpp munit-plus/munit.hpp)
target_compile_features(munit_plus PUBLIC cxx_noexcept cxx_constexpr)
add_library(tcmplx_accessP_testfont STATIC
  testfont.cpp testfont.hpp)
target_link_libraries(tcmplx_accessP_testfont mmaptwo_plus munit_plus)

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

#BEGIN Woff2 test
  add_executable(tcmplx_accessP_test_woff2 tca_woff2.cpp)
  target_link_libraries(tcmplx_accessP_test_woff2
    tcmplx_accessP_testfont munit_plus tcmplx_access_plus)
#END   Woff2 test

#BEGIN Seq test
  add_executable(tcmplx_accessP_test_seq tca_seq.cpp)
  target_link_libraries(tcmplx_accessP_test_seq
    tcmplx_accessP_testfont munit_plus tcmplx_access_plus mmaptwo_plus)
#END   Seq test
endif (TCMPLX_ACCESS_P_BUILD_TESTS)
