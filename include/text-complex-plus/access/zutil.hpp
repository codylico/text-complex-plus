/**
 * \file text-complex-plus/access/zutil.hpp
 * \brief Tools for DEFLATE
 * \author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_ZUtil_H_
#define hg_TextComplexAccessP_ZUtil_H_

#include "api.hpp"

namespace text_complex {
  namespace access {
    /**
     * @defgroup zutil Tools for DEFLATE
     *   (access/zutil.hpp)
     * @{
     */
    //BEGIN zutil / namespace local
    /**
     * @brief Compute a checksum.
     * @param len length of buffer to process
     * @param buf buffer to process
     * @param chk last check value
     * @return new checksum
     */            
    TCMPLX_AP_API
    uint32 zutil_adler32
      (size_t len, unsigned char const* buf, uint32 chk = 1u) noexcept;
    //END   zutil / namespace local
    /** @} */
  };
};


#endif //hg_TextComplexAccessP_ZUtil_H_
