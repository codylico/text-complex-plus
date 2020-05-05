cmake_minimum_required(VERSION 3.0)
project(text-complex-access-plus CXX)


set(text_complex_access_plus_FILES
  api.cpp          api.hpp
  util.cpp         util.hpp
  offtable.cpp     offtable.hpp
  seq.cpp          seq.hpp           seq.txx
  )

add_library(tcmplx_access_plus ${text_complex_access_plus_FILES})
target_link_libraries(tcmplx_access_plus mmaptwo_plus)
target_compile_features(tcmplx_access_plus
    PUBLIC cxx_noexcept cxx_deleted_functions cxx_final
      cxx_constexpr cxx_rvalue_references
    PRIVATE cxx_nullptr
  )
if (WIN32 AND BUILD_SHARED_LIBS)
  target_compile_definitions(tcmplx_access_plus
    PUBLIC "TCMPLX_AP_WIN32_DLL")
endif(WIN32 AND BUILD_SHARED_LIBS)
