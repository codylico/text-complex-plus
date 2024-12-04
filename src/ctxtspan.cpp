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
    /* BEGIN context score / public */

  }
}
