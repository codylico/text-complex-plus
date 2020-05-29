/**
 * @file tcmplx-access-plus/ringdist.txx
 * @brief Distance ring buffer
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_RingDist_Txx_
#define hg_TextComplexAccessP_RingDist_Txx_

#include "ringdist.hpp"
#include "api.hpp"

namespace text_complex {
  namespace access {
#if  (!(defined TextComplexAccessP_NO_EXCEPT))
    inline
    uint32 distance_ring::decode(unsigned int dcode, unsigned int extra) {
      api_error ae;
      uint32 const out = decode(dcode, extra, ae);
      if (ae < api_error::Success) {
        throw api_exception(ae);
      } else return out;
    }
#endif //TextComplexAccessP_NO_EXCEPT
  };
};

#endif //hg_TextComplexAccessP_RingDist_Txx_
