/*
 * \file tcmplx-plus/api.hpp
 * \brief API-wide declarations for text-complex-plus
 * \author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplex_Plus_api_H_
#define hg_TextComplex_Plus_api_H_

#ifdef TCMPLX_PLUS_WIN32_DLL
#  ifdef TCMPLX_PLUS_WIN32_DLL_INTERNAL
#    define TCMPLX_PLUS_API __declspec(dllexport)
#  else
#    define TCMPLX_PLUS_API __declspec(dllimport)
#  endif /*TCMPLX_PLUS_WIN32_DLL_INTERNAL*/
#else
#  define TCMPLX_PLUS_API
#endif /*TCMPLX_PLUS_WIN32_DLL*/

/**
 * \brief Text processing library
 */
namespace text_complex {
  /**
   * \brief Check the library's version.
   * \return a version string
   */
  char const* api_version(void) noexcept;
};

#endif //hg_TextComplex_Plus_api_H_
