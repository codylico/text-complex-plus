/*
 * \file tcmplx-access-plus/api.hpp
 * \brief API-wide declarations for text-complex-plus
 * \author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_api_H_
#define hg_TextComplexAccessP_api_H_

#include <climits>
#include <cstddef>
#include <exception>

#ifdef TCMPLX_AP_WIN32_DLL
#  ifdef TCMPLX_AP_WIN32_DLL_INTERNAL
#    define TCMPLX_AP_API __declspec(dllexport)
#  else
#    define TCMPLX_AP_API __declspec(dllimport)
#  endif /*TCMPLX_AP_WIN32_DLL_INTERNAL*/
#else
#  define TCMPLX_AP_API
#endif /*TCMPLX_AP_WIN32_DLL*/

/**
 * \brief Text processing library
 */
namespace text_complex {
  /**
   * \brief Data access
   */
  namespace access {
#if UINT_MAX >= 0xFFffFFff
    typedef unsigned int uint32;
#else
    typedef unsigned long int uint32;
#endif /*UINT_MAX*/
    using std::size_t;

    // BEGIN error codes
    enum struct api_error {
      /** Invalid parameter given */
      ErrParam = -4,
      /** File sanity check failed */
      ErrSanitize = -3,
      /** Memory acquisition error */
      ErrMemory = -2,
      /** Initialization error */
      ErrInit = -1,
      /** Success code */
      Success = 0
    };
    // END   error codes

    /**
     * \brief Check the library's version.
     * \return a version string
     */
    TCMPLX_AP_API
    char const* api_version(void) noexcept;
  };
};

#endif //hg_TextComplexAccessP_api_H_
