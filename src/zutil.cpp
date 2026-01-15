/*
 * \file tcmplx-access-plus/zutil.cpp
 * \brief Tools for DEFLATE
 * \author Cody Licorish (svgmovement@gmail.com)
 */
#define TCMPLX_AP_WIN32_DLL_INTERNAL
#if (defined TextComplexAccessP_NO_EXCEPT)
#  undef TextComplexAccessP_NO_EXCEPT
#endif //TextComplexAccessP_NO_EXCEPT
#include "text-complex-plus/access/zutil.hpp"

namespace text_complex {
  namespace access {
    //BEGIN zutil / namespace local
    uint32 zutil_adler32  
      (size_t len, unsigned char const* buf, uint32 chk) noexcept
    {
      uint32 s1 = chk&0xFFff;
      uint32 s2 = (chk>>16);
      size_t i;
      for (i = 0u; i < len; ) {        
        size_t j;
        for (j = 0; j < 5550u && i < len; ++j, ++i) {
          s1 += buf[i];
          s2 += s1; 
        } 
        s1 %= 65521u;
        s2 %= 65521u;
      } 
      return s1|(s2<<16);
    }
    //END   zutil / namespace local
  };
};
