/**
 * @file text-complex-plus/access/zcvt.txx
 * @brief zlib conversion state
 */
#ifndef hg_TextComplexAccessP_ZCvt_Txx_
#define hg_TextComplexAccessP_ZCvt_Txx_

#include "zcvt.hpp"
#include "api.hpp"

namespace text_complex {
  namespace access {
#if  (!(defined TextComplexAccessP_NO_EXCEPT))
    inline
    size_t zcvt_bypass
      (zcvt_state& state, unsigned char const* buf, size_t sz)
    {
      api_error ae;
      size_t const out = zcvt_bypass(state, buf, sz, ae);
      if (ae < api_error::Success) {
        throw api_exception(ae);
      } else return out;
    }
#endif //TextComplexAccessP_NO_EXCEPT
  };
};

#endif //hg_TextComplexAccessP_ZCvt_Txx_
