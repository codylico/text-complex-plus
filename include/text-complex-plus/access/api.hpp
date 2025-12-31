/**
 * \file text-complex-plus/access/api.hpp
 * \brief API-wide declarations for text-complex-plus
 * \author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_api_H_
#define hg_TextComplexAccessP_api_H_

#include <exception>
#include <climits>
#include <cstddef>

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
     * @defgroup api API-wide declarations for text-complex-plus
     *   (access/api.hpp)
     * @{
     */
#if UINT_MAX >= 0xFFffFFff
    /** @brief Unsigned integer at least 32 bits. */
    typedef unsigned int uint32;
#else
    /** @brief Unsigned integer at least 32 bits. */
    typedef unsigned long int uint32;
#endif /*UINT_MAX*/
    using std::size_t;

    //BEGIN error codes
    /**
     * @brief Library error codes.
     */
    enum struct api_error : int {
      /** An expected insert-copy code is missing */
      InsCopyMissing = -13,
      /** Expecting a ZLIB dictionary */
      ZDictionary = -12,
      /** Block buffer may overflow */
      BlockOverflow = -11,
      /** Array index out of range */
      OutOfRange = -10,
      /** Numeric overflow produced by a distance code conversion */
      RingDistOverflow = -9,
      /** Negative or zero distance produced by a distance code conversion */
      RingDistUnderflow = -8,
      /** Prefix code lengths of same bit count were too numerous */
      FixCodeAlloc = -7,
      /** Prefix code lengths were too large */
      FixLenRange = -6,
      /** Error occured, not sure how to describe it */
      Unknown = -5,
      /** Invalid parameter given */
      Param = -4,
      /** File sanity check failed */
      Sanitize = -3,
      /** Memory acquisition error */
      Memory = -2,
      /** Initialization error */
      Init = -1,
      /** Success code */
      Success = 0,
      /** End of stream */
      EndOfFile = 1,
      /** Partial output */
      Partial = 2
    };

    /**
     * @brief Convert an error code to a string.
     * @return a corresponding string, or `nullptr` if unavailable.
     */
    TCMPLX_AP_API
    char const* api_error_toa(api_error v) noexcept;
    //END   error codes

    //BEGIN configurations
    /**
     * \brief Check the library's version.
     * \return a version string
     */
    TCMPLX_AP_API
    char const* api_version(void) noexcept;
    //END   configurations
    /** @} */
  };
};

#if  1 || (!(defined TextComplexAccessP_NO_EXCEPT))
#include <exception>

namespace text_complex {
  namespace access {
    /**
     * @brief Exception thrown by text-complex API functions.
     * @ingroup api
     */
    class api_exception : public std::exception {
    private:
      api_error v;

    public:
      api_exception(api_error value = api_error::Unknown) noexcept;
      api_exception(api_exception const& ) noexcept;
      api_exception& operator=(api_exception const&) noexcept;
      ~api_exception(void) noexcept override;

      /**
       * @ingroup exception-override
       * @return a description of the error message
       */
      char const* what(void) const noexcept override;
      /**
       * @ingroup methods
       * @return the Access error activating this exception
       */
      api_error to_error() const noexcept;
    };

    /**
     * @brief Throw an exception if the given error code is negative.
     * @param ae the error code to check
     * @throw api_exception only if `ae` is negative
     */
    void api_throw(api_error ae);
  };
};
#endif //TextComplexAccessP_NO_EXCEPT

#include "api.txx"

#endif //hg_TextComplexAccessP_api_H_
