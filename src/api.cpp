/*
 * \file tcmplx-access-plus/api.cpp
 * \brief API-wide declarations for text-complex
 * \author Cody Licorish (svgmovement@gmail.com)
 */
#define TCMPLX_AP_WIN32_DLL_INTERNAL
#if (defined TextComplexAccessP_NO_EXCEPT)
#  undef TextComplexAccessP_NO_EXCEPT
#endif //TextComplexAccessP_NO_EXCEPT
#include "text-complex-plus/access/api.hpp"

namespace text_complex {
  namespace access {
    //BEGIN configurations
    char const* api_version(void) noexcept {
      return "0.3.4-alpha";
    }
    //END   configurations

    //BEGIN error codes
    char const* api_error_toa(api_error v) noexcept {
      switch (v) {
      case api_error::OutOfRange:
        return "Array index out of range";
      case api_error::RingDistOverflow:
        return "Numeric overflow produced by a distance code conversion";
      case api_error::RingDistUnderflow:
        return  "Numeric underflow produced by a distance code conversion";
      case api_error::FixCodeAlloc:
        return "Prefix code lengths of same bit count were too numerous";
      case api_error::FixLenRange:
        return "Prefix code lengths were too large";
      case api_error::Param:
        return "Invalid parameter given";
      case api_error::Sanitize:
        return "File sanity check failed";
      case api_error::Memory:
        return "Memory acquisition error";
      case api_error::Init:
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
