/**
 * @file tcmplx-access-plus/bdict.txx
 * @brief Built-in dictionary
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_BDict_Txx_
#define hg_TextComplexAccessP_BDict_Txx_

#include "bdict.hpp"
#include "api.hpp"

namespace text_complex {
  namespace access {
#if  (!(defined TextComplexAccessP_NO_EXCEPT))
    inline
    unsigned char& bdict_word::at(size_t i) {
      if (i >= len)
        throw api_exception(api_error::OutOfRange);
      else return p[i];
    }

    inline
    unsigned char const& bdict_word::at(size_t i) const {
      if (i >= len)
        throw api_exception(api_error::OutOfRange);
      else return p[i];
    }

    inline
    void bdict_word::resize(size_t sz) {
      api_error ae;
      resize(sz, ae);
      if (ae < api_error::Success) {
        throw api_exception(ae);
      } else return;
    }
#endif //TextComplexAccessP_NO_EXCEPT
  };
};

#endif //hg_TextComplexAccessP_BDict_Txx_
