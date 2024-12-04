/**
 * @file text-complex-plus/access/ctxtspan.hpp
 * @brief Context span heuristic for compressed streams
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_CtxtSpan_H_
#define hg_TextComplexAccessP_CtxtSpan_H_

#include "ctxtmap.hpp"
#include "api.hpp"

namespace text_complex {
  namespace access {

    /** @brief Context score vector. */
    struct context_score {
        /** Literal context fitness scores, one per mode. */
        unsigned vec[static_cast<int>(context_map_mode::ModeMax)];
        /**
         * @brief Array index operator.
         * @param m array index
         * @return a reference to the score at the given index
         */
        unsigned& operator[](context_map_mode m) noexcept;
        /**
         * @brief Array index operator.
         * @param m array index
         * @return a reference to the score at the given index
         */
        unsigned const& operator[](context_map_mode m) const noexcept;
    };

    /**
     * @brief Calculate a literal context fitness score.
     * @param[in,out] results score accumulation vector;
     *   zero-fill before for initial call
     * @param buf array of bytes to parse
     * @param buf_len length of array in bytes
     */
    TCMPLX_AP_API
    void ctxtspan_guess(context_score& results,
        void const* buf, size_t buf_len) noexcept;
  }
}

#include "ctxtspan.txx"

#endif //hg_TextComplexAccessP_CtxtSpan_H_
