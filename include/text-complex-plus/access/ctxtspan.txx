/**
 * @file text-complex-plus/access/ctxtspan.txx
 * @brief Context span heuristic for compressed streams
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_CtxtSpan_Txx_
#define hg_TextComplexAccessP_CtxtSpan_Txx_

#ifndef hg_TextComplexAccessP_CtxtSpan_H_
#  error "Use ctxtspan.hpp instead."
#endif //hg_TextComplexAccessP_CtxtSpan_H_

namespace text_complex {
  namespace access {
      inline
      unsigned& context_score::operator[](context_map_mode m) noexcept {
        return vec[static_cast<unsigned>(m)];
      }
      inline
      unsigned const& context_score::operator[](context_map_mode m) const noexcept {
        return vec[static_cast<unsigned>(m)];
      }
  };
};

#endif //hg_TextComplexAccessP_CtxtSpan_Txx_
