/**
 * @file tcmplx-access-plus/fixlist.txx
 * @brief Prefix code list
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_FixList_Txx_
#define hg_TextComplexAccessP_FixList_Txx_

#include "fixlist.hpp"
#include "api.hpp"

namespace text_complex {
  namespace access {
#if  (!(defined TextComplexAccessP_NO_EXCEPT))
    inline
    void fixlist_gen_codes(prefix_list& dst) {
      api_error ae;
      fixlist_gen_codes(dst, ae);
      if (ae < api_error::Success) {
        throw api_exception(ae);
      } else return;
    }

    inline
    void fixlist_preset(prefix_list& dst, prefix_preset i) {
      api_error ae;
      fixlist_preset(dst, i, ae);
      if (ae < api_error::Success) {
        throw api_exception(ae);
      } else return;
    }
#endif //TextComplexAccessP_NO_EXCEPT    
  };
};

#endif //hg_TextComplexAccessP_FixList_Txx_
