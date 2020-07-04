/**
 * @file tcmplx-access-plus/ctxtmap.txx
 * @brief Context map for compressed streams
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_CtxtMap_Txx_
#define hg_TextComplexAccessP_CtxtMap_Txx_

#include "ctxtmap.hpp"

namespace text_complex {
  namespace access {
#if  (!(defined TextComplexAccessP_NO_EXCEPT))
    inline
    size_t ctxtmap_distance_context(unsigned long int copylen) {
      api_error ae;
      size_t const out = ctxtmap_distance_context(copylen, ae);
      if (ae < api_error::Success) {
        throw api_exception(ae);
      } else return out;
    }

    inline
    size_t ctxtmap_literal_context
      (context_map_mode mode, unsigned char p1, unsigned char p2)
    {
      api_error ae;
      size_t const out = ctxtmap_literal_context(mode,p1,p2, ae);
      if (ae < api_error::Success) {
        throw api_exception(ae);
      } else return out;
    }
#endif //TextComplexAccessP_NO_EXCEPT
  };
};

#endif //hg_TextComplexAccessP_CtxtMap_Txx_
