/*
 * \file tcmplx/api.cpp
 * \brief API-wide declarations for text-complex
 * \author Cody Licorish (svgmovement@gmail.com)
 */
#define TCMPLX_PLUS_WIN32_DLL_INTERNAL
#include "api.hpp"

namespace text_complex {
  //BEGIN configurations
  char const* api_version(void) noexcept {
    return "0.0";
  }
  //END   configurations
};
