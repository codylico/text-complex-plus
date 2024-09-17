/*
 * \file tcmplx-access-plus/api.txx
 * \brief API-wide declarations for text-complex-plus
 * \author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_api_Txx_
#define hg_TextComplexAccessP_api_Txx_

#include "api.hpp"

#if  (!(defined TextComplexAccessP_NO_EXCEPT))
namespace text_complex {
  namespace access {
    //BEGIN api exception / rule-of-three
    inline
    api_exception::api_exception(api_error value) noexcept
      : v(value)
    {
      return;
    }

    inline
    api_exception::api_exception(api_exception const& a) noexcept
      : v(a.v)
    {
      return;
    }

    inline
    api_exception& api_exception::operator=(api_exception const& a) noexcept {
      v = a.v;
      return *this;
    }

    inline
    api_exception::~api_exception(void) noexcept {
      return;
    }
    //END   api exception / rule-of-three

    //BEGIN api exception / public exception-override
    inline
    char const* api_exception::what(void) const noexcept {
      char const* const out = api_error_toa(v);
      if (!out)
        return "Unknown text_complex error";
      else return out;
    }
    //END   api exception / exception-override

    //BEGIN api exception / namespace local
    inline
    void api_throw(api_error ae) {
      if (ae < api_error::Success) {
        throw api_exception(ae);
      } else return;
    }
    //END   api exception / namespace local
  };
};
#endif //TextComplexAccessP_NO_EXCEPT

#endif //hg_TextComplexAccessP_api_Txx_
