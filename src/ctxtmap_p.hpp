/**
 * @file tcmplx-access-plus/ctxtmap_p.hpp
 * @brief Context map for compressed streams
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_CtxtMap_pH_
#define hg_TextComplexAccessP_CtxtMap_pH_

namespace text_complex {
  namespace access {
    /**
    * @brief Query the Brotli Lookup Table #2.
    * @param i byte value index
    * @return a lookup table value
    */
    unsigned char ctxtmap_getlut2(unsigned char i) noexcept;
  };
};

#endif //hg_TextComplexAccessP_CtxtMap_pH_
