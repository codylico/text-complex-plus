cmake_minimum_required(VERSION 3.0)
project(text-complex-access-plus CXX)


set(text_complex_access_plus_FILES
  api.cpp
  util.cpp
  offtable.cpp
  seq.cpp
  woff2.cpp
  fixlist.cpp
  ringdist.cpp
  inscopy.cpp
  ctxtmap.cpp

  bdict.cpp    bdict_p.hpp  bdict_cb.cpp
  bdict04.cpp  bdict05.cpp  bdict06.cpp  bdict07.cpp  bdict08.cpp
  bdict09.cpp  bdict10.cpp  bdict11.cpp  bdict12.cpp  bdict13.cpp
  bdict14.cpp  bdict15.cpp  bdict16.cpp  bdict17.cpp  bdict18.cpp
  bdict19.cpp  bdict20.cpp  bdict21.cpp  bdict22.cpp  bdict23.cpp
  bdict24.cpp

  zutil.cpp
  ringslide.cpp
  hashchain.cpp
  blockbuf.cpp
  )
set(text_complex_access_plus_HEADERS
  ../include/text-complex-plus/access/api.hpp
  ../include/text-complex-plus/access/api.txx
  ../include/text-complex-plus/access/util.hpp
  ../include/text-complex-plus/access/offtable.hpp
  ../include/text-complex-plus/access/seq.hpp
  ../include/text-complex-plus/access/seq.txx
  ../include/text-complex-plus/access/woff2.hpp
  ../include/text-complex-plus/access/fixlist.hpp
  ../include/text-complex-plus/access/fixlist.txx
  ../include/text-complex-plus/access/ringdist.hpp
  ../include/text-complex-plus/access/ringdist.txx
  ../include/text-complex-plus/access/inscopy.hpp
  ../include/text-complex-plus/access/inscopy.txx
  ../include/text-complex-plus/access/ctxtmap.hpp
  ../include/text-complex-plus/access/ctxtmap.txx
  ../include/text-complex-plus/access/bdict.hpp
  ../include/text-complex-plus/access/bdict.txx
  ../include/text-complex-plus/access/ringslide.hpp
  ../include/text-complex-plus/access/ringslide.txx
  ../include/text-complex-plus/access/hashchain.hpp
  ../include/text-complex-plus/access/hashchain.txx
  ../include/text-complex-plus/access/blockbuf.hpp
  ../include/text-complex-plus/access/blockbuf.txx
  )

add_library(tcmplx_access_plus ${text_complex_access_plus_FILES}
  ${text_complex_access_plus_HEADERS})
target_link_libraries(tcmplx_access_plus mmaptwo_plus)
target_compile_features(tcmplx_access_plus
    PUBLIC cxx_noexcept cxx_deleted_functions cxx_final
      cxx_defaulted_functions cxx_constexpr cxx_rvalue_references
      cxx_strong_enums
    PRIVATE cxx_nullptr
  )
target_include_directories(tcmplx_access_plus
    PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../deps>"
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>" 
      "$<INSTALL_INTERFACE:include>" 
  )

if (WIN32 AND BUILD_SHARED_LIBS)
  target_compile_definitions(tcmplx_access_plus
    PUBLIC "TCMPLX_AP_WIN32_DLL")
endif(WIN32 AND BUILD_SHARED_LIBS)
