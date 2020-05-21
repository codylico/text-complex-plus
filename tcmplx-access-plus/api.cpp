/*
 * \file tcmplx-access-plus/api.cpp
 * \brief API-wide declarations for text-complex
 * \author Cody Licorish (svgmovement@gmail.com)
 */
#define TCMPLX_AP_WIN32_DLL_INTERNAL
#include "api.hpp"

namespace text_complex {
  namespace access {
    //BEGIN configurations
    char const* api_version(void) noexcept {
      return "0.1";
    }
    //END   configurations
  };
};
