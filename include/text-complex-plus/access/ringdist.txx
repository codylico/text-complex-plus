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
    uint32 distance_ring::decode(unsigned int dcode, uint32 extra, uint32 norecord) {
      api_error ae;
      uint32 const out = decode(dcode, extra, norecord, ae);
      if (ae < api_error::Success) {
        throw api_exception(ae);
      } else return out;
    }

    inline
    unsigned int distance_ring::encode(uint32 back_dist, uint32& extra, uint32 norecord) {
      api_error ae;
      unsigned int a_extra;
      unsigned int const out = encode(back_dist, a_extra, norecord, ae);
      if (ae < api_error::Success) {
        throw api_exception(ae);
      } else {
        extra = a_extra;
        return out;
      }
    }

    inline
    void distance_ring::reconfigure(bool special, unsigned int direct, unsigned int postfix) {
      api_error ae;
      reconfigure(special, direct, postfix, ae);
      if (ae < api_error::Success)
        throw api_exception(ae);
      return;
    }
#endif //TextComplexAccessP_NO_EXCEPT
  };
};

#endif //hg_TextComplexAccessP_RingDist_Txx_
