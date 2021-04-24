/**
 * @file text-complex-plus/access/bdict_cb.cpp
 * @brief Built-in dictionary
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#define TCMPLX_AP_WIN32_DLL_INTERNAL
#include "bdict_p.hpp"
#include "text-complex-plus/access/bdict.hpp"
#include <new>
#include <cstring>

namespace text_complex {
  namespace access {
    /**
     * @internal
     * @brief Transform callback.
     * @param dst destination buffer
     * @param[in,out] dstlen destination length
     * @param src source buffer
     * @param srclen source length
     */
    typedef void (*bdict_cb_fn)
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen);


    /**
     * @internal
     * @brief Identity transform.
     * @param dst destination buffer
     * @param[in,out] dstlen destination length
     * @param src source buffer
     * @param srclen source length
     */
    static
    void bdict_cb_identity
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept;
    /**
     * @internal
     * @brief Ferment some bytes.
     * @param[out] dst destination buffer
     * @param[in,out] dstlen destination length
     * @param src source buffer
     * @param srclen length of source buffer
     * @param[in,out] srci read position
     * @return advance distance
     */
    static
    unsigned int bdict_brew
      ( unsigned char* dst, unsigned int dstlen,
        unsigned char const* src, unsigned int srclen, unsigned int srci)
      noexcept;
    /**
     * @internal
     * @brief Ferment single transform.
     * @param dst destination buffer
     * @param[in,out] dstlen destination length
     * @param src source buffer
     * @param srclen source length
     */
    static
    void bdict_cb_brew_one
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept;
    /**
     * @internal
     * @brief Ferment complete transform.
     * @param dst destination buffer
     * @param[in,out] dstlen destination length
     * @param src source buffer
     * @param srclen source length
     */
    static
    void bdict_cb_brew_all
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept;
    /**
     * @internal
     * @brief Drop one from front transform.
     * @param dst destination buffer
     * @param[in,out] dstlen destination length
     * @param src source buffer
     * @param srclen source length
     */
    static
    void bdict_cb_dropfront_one
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept;
    /**
     * @internal
     * @brief Drop two from front transform.
     * @param dst destination buffer
     * @param[in,out] dstlen destination length
     * @param src source buffer
     * @param srclen source length
     */
    static
    void bdict_cb_dropfront_two
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept;
    /**
     * @internal
     * @brief Drop three from front transform.
     * @param dst destination buffer
     * @param[in,out] dstlen destination length
     * @param src source buffer
     * @param srclen source length
     */
    static
    void bdict_cb_dropfront_three
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept;
    /**
     * @internal
     * @brief Drop four from front transform.
     * @param dst destination buffer
     * @param[in,out] dstlen destination length
     * @param src source buffer
     * @param srclen source length
     */
    static
    void bdict_cb_dropfront_four
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept;
    /**
     * @internal
     * @brief Drop five from front transform.
     * @param dst destination buffer
     * @param[in,out] dstlen destination length
     * @param src source buffer
     * @param srclen source length
     */
    static
    void bdict_cb_dropfront_five
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept;
    /**
     * @internal
     * @brief Drop six from front transform.
     * @param dst destination buffer
     * @param[in,out] dstlen destination length
     * @param src source buffer
     * @param srclen source length
     */
    static
    void bdict_cb_dropfront_six
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept;
    /**
     * @internal
     * @brief Drop seven from front transform.
     * @param dst destination buffer
     * @param[in,out] dstlen destination length
     * @param src source buffer
     * @param srclen source length
     */
    static
    void bdict_cb_dropfront_seven
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept;
    /**
     * @internal
     * @brief Drop eight from front transform.
     * @param dst destination buffer
     * @param[in,out] dstlen destination length
     * @param src source buffer
     * @param srclen source length
     */
    static
    void bdict_cb_dropfront_eight
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept;
    /**
     * @internal
     * @brief Drop nine from front transform.
     * @param dst destination buffer
     * @param[in,out] dstlen destination length
     * @param src source buffer
     * @param srclen source length
     */
    static
    void bdict_cb_dropfront_nine
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept;
    /**
     * @internal
     * @brief Drop one from back transform.
     * @param dst destination buffer
     * @param[in,out] dstlen destination length
     * @param src source buffer
     * @param srclen source length
     */
    static
    void bdict_cb_dropback_one
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept;
    /**
     * @internal
     * @brief Drop two from back transform.
     * @param dst destination buffer
     * @param[in,out] dstlen destination length
     * @param src source buffer
     * @param srclen source length
     */
    static
    void bdict_cb_dropback_two
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept;
    /**
     * @internal
     * @brief Drop three from back transform.
     * @param dst destination buffer
     * @param[in,out] dstlen destination length
     * @param src source buffer
     * @param srclen source length
     */
    static
    void bdict_cb_dropback_three
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept;
    /**
     * @internal
     * @brief Drop four from back transform.
     * @param dst destination buffer
     * @param[in,out] dstlen destination length
     * @param src source buffer
     * @param srclen source length
     */
    static
    void bdict_cb_dropback_four
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept;
    /**
     * @internal
     * @brief Drop five from back transform.
     * @param dst destination buffer
     * @param[in,out] dstlen destination length
     * @param src source buffer
     * @param srclen source length
     */
    static
    void bdict_cb_dropback_five
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept;
    /**
     * @internal
     * @brief Drop six from back transform.
     * @param dst destination buffer
     * @param[in,out] dstlen destination length
     * @param src source buffer
     * @param srclen source length
     */
    static
    void bdict_cb_dropback_six
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept;
    /**
     * @internal
     * @brief Drop seven from back transform.
     * @param dst destination buffer
     * @param[in,out] dstlen destination length
     * @param src source buffer
     * @param srclen source length
     */
    static
    void bdict_cb_dropback_seven
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept;
    /**
     * @internal
     * @brief Drop eight from back transform.
     * @param dst destination buffer
     * @param[in,out] dstlen destination length
     * @param src source buffer
     * @param srclen source length
     */
    static
    void bdict_cb_dropback_eight
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept;
    /**
     * @internal
     * @brief Drop nine from back transform.
     * @param dst destination buffer
     * @param[in,out] dstlen destination length
     * @param src source buffer
     * @param srclen source length
     */
    static
    void bdict_cb_dropback_nine
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept;

    /**
     * @internal
     * @brief List of callbacks for word transforms.
     */
    static
    bdict_cb_fn const bdict_cblist[21u] = {
      /*  0 */ bdict_cb_identity,
      /*  1 */ bdict_cb_brew_one,
      /*  2 */ bdict_cb_brew_all,
      /*  3 */ bdict_cb_dropfront_one,
      /*  4 */ bdict_cb_dropfront_two,
      /*  5 */ bdict_cb_dropfront_three,
      /*  6 */ bdict_cb_dropfront_four,
      /*  7 */ bdict_cb_dropfront_five,
      /*  8 */ bdict_cb_dropfront_six,
      /*  9 */ bdict_cb_dropfront_seven,
      /* 10 */ bdict_cb_dropfront_eight,
      /* 11 */ bdict_cb_dropfront_nine,
      /* 12 */ bdict_cb_dropback_one,
      /* 13 */ bdict_cb_dropback_two,
      /* 14 */ bdict_cb_dropback_three,
      /* 15 */ bdict_cb_dropback_four,
      /* 16 */ bdict_cb_dropback_five,
      /* 17 */ bdict_cb_dropback_six,
      /* 18 */ bdict_cb_dropback_seven,
      /* 19 */ bdict_cb_dropback_eight,
      /* 20 */ bdict_cb_dropback_nine
    };


    //BEGIN built-in dictionary / static
    unsigned int bdict_brew
      ( unsigned char* dst, unsigned int wpos,
        unsigned char const* src, unsigned int srclen, unsigned int rpos)
      noexcept
    {
      if (src[rpos] < 192u) {
        dst[wpos] = (src[rpos] >= 97u && src[rpos] <= 122u)
          ? src[rpos]^32u : src[rpos];
        return 1u;
      } else if (src[rpos] < 224u) {
        dst[wpos] = src[rpos]; 
        if (rpos + 1u < srclen) {
          dst[wpos + 1u] = src[rpos + 1u] ^ 32u;
          return 2u;
        } else return 1u;
      } else /* src[rpos] <= 255u */{
        dst[wpos] = src[rpos];   
        if (rpos + 1u < srclen) {
          dst[wpos+1u] = src[rpos+1u];
          if (rpos + 2u < srclen) {
            dst[wpos + 2u] = src[rpos + 2u] ^ 5u;
            return 3u;
          } else return 2u;
        } else return 1u;
      }
    }

    void bdict_cb_identity
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept
    {
      std::memcpy(dst+dstlen, src, srclen*sizeof(unsigned char));
      dstlen += srclen;
      return;
    }

    void bdict_cb_brew_one
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept
    {
      unsigned int dstpos = dstlen;
      if (srclen > 0u && dstpos < 34u) {
        unsigned int const srcxpos = bdict_brew(dst, dstpos, src, srclen, 0u);
        dstpos += srcxpos;
        /* */{                
          unsigned int const dstdiff = 37u-dstpos;
          unsigned int const srcdiff = srclen - srcxpos;
          unsigned int const len = (dstdiff < srcdiff) ? dstdiff : srcdiff;
          std::memcpy(dst+dstpos, src+srcxpos, len);
          dstpos += len;
        }
        dstlen = dstpos;
      }
      return;
    }

    void bdict_cb_brew_all
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept
    {
      unsigned int dstpos = dstlen;
      unsigned int i;
      for (i = 0u; i < srclen && dstpos < 34u; ) {
        unsigned int const next = bdict_brew(dst, dstpos, src, srclen, i);
        i += next;
        dstpos += next;
      }
      dstlen = dstpos;
      return;
    }

    void bdict_cb_dropfront_one
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept
    {
      if (srclen >= 1u) {
        unsigned int const omitlen = srclen-1u;
        unsigned int const oldlen = dstlen;
        std::memcpy(dst+oldlen, src+1u, omitlen*sizeof(unsigned char));
        dstlen = oldlen+omitlen;
      }
      return;
    }

    void bdict_cb_dropfront_two
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept
    {
      if (srclen >= 2u) {
        unsigned int const omitlen = srclen-2u;
        unsigned int const oldlen = dstlen;
        std::memcpy(dst+oldlen, src+2u, omitlen*sizeof(unsigned char));
        dstlen = oldlen+omitlen;
      }
      return;
    }

    void bdict_cb_dropfront_three
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept
    {
      if (srclen >= 3u) {
        unsigned int const omitlen = srclen-3u;
        unsigned int const oldlen = dstlen;
        std::memcpy(dst+oldlen, src+3u, omitlen*sizeof(unsigned char));
        dstlen = oldlen+omitlen;
      }
      return;
    }

    void bdict_cb_dropfront_four
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept
    {
      if (srclen >= 4u) {
        unsigned int const omitlen = srclen-4u;
        unsigned int const oldlen = dstlen;
        std::memcpy(dst+oldlen, src+4u, omitlen*sizeof(unsigned char));
        dstlen = oldlen+omitlen;
      }
      return;
    }

    void bdict_cb_dropfront_five
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept
    {
      if (srclen >= 5u) {
        unsigned int const omitlen = srclen-5u;
        unsigned int const oldlen = dstlen;
        std::memcpy(dst+oldlen, src+5u, omitlen*sizeof(unsigned char));
        dstlen = oldlen+omitlen;
      }
      return;
    }

    void bdict_cb_dropfront_six
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept
    {
      if (srclen >= 6u) {
        unsigned int const omitlen = srclen-6u;
        unsigned int const oldlen = dstlen;
        std::memcpy(dst+oldlen, src+6u, omitlen*sizeof(unsigned char));
        dstlen = oldlen+omitlen;
      }
      return;
    }

    void bdict_cb_dropfront_seven
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept
    {
      if (srclen >= 7u) {
        unsigned int const omitlen = srclen-7u;
        unsigned int const oldlen = dstlen;
        std::memcpy(dst+oldlen, src+7u, omitlen*sizeof(unsigned char));
        dstlen = oldlen+omitlen;
      }
      return;
    }

    void bdict_cb_dropfront_eight
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept
    {
      if (srclen >= 8u) {
        unsigned int const omitlen = srclen-8u;
        unsigned int const oldlen = dstlen;
        std::memcpy(dst+oldlen, src+8u, omitlen*sizeof(unsigned char));
        dstlen = oldlen+omitlen;
      }
      return;
    }

    void bdict_cb_dropfront_nine
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept
    {
      if (srclen >= 9u) {
        unsigned int const omitlen = srclen-9u;
        unsigned int const oldlen = dstlen;
        std::memcpy(dst+oldlen, src+9u, omitlen*sizeof(unsigned char));
        dstlen = oldlen+omitlen;
      }
      return;
    }

    void bdict_cb_dropback_one
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept
    {
      if (srclen >= 1u) {
        unsigned int const omitlen = srclen-1u;
        unsigned int const oldlen = (dstlen);
        std::memcpy(dst+oldlen, src, omitlen*sizeof(unsigned char));   
        (dstlen) = oldlen+omitlen;
      }
      return;
    }

    void bdict_cb_dropback_two
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept
    {
      if (srclen >= 2u) {
        unsigned int const omitlen = srclen-2u;
        unsigned int const oldlen = (dstlen);
        std::memcpy(dst+oldlen, src, omitlen*sizeof(unsigned char));   
        (dstlen) = oldlen+omitlen;
      }
      return;
    }

    void bdict_cb_dropback_three
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept
    {
      if (srclen >= 3u) {
        unsigned int const omitlen = srclen-3u;
        unsigned int const oldlen = (dstlen);
        std::memcpy(dst+oldlen, src, omitlen*sizeof(unsigned char));   
        (dstlen) = oldlen+omitlen;
      }
      return;
    }

    void bdict_cb_dropback_four
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept
    {
      if (srclen >= 4u) {
        unsigned int const omitlen = srclen-4u;
        unsigned int const oldlen = (dstlen);
        std::memcpy(dst+oldlen, src, omitlen*sizeof(unsigned char));   
        (dstlen) = oldlen+omitlen;
      }
      return;
    }

    void bdict_cb_dropback_five
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept
    {
      if (srclen >= 5u) {
        unsigned int const omitlen = srclen-5u;
        unsigned int const oldlen = (dstlen);
        std::memcpy(dst+oldlen, src, omitlen*sizeof(unsigned char));   
        (dstlen) = oldlen+omitlen;
      }
      return;
    }

    void bdict_cb_dropback_six
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept
    {
      if (srclen >= 6u) {
        unsigned int const omitlen = srclen-6u;
        unsigned int const oldlen = (dstlen);
        std::memcpy(dst+oldlen, src, omitlen*sizeof(unsigned char));   
        (dstlen) = oldlen+omitlen;
      }
      return;
    }

    void bdict_cb_dropback_seven
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept
    {
      if (srclen >= 7u) {
        unsigned int const omitlen = srclen-7u;
        unsigned int const oldlen = (dstlen);
        std::memcpy(dst+oldlen, src, omitlen*sizeof(unsigned char));   
        (dstlen) = oldlen+omitlen;
      }
      return;
    }

    void bdict_cb_dropback_eight
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept
    {
      if (srclen >= 8u) {
        unsigned int const omitlen = srclen-8u;
        unsigned int const oldlen = (dstlen);
        std::memcpy(dst+oldlen, src, omitlen*sizeof(unsigned char));   
        (dstlen) = oldlen+omitlen;
      }
      return;
    }

    void bdict_cb_dropback_nine
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen) noexcept
    {
      if (srclen >= 9u) {
        unsigned int const omitlen = srclen-9u;
        unsigned int const oldlen = (dstlen);
        std::memcpy(dst+oldlen, src, omitlen*sizeof(unsigned char));   
        (dstlen) = oldlen+omitlen;
      }
      return;
    }
    //END   built-in dictionary / static

    //BEGIN built-in dictionary / private
    void bdict_cb_do
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen, bdict_cb k) noexcept
    {
      (*bdict_cblist[static_cast<unsigned short>(k)])(dst,dstlen,src,srclen);
      return;
    }
    //END   built-in dictionary / private
  };
};
