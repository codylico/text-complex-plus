cmake_minimum_required(VERSION 3.0)
project(text-complex-access-plus CXX)


set(text_complex_access_plus_FILES
  api.cpp     api.hpp
  )

add_library(tcmplx_access_plus ${text_complex_access_plus_FILES})
target_compile_features(tcmplx_access_plus
    PUBLIC cxx_noexcept
  )
