/*
 * \file tcmplx-access-plus/api.hpp
 * \brief API-wide declarations for text-complex-plus
 * \author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_api_H_
#define hg_TextComplexAccessP_api_H_

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
    /**
     * \brief Check the library's version.
     * \return a version string
     */
    TCMPLX_AP_API
    char const* api_version(void) noexcept;
  };
};

#endif //hg_TextComplexAccessP_api_H_
