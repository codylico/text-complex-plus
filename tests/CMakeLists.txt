cmake_minimum_required(VERSION 3.0)
option(TCMPLX_ACCESS_P_BUILD_TESTS "build some tests" ON)

if (TCMPLX_ACCESS_P_BUILD_TESTS)
add_library(munit_plus STATIC
  munit-plus/munit.cpp munit-plus/munit.hpp)
add_library(tcmplx_accessP_testfont STATIC
  testfont.cpp testfont.hpp)
target_link_libraries(tcmplx_accessP_testfont
  tcmplx_access_plus mmaptwo_plus munit_plus)
target_include_directories(tcmplx_accessP_testfont
    PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../deps>")
target_compile_features(tcmplx_accessP_testfont
  PUBLIC cxx_strong_enums)

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
  target_include_directories(tcmplx_accessP_test_woff2
    PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../deps>")
#END   Woff2 test

#BEGIN Seq test
  add_executable(tcmplx_accessP_test_seq tca_seq.cpp)
  target_link_libraries(tcmplx_accessP_test_seq
    tcmplx_accessP_testfont munit_plus tcmplx_access_plus mmaptwo_plus)
  target_include_directories(tcmplx_accessP_test_seq
    PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../deps>")
#END   Seq test

#BEGIN FixList test
  add_executable(tcmplx_accessP_test_fixlist tca_fixlist.cpp)
  target_link_libraries(tcmplx_accessP_test_fixlist
    tcmplx_accessP_testfont munit_plus tcmplx_access_plus mmaptwo_plus)
#END   FixList test

#BEGIN RingDist test
  add_executable(tcmplx_accessP_test_ringdist tca_ringdist.cpp)
  target_link_libraries(tcmplx_accessP_test_ringdist
    tcmplx_accessP_testfont munit_plus tcmplx_access_plus)
#END   RingDist test

#BEGIN InsCopy test
  add_executable(tcmplx_accessP_test_inscopy tca_inscopy.cpp)
  target_link_libraries(tcmplx_accessP_test_inscopy
    tcmplx_accessP_testfont munit_plus tcmplx_access_plus)
#END   InsCopy test

#BEGIN CtxtMap test
  add_executable(tcmplx_accessP_test_ctxtmap tca_ctxtmap.cpp)
  target_link_libraries(tcmplx_accessP_test_ctxtmap
    tcmplx_accessP_testfont munit_plus tcmplx_access_plus)
#END   CtxtMap test

#BEGIN BDict test
  add_executable(tcmplx_accessP_test_bdict tca_bdict.cpp)
  target_link_libraries(tcmplx_accessP_test_bdict
    tcmplx_accessP_testfont munit_plus tcmplx_access_plus)
#END   BDict test

#BEGIN ZUtil test
  add_executable(tcmplx_accessP_test_zutil tca_zutil.cpp)
  target_link_libraries(tcmplx_accessP_test_zutil
    munit_plus tcmplx_access_plus)
#END   ZUtil test

#BEGIN RingSlide test
  add_executable(tcmplx_accessP_test_ringslide tca_ringslide.cpp)
  target_link_libraries(tcmplx_accessP_test_ringslide
    munit_plus tcmplx_access_plus)
#END   RingSlide test

#BEGIN HashChain test
  add_executable(tcmplx_accessP_test_hashchain tca_hashchain.cpp)
  target_link_libraries(tcmplx_accessP_test_hashchain
    munit_plus tcmplx_access_plus)
#END   HashChain test

#BEGIN BlockBuf test
  add_executable(tcmplx_accessP_test_blockbuf tca_blockbuf.cpp)
  target_link_libraries(tcmplx_accessP_test_blockbuf
    tcmplx_accessP_testfont munit_plus tcmplx_access_plus)
#END   BlockBuf test

#BEGIN ZCvt test
  add_executable(tcmplx_accessP_test_zcvt tca_zcvt.cpp)
  target_link_libraries(tcmplx_accessP_test_zcvt
    tcmplx_accessP_testfont munit_plus tcmplx_access_plus)
#END   ZCvt test

#BEGIN BrMeta test
  add_executable(tcmplx_accessP_test_brmeta tca_brmeta.cpp)
  target_link_libraries(tcmplx_accessP_test_brmeta
    tcmplx_accessP_testfont munit_plus tcmplx_access_plus)
#END   BrMeta test

#BEGIN BrCvt test
  add_executable(tcmplx_accessP_test_brcvt tca_brcvt.cpp)
  target_link_libraries(tcmplx_accessP_test_brcvt
    tcmplx_accessP_testfont munit_plus tcmplx_access_plus)
#END   BrCvt test
endif (TCMPLX_ACCESS_P_BUILD_TESTS)
