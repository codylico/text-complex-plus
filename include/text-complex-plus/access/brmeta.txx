/**
 * @file text-complex-plus/access/brmeta.txx
 * @brief Brotli metadata storage
 */
#ifndef hg_TextComplexAccessP_BrMeta_Txx_
#define hg_TextComplexAccessP_BrMeta_Txx_

#if !(defined hg_TextComplexAccessP_BrMeta_H_)
#  error "Do not include directly; use \"brmeta.h\" instead."
#endif //hg_TextComplexAccessP_BrMeta_H_

namespace text_complex {
  namespace access {
#if  (!(defined TextComplexAccessP_NO_EXCEPT))
    inline
    void brotli_meta::emplace(std::size_t n) {
      api_error ae;
      emplace(n, ae);
      if (ae < api_error::Success) {
        throw api_exception(ae);
      } else return;
    }
#endif //TextComplexAccessP_NO_EXCEPT
  };
};

#endif //hg_TextComplexAccessP_BrMeta_Txx_
