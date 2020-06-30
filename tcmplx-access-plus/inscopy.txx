/**
 * @file tcmplx-access-plus/inscopy.txx
 * @brief table for insert and copy lengths
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_InsCopy_Txx_
#define hg_TextComplexAccessP_InsCopy_Txx_

#include "inscopy.hpp"

namespace text_complex {
  namespace access {
#if  (!(defined TextComplexAccessP_NO_EXCEPT))
    inline
    void inscopy_preset(insert_copy_table& dst, insert_copy_preset i) {
      api_error ae;
      inscopy_preset(dst, i, ae);
      if (ae < api_error::Success) {
        throw api_exception(ae);
      } else return;
    }

    inline
    void inscopy_codesort(insert_copy_table& ict) {
      api_error ae;
      inscopy_codesort(ict, ae);
      if (ae < api_error::Success) {
        throw api_exception(ae);
      } else return;
    }

    inline
    void inscopy_lengthsort(insert_copy_table& ict) {
      api_error ae;
      inscopy_lengthsort(ict, ae);
      if (ae < api_error::Success) {
        throw api_exception(ae);
      } else return;
    }
#endif //TextComplexAccessP_NO_EXCEPT    
  };
};

#endif //hg_TextComplexAccessP_InsCopy_Txx_
