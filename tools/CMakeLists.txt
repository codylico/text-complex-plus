cmake_minimum_required(VERSION 3.0)
option(TCMPLX_ACCESS_P_BUILD_TOOLS "build some tools" OFF)

if (TCMPLX_ACCESS_P_BUILD_TOOLS)
#BEGIN ZCvt tool
  add_executable(tcmplx_accessP_zcvt tca_zcvt.cpp)
  target_link_libraries(tcmplx_accessP_zcvt tcmplx_access_plus)
#END   ZCvt tool

#BEGIN BrCvt tool
add_executable(tcmplx_accessP_brcvt tca_brcvt.cpp)
target_link_libraries(tcmplx_accessP_brcvt tcmplx_access_plus)
#END   BrCvt tool
endif (TCMPLX_ACCESS_P_BUILD_TOOLS)
