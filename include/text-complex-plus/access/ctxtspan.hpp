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
    /** @brief Various constant expressions for context spans. */
    enum context_expr {
      CtxtSpan_Size = 16
    };

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

    /** @brief Context mode spans. */
    struct context_span {
        /** @brief Total bytes covered. */
        std::size_t total_bytes;
        /** @brief Starting offset for each span. */
        std::size_t offsets[CtxtSpan_Size];
        /** @brief Context mode for each span. */
        context_map_mode modes[CtxtSpan_Size];
        /** @brief Number of offsets in the span. */
        std::size_t count;
        /** @return the maximum spans stored in a context span */
        constexpr std::size_t max_size() { return CtxtSpan_Size; }
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

    /**
     * @brief Calculate context modes for parts of a byte array.
     * @param[out] results new modes for the byte array
     * @param buf array of bytes to parse
     * @param buf_len length of array in bytes
     * @param margin maximum score difference to allow for bad coalescing
     */
    TCMPLX_AP_API
    void ctxtspan_subdivide(context_span& results,
        void const* buf, size_t buf_len, unsigned margin) noexcept;
  }
}

#include "ctxtspan.txx"

#endif //hg_TextComplexAccessP_CtxtSpan_H_
