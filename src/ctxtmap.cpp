/**
 * @file tcmplx-access-plus/ctxtmap.cpp
 * @brief Context map for compressed streams
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#define TCMPLX_AP_WIN32_DLL_INTERNAL
#include "ctxtmap_p.hpp"
#include "text-complex-plus/access/ctxtmap.hpp"
#include <new>
#include <limits>
#include <stdexcept>
#include <cstring>

namespace text_complex {
  namespace access {
    /* NOTE from RFC7932 */
    static
    unsigned char ctxtmap_lut0[256] = {
       0,  0,  0,  0,  0,  0,  0,  0,  0,  4,  4,  0,  0,  4,  0,  0,
       0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
       8, 12, 16, 12, 12, 20, 12, 16, 24, 28, 12, 12, 32, 12, 36, 12,
      44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 32, 32, 24, 40, 28, 12,
      12, 48, 52, 52, 52, 48, 52, 52, 52, 48, 52, 52, 52, 52, 52, 48,
      52, 52, 52, 52, 52, 48, 52, 52, 52, 52, 52, 24, 12, 28, 12, 12,
      12, 56, 60, 60, 60, 56, 60, 60, 60, 56, 60, 60, 60, 60, 60, 56,
      60, 60, 60, 60, 60, 56, 60, 60, 60, 60, 60, 24, 12, 28, 12,  0,
       0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
       0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
       0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
       0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
       2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3,
       2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3,
       2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3,
       2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3
     };
    /* NOTE from RFC7932 */
    static
    unsigned char ctxtmap_lut1[256] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1,
        1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1,
        1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
      };
    /* NOTE from RFC7932 */
    static
    unsigned char ctxtmap_lut2[256] = {
         0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
         3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
         3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
         3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
         4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
         4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
         4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
         4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
         5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7
      };
    static
    unsigned char const ctxtmap_imtf_init[256] = {
        0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u,
        8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u,
        16u, 17u, 18u, 19u, 20u, 21u, 22u, 23u,
        24u, 25u, 26u, 27u, 28u, 29u, 30u, 31u,
        32u, 33u, 34u, 35u, 36u, 37u, 38u, 39u,
        40u, 41u, 42u, 43u, 44u, 45u, 46u, 47u,
        48u, 49u, 50u, 51u, 52u, 53u, 54u, 55u,
        56u, 57u, 58u, 59u, 60u, 61u, 62u, 63u,
        64u, 65u, 66u, 67u, 68u, 69u, 70u, 71u,
        72u, 73u, 74u, 75u, 76u, 77u, 78u, 79u,
        80u, 81u, 82u, 83u, 84u, 85u, 86u, 87u,
        88u, 89u, 90u, 91u, 92u, 93u, 94u, 95u,
        96u, 97u, 98u, 99u, 100u, 101u, 102u, 103u,
        104u, 105u, 106u, 107u, 108u, 109u, 110u, 111u,
        112u, 113u, 114u, 115u, 116u, 117u, 118u, 119u,
        120u, 121u, 122u, 123u, 124u, 125u, 126u, 127u,
        128u, 129u, 130u, 131u, 132u, 133u, 134u, 135u,
        136u, 137u, 138u, 139u, 140u, 141u, 142u, 143u,
        144u, 145u, 146u, 147u, 148u, 149u, 150u, 151u,
        152u, 153u, 154u, 155u, 156u, 157u, 158u, 159u,
        160u, 161u, 162u, 163u, 164u, 165u, 166u, 167u,
        168u, 169u, 170u, 171u, 172u, 173u, 174u, 175u,
        176u, 177u, 178u, 179u, 180u, 181u, 182u, 183u,
        184u, 185u, 186u, 187u, 188u, 189u, 190u, 191u,
        192u, 193u, 194u, 195u, 196u, 197u, 198u, 199u,
        200u, 201u, 202u, 203u, 204u, 205u, 206u, 207u,
        208u, 209u, 210u, 211u, 212u, 213u, 214u, 215u,
        216u, 217u, 218u, 219u, 220u, 221u, 222u, 223u,
        224u, 225u, 226u, 227u, 228u, 229u, 230u, 231u,
        232u, 233u, 234u, 235u, 236u, 237u, 238u, 239u,
        240u, 241u, 242u, 243u, 244u, 245u, 246u, 247u,
        248u, 249u, 250u, 251u, 252u, 253u, 254u, 255u
      };

    //BEGIN context_map / internal
    unsigned char ctxtmap_getlut2(unsigned char i) noexcept {
      return ctxtmap_lut2[i];
    }
    //END   context_map / internal

    //BEGIN context_map / rule-of-six
    context_map::context_map(size_t xbtypes, size_t xctxts)
      : p(nullptr), ctxts(0u), btypes(0u)
    {
      resize(xbtypes, xctxts);
      return;
    }

    context_map::~context_map(void) {
      if (p) {
        delete[] p;
        p = nullptr;
      }
      btypes = 0u;
      ctxts = 0u;
      return;
    }

    context_map::context_map(context_map const& other)
      : p(nullptr), ctxts(0u), btypes(0u)
    {
      duplicate(other);
      return;
    }

    context_map& context_map::operator=(context_map const& other) {
      duplicate(other);
      return *this;
    }

    context_map::context_map(context_map&& other) noexcept
      : p(nullptr), ctxts(0u), btypes(0u)
    {
      transfer(static_cast<context_map&&>(other));
      return;
    }

    context_map& context_map::operator=(context_map&& other) noexcept {
      transfer(static_cast<context_map&&>(other));
      return *this;
    }

    void context_map::duplicate(context_map const& other) {
      if (this == &other)
        return;
      resize(other.btypes, other.ctxts);
      std::memcpy(p, other.p, btypes*ctxts);
      return;
    }

    void context_map::transfer(context_map&& other) noexcept {
      /* release */
      unsigned char *const np = other.p; other.p = nullptr;
      size_t const nbtypes = other.btypes; other.btypes = 0u;
      size_t const nctxts = other.ctxts; other.ctxts = 0u;
      /* reset */{
        if (p) {
          delete[] p;
        }
        p = np;
        btypes = nbtypes;
        ctxts = nctxts;
      }
      return;
    }

    void context_map::resize(size_t b, size_t c) {
      if (c > 0u && b >= std::numeric_limits<size_t>::max()/c) {
        throw std::bad_alloc();
      } else if (c == 0u || b == 0u) {
        if (p) {
          delete[] p;
          p = nullptr;
        }
      } else {
        unsigned char* const np = new unsigned char[b*c];
        if (p)
          delete[] p;
        p = np;
      }
      ctxts = c;
      btypes = b;
      return;
    }
    //END   context_map / rule-of-six

    //BEGIN context_map / allocation
    void* context_map::operator new(std::size_t sz) {
      return ::operator new(sz);
    }

    void* context_map::operator new[](std::size_t sz) {
      return ::operator new(sz);
    }

    void context_map::operator delete(void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    void context_map::operator delete[](void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    context_map* ctxtmap_new(size_t btypes, size_t ctxts) noexcept {
      try {
        return new context_map(btypes, ctxts);
      } catch (api_exception const& ) {
        return nullptr;
      } catch (std::bad_alloc const& ) {
        return nullptr;
      }
    }

    util_unique_ptr<context_map> ctxtmap_unique
          (size_t btypes, size_t ctxts) noexcept
    {
      return util_unique_ptr<context_map>(ctxtmap_new(btypes, ctxts));
    }

    void ctxtmap_destroy(context_map* x) noexcept {
      if (x) {
        delete x;
      }
    }
    //END   context_map / allocation

    //BEGIN context_map / public
    size_t context_map::contexts(void) const noexcept {
      return ctxts;
    }

    size_t context_map::block_types(void) const noexcept {
      return btypes;
    }

    unsigned char* context_map::data(void) noexcept {
      return p;
    }

    unsigned char const* context_map::data(void) const noexcept {
      return p;
    }
    //END   context_map / public

    //BEGIN context_map / matrix-compat
    unsigned char& context_map::operator()(size_t i, size_t j) noexcept {
      return p[i*ctxts+j];
    }

    unsigned char const& context_map::operator()
        (size_t i, size_t j) const noexcept
    {
      return p[i*ctxts+j];
    }

    unsigned char& context_map::at(size_t i, size_t j) {
      if (i >= btypes || j >= ctxts)
        throw std::out_of_range("text_complex::access::context_map::at");
      else return this->operator()(i,j);
    }

    unsigned char const& context_map::at(size_t i, size_t j) const {
      if (i >= btypes || j >= ctxts)
        throw std::out_of_range("text_complex::access::context_map::at");
      else return this->operator()(i,j);
    }
    //END   context_map / matrix-compat

    //BEGIN context_map / namespace local
    size_t ctxtmap_distance_context
        (unsigned long int copylen, api_error& ae) noexcept
    {
      if (copylen < 2) {
        ae = api_error::Param;
        return std::numeric_limits<size_t>::max();
      } else {
        ae = api_error::Success;
        switch (copylen) {
        case 2: return 0;
        case 3: return 1;
        case 4: return 2;
        default: return 3;
        }
      }
    }

    size_t ctxtmap_literal_context
      ( context_map_mode mode, unsigned char p1, unsigned char p2,
        api_error& ae) noexcept
    {
      switch (mode) {
      case context_map_mode::LSB6:
        ae = api_error::Success;
        return p1&63;
      case context_map_mode::MSB6:
        ae = api_error::Success;
        return (p1>>2)&63;
      case context_map_mode::UTF8:
        ae = api_error::Success;
        return ctxtmap_lut0[p1&255] | ctxtmap_lut1[p2&255];
      case context_map_mode::Signed:
        ae = api_error::Success;
        return (ctxtmap_lut2[p1&255]<<3) | ctxtmap_lut2[p2&255];
      default:
        ae = api_error::Param;
        return std::numeric_limits<size_t>::max();
      }
    }

    void ctxtmap_apply_movetofront(context_map& x) noexcept {
      size_t const len = x.contexts() * x.block_types();
      unsigned char *const data = x.data();
      unsigned char mtf_refs[256];
      std::memcpy(mtf_refs, ctxtmap_imtf_init, 256u*sizeof(unsigned char));
      /* */{
        size_t i;
        for (i = 0u; i < len; ++i) {
          unsigned char const v = data[i];
          unsigned short const j = static_cast<unsigned short>(
                  static_cast<unsigned char*>(std::memchr(mtf_refs, v, 256u))
                - mtf_refs
              );
          data[i] = j;
          std::memmove(mtf_refs+1u, mtf_refs, j*sizeof(char));
          mtf_refs[0u] = v;
        }
      }
      return;
    }

    void ctxtmap_revert_movetofront(context_map& x) noexcept {
      size_t const len = x.contexts() * x.block_types();
      unsigned char *const data = x.data();
      unsigned char mtf_refs[256];
      std::memcpy(mtf_refs, ctxtmap_imtf_init, 256u*sizeof(unsigned char));
      /* */{
        size_t i;
        for (i = 0u; i < len; ++i) {
          unsigned short const j = data[i];
          unsigned char const v = mtf_refs[j];
          data[i] = v;
          std::memmove(mtf_refs+1u, mtf_refs, j*sizeof(unsigned char));
          mtf_refs[0u] = v;
        }
      }
      return;
    }
    //END   context_map / namespace local
  };
};
