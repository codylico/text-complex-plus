/**
 * @file text-complex-plus/access/ctxtspan.cpp
 * @brief Context span heuristic for compressed streams
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#define TCMPLX_AP_WIN32_DLL_INTERNAL
#include "ctxtmap_p.hpp"
#include "text-complex-plus/access/ctxtspan.hpp"
#include "text-complex-plus/access/api.hpp"
#include "text-complex-plus/access/util.hpp"
#include <limits>
#include <cstring>

namespace text_complex {
  namespace access {

    enum ctxtspan_consts {
        CtxtSpan_UtfPoint = 6,
        CtxtSpan_Ceiling = 6
    };


    /**
     * @brief Count the number of nonzero bits in an integer.
     * @param x integer to inspect
     * @return a bit count
     */
    static
    int ctxtspan_popcount(unsigned x);
    /**
     * @brief Add and subtract numbers from a score ceiling.
     * @param sub subtractor
     * @param add addend
     * @return a sum
     */
    static
    unsigned ctxtspan_subscore(unsigned sub, unsigned add);
    /**
     * @brief Absolute difference of two unsigned integers.
     * @param a one integer
     * @param b other integer
     * @return a difference
     */
    static
    unsigned ctxtspan_absdiff(unsigned a, unsigned b) noexcept;
    /**
     * @brief Select a mode from a score collection.
     * @param score collection to use
     * @return a context mode
     */
    static
    context_map_mode ctxtspan_select(context_score const& score) noexcept;

    /* BEGIN context span / static */
    int ctxtspan_popcount(unsigned x) {
    #if (defined __GNUC__)
        return __builtin_popcount(x);
    #else
        int out = 0;
        int i;
        for (i = 0; i < std::numeric_limits<unsigned>::digits(); ++i, x >>= 1) {
            out += (x&1);
        }
        return out;
    #endif //__GNUC__
    }

    unsigned ctxtspan_subscore(unsigned sub, unsigned add) {
        return add > sub ? CtxtSpan_Ceiling : CtxtSpan_Ceiling+add-sub;
    }

    context_map_mode ctxtspan_select(context_score const& score) noexcept {
        unsigned current_score = 0;
        context_map_mode mode = context_map_mode::LSB6;
        for (context_map_mode i = context_map_mode::LSB6; i < context_map_mode::ModeMax; ++i) {
            if (score[i] > current_score) {
                mode = i;
                current_score = score[i];
            }
        }
        return mode;
    }

    unsigned ctxtspan_absdiff(unsigned a, unsigned b) noexcept {
        return a > b ? a - b : b - a;
    }
    /* END   context span / static */

    /* BEGIN context score / public */
    void ctxtspan_guess(context_score& results,
        void const* buf, size_t buf_len) noexcept
    {
        context_score out = results;
        unsigned char const* p = (unsigned char const*)buf;
        unsigned char last = 0;
        unsigned char utf8count = 0;
        size_t i;
        for (i = 0; i < buf_len; last = p[i], ++i) {
            unsigned char const current = p[i];
            /* LSB6 & MSB6 */{
                unsigned char const lsb_tmp = last ^ current;
                out[context_map_mode::LSB6] += ctxtspan_subscore(
                    ctxtspan_popcount(lsb_tmp&0xC0)*3,
                    ctxtspan_popcount(lsb_tmp&0x3F));
                out[context_map_mode::MSB6] += ctxtspan_subscore(
                    ctxtspan_popcount(lsb_tmp&0x03)*3,
                    ctxtspan_popcount(lsb_tmp&0xFC));
            }
            /* Signed */{
                unsigned char const last_lut2 = ctxtmap_getlut2(last);
                unsigned char const lut2 = ctxtmap_getlut2(current);
                out[context_map_mode::Signed] += (7 -
                    (lut2>last_lut2 ? lut2-last_lut2 : last_lut2-lut2));
            }
            /* UTF8 */{
                if (utf8count > 0) {
                    unsigned const ok = ((current&0xC0)==0x80);
                    utf8count = ok ? utf8count-1 : 0;
                    out[context_map_mode::UTF8] += ok*CtxtSpan_UtfPoint;
                } else if (current >= 0xF0) {
                    utf8count = 3;
                    out[context_map_mode::UTF8] += CtxtSpan_UtfPoint;
                } else if (current >= 0xE0) {
                    utf8count = 2;
                    out[context_map_mode::UTF8] += CtxtSpan_UtfPoint;
                } else if (current >= 0xC0) {
                    utf8count = 1;
                    out[context_map_mode::UTF8] += CtxtSpan_UtfPoint;
                } else {
                    out[context_map_mode::UTF8] += (current<0x80)*CtxtSpan_UtfPoint;
                }
            }
        }
        results = out;
        return;
    }

    void ctxtspan_subdivide(context_span& results,
        void const* buf, size_t buf_len, unsigned margin) noexcept
    {
        unsigned char const* const char_buf = static_cast<unsigned char const*>(buf);
        context_score scores[CtxtSpan_Size] = {};
        unsigned char groups[CtxtSpan_Size] = {};
        size_t stops[CtxtSpan_Size] = {};
        size_t const span_len = (buf_len/CtxtSpan_Size);
        unsigned char last_group = std::numeric_limits<unsigned char>::max();
        results = {};
        for (unsigned i = 0; i < CtxtSpan_Size; ++i) {
            groups[i] = (unsigned char)i;
            results.offsets[i] = span_len*i;
            stops[i] = (i+1>=CtxtSpan_Size) ? buf_len : span_len*(i+1);
        }
        /* Initial guesses. */
        for (unsigned i = 0; i < CtxtSpan_Size; ++i) {
            size_t const start = results.offsets[i];
            size_t const stop = stops[i];
            ctxtspan_guess(scores[i], char_buf+start, stop-start);
            results.modes[i] = ctxtspan_select(scores[i]);
        }
        /* Collapse. */
        for (unsigned bit = 0; bit < 4; ++bit) {
            unsigned const substep = 1u<<bit;
            unsigned const step = substep<<1;
            for (unsigned i = 0; i < CtxtSpan_Size; i += step) {
                unsigned const inner = substep>>1;
                unsigned const next = i+substep;
                size_t const start = results.offsets[i];
                size_t const stop = stops[i];
                unsigned int j;
                if (groups[i] != groups[i+inner]
                ||  groups[next] != groups[next+inner])
                {
                    /* Previous grouping step failed. */
                    continue;
                }
                /* See if the contexts are similar enough. */
                if (results.modes[i] != results.modes[next]) {
                    int const i_mode = static_cast<int>(results.modes[i]);
                    int const next_mode = static_cast<int>(results.modes[next]);
                    /*
                    * Cross difference based on `X` term of seam objective from:
                    * Agarwala et al. "Interactive Digital Photomontage."
                    *   ACM Transactions on Graphics (Proceedings of SIGGRAPH 2004), 2004.
                    * https://grail.cs.washington.edu/projects/photomontage/
                    */
                    unsigned const cross_diff =
                        ctxtspan_absdiff(scores[i].vec[i_mode], scores[i].vec[next_mode])
                        + ctxtspan_absdiff(scores[next].vec[i_mode], scores[next].vec[next_mode]);
                    if (cross_diff > margin)
                        continue;
                }
                /* Group the spans. */
                for (j = i; j < i+step; ++j)
                    groups[j] = (unsigned char)i;
                ctxtspan_guess(scores[i], char_buf+start, stop-start);
                {
                    context_map_mode const mode = ctxtspan_select(scores[i]);
                    results.modes[i] = mode;
                    results.modes[next] = mode;
                }
                stops[i] = stops[next];
                results.offsets[next] = stops[next];
            }
        }
        /* Shift */
        results.count = 0;
        results.total_bytes = buf_len;
        for (unsigned i = 0; i < CtxtSpan_Size; ++i) {
            size_t const current = results.count;
            if (groups[i] == last_group || results.offsets[i] == stops[i])
                continue;
            last_group = groups[i];
            results.offsets[current] = results.offsets[i];
            results.modes[current] = results.modes[i];
            results.count += 1;
        }
        for (size_t i = results.count; i < CtxtSpan_Size; ++i) {
            results.offsets[i] = buf_len;
            results.modes[i] = context_map_mode::ModeMax;
        }
        return;
    }
    /* BEGIN context score / public */

  }
}
