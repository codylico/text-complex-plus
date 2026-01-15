/**
 * @file text-complex-plus/access/brcvt.txx
 * @brief Brotli conversion state
 */
#ifndef hg_TextComplexAccessP_BrCvt_Txx_
#define hg_TextComplexAccessP_BrCvt_Txx_

#if !(defined hg_TextComplexAccessP_BrCvt_H_)
#  error "Do not include directly; use \"brcvt.h\" instead."
#endif //hg_TextComplexAccessP_BrCvt_H_

namespace text_complex {
  namespace access {
    inline
    brotli_meta& brcvt_state::get_metadata() noexcept {
      return metadata;
    }

    inline
    brotli_meta const& brcvt_state::get_metadata() const noexcept {
      return metadata;
    }

#if  (!(defined TextComplexAccessP_NO_EXCEPT))
    inline
    size_t brcvt_bypass
      (brcvt_state& state, unsigned char const* buf, size_t sz)
    {
      api_error ae;
      size_t const out = brcvt_bypass(state, buf, sz, ae);
      if (ae < api_error::Success) {
        throw api_exception(ae);
      } else return out;
    }
#endif //TextComplexAccessP_NO_EXCEPT
  };
};

#endif //hg_TextComplexAccessP_BrCvt_Txx_
