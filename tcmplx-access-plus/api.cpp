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

    //BEGIN error codes
    char const* api_error_toa(api_error v) noexcept {
      switch (v) {
      case api_error::ErrParam:
        return "Invalid parameter given";
      case api_error::ErrSanitize:
        return "File sanity check failed";
      case api_error::ErrMemory:
        return "Memory acquisition error";
      case api_error::ErrInit:
        return "Initialization error";
      case api_error::Success:
        return "Success";
      default:
        return nullptr;
      }
    }
    //END   error codes
  };
};
