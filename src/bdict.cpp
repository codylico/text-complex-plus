/**
 * @file text-complex-plus/access/bdict.cpp
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
    static
    unsigned int bdict_wordcounts[25] = {
      0, 0, 0, 0, 1024, 1024, 2048, 2048,
      1024, 1024, 1024, 1024, 1024, 512, 512, 256,
      128, 128, 256, 128, 128, 64, 64, 32,
      32
    };

    /**
     * @brief Return `nullptr`.
     * @return `nullptr`
     */
    static
    unsigned char const* bdict_null(unsigned int );

    /**
     * @brief Word table access callback.
     * @param i word array index
     * @return the word at the given index, NULL otherwise
     */
    typedef unsigned char const* (*bdict_fn)(unsigned int i);


    struct bdict_affix {
      unsigned char p[8];
      unsigned char len;
    };

    struct bdict_formula {
      struct bdict_affix front;
      struct bdict_affix back;
      bdict_cb cb;
    };


    /** @internal @brief Transform list. */
    static
    struct bdict_formula const bdict_formulas[121u] = {
      /*   0 */ { {{0},0},
                  {{0},0},
                  bdict_cb::Identity},
      /*   1 */ { {{0},0},
                  {{0x20},1},
                  bdict_cb::Identity},
      /*   2 */ { {{0x20},1},
                  {{0x20},1},
                  bdict_cb::Identity},
      /*   3 */ { {{0},0},
                  {{0},0},
                  bdict_cb::OmitFirst1},
      /*   4 */ { {{0},0},
                  {{0x20},1},
                  bdict_cb::FermentFirst},
      /*   5 */ { {{0},0},
                  {{0x20,0x74,0x68,0x65,0x20},5},
                  bdict_cb::Identity},
      /*   6 */ { {{0x20},1},
                  {{0},0},
                  bdict_cb::Identity},
      /*   7 */ { {{0x73,0x20},2},
                  {{0x20},1},
                  bdict_cb::Identity},
      /*   8 */ { {{0},0},
                  {{0x20,0x6f,0x66,0x20},4},
                  bdict_cb::Identity},
      /*   9 */ { {{0},0},
                  {{0},0},
                  bdict_cb::FermentFirst},
      /*  10 */ { {{0},0},
                  {{0x20,0x61,0x6e,0x64,0x20},5},
                  bdict_cb::Identity},
      /*  11 */ { {{0},0},
                  {{0},0},
                  bdict_cb::OmitFirst2},
      /*  12 */ { {{0},0},
                  {{0},0},
                  bdict_cb::OmitLast1},
      /*  13 */ { {{0x2c,0x20},2},
                  {{0x20},1},
                  bdict_cb::Identity},
      /*  14 */ { {{0},0},
                  {{0x2c,0x20},2},
                  bdict_cb::Identity},
      /*  15 */ { {{0x20},1},
                  {{0x20},1},
                  bdict_cb::FermentFirst},
      /*  16 */ { {{0},0},
                  {{0x20,0x69,0x6e,0x20},4},
                  bdict_cb::Identity},
      /*  17 */ { {{0},0},
                  {{0x20,0x74,0x6f,0x20},4},
                  bdict_cb::Identity},
      /*  18 */ { {{0x65,0x20},2},
                  {{0x20},1},
                  bdict_cb::Identity},
      /*  19 */ { {{0},0},
                  {{0x22},1},
                  bdict_cb::Identity},
      /*  20 */ { {{0},0},
                  {{0x2e},1},
                  bdict_cb::Identity},
      /*  21 */ { {{0},0},
                  {{0x22,0x3e},2},
                  bdict_cb::Identity},
      /*  22 */ { {{0},0},
                  {{0xa},1},
                  bdict_cb::Identity},
      /*  23 */ { {{0},0},
                  {{0},0},
                  bdict_cb::OmitLast3},
      /*  24 */ { {{0},0},
                  {{0x5d},1},
                  bdict_cb::Identity},
      /*  25 */ { {{0},0},
                  {{0x20,0x66,0x6f,0x72,0x20},5},
                  bdict_cb::Identity},
      /*  26 */ { {{0},0},
                  {{0},0},
                  bdict_cb::OmitFirst3},
      /*  27 */ { {{0},0},
                  {{0},0},
                  bdict_cb::OmitLast2},
      /*  28 */ { {{0},0},
                  {{0x20,0x61,0x20},3},
                  bdict_cb::Identity},
      /*  29 */ { {{0},0},
                  {{0x20,0x74,0x68,0x61,0x74,0x20},6},
                  bdict_cb::Identity},
      /*  30 */ { {{0x20},1},
                  {{0},0},
                  bdict_cb::FermentFirst},
      /*  31 */ { {{0},0},
                  {{0x2e,0x20},2},
                  bdict_cb::Identity},
      /*  32 */ { {{0x2e},1},
                  {{0},0},
                  bdict_cb::Identity},
      /*  33 */ { {{0x20},1},
                  {{0x2c,0x20},2},
                  bdict_cb::Identity},
      /*  34 */ { {{0},0},
                  {{0},0},
                  bdict_cb::OmitFirst4},
      /*  35 */ { {{0},0},
                  {{0x20,0x77,0x69,0x74,0x68,0x20},6},
                  bdict_cb::Identity},
      /*  36 */ { {{0},0},
                  {{0x27},1},
                  bdict_cb::Identity},
      /*  37 */ { {{0},0},
                  {{0x20,0x66,0x72,0x6f,0x6d,0x20},6},
                  bdict_cb::Identity},
      /*  38 */ { {{0},0},
                  {{0x20,0x62,0x79,0x20},4},
                  bdict_cb::Identity},
      /*  39 */ { {{0},0},
                  {{0},0},
                  bdict_cb::OmitFirst5},
      /*  40 */ { {{0},0},
                  {{0},0},
                  bdict_cb::OmitFirst6},
      /*  41 */ { {{0x20,0x74,0x68,0x65,0x20},5},
                  {{0},0},
                  bdict_cb::Identity},
      /*  42 */ { {{0},0},
                  {{0},0},
                  bdict_cb::OmitLast4},
      /*  43 */ { {{0},0},
                  {{0x2e,0x20,0x54,0x68,0x65,0x20},6},
                  bdict_cb::Identity},
      /*  44 */ { {{0},0},
                  {{0},0},
                  bdict_cb::FermentAll},
      /*  45 */ { {{0},0},
                  {{0x20,0x6f,0x6e,0x20},4},
                  bdict_cb::Identity},
      /*  46 */ { {{0},0},
                  {{0x20,0x61,0x73,0x20},4},
                  bdict_cb::Identity},
      /*  47 */ { {{0},0},
                  {{0x20,0x69,0x73,0x20},4},
                  bdict_cb::Identity},
      /*  48 */ { {{0},0},
                  {{0},0},
                  bdict_cb::OmitLast7},
      /*  49 */ { {{0},0},
                  {{0x69,0x6e,0x67,0x20},4},
                  bdict_cb::OmitLast1},
      /*  50 */ { {{0},0},
                  {{0xa,0x9},2},
                  bdict_cb::Identity},
      /*  51 */ { {{0},0},
                  {{0x3a},1},
                  bdict_cb::Identity},
      /*  52 */ { {{0x20},1},
                  {{0x2e,0x20},2},
                  bdict_cb::Identity},
      /*  53 */ { {{0},0},
                  {{0x65,0x64,0x20},3},
                  bdict_cb::Identity},
      /*  54 */ { {{0},0},
                  {{0},0},
                  bdict_cb::OmitFirst9},
      /*  55 */ { {{0},0},
                  {{0},0},
                  bdict_cb::OmitFirst7},
      /*  56 */ { {{0},0},
                  {{0},0},
                  bdict_cb::OmitLast6},
      /*  57 */ { {{0},0},
                  {{0x28},1},
                  bdict_cb::Identity},
      /*  58 */ { {{0},0},
                  {{0x2c,0x20},2},
                  bdict_cb::FermentFirst},
      /*  59 */ { {{0},0},
                  {{0},0},
                  bdict_cb::OmitLast8},
      /*  60 */ { {{0},0},
                  {{0x20,0x61,0x74,0x20},4},
                  bdict_cb::Identity},
      /*  61 */ { {{0},0},
                  {{0x6c,0x79,0x20},3},
                  bdict_cb::Identity},
      /*  62 */ { {{0x20,0x74,0x68,0x65,0x20},5},
                  {{0x20,0x6f,0x66,0x20},4},
                  bdict_cb::Identity},
      /*  63 */ { {{0},0},
                  {{0},0},
                  bdict_cb::OmitLast5},
      /*  64 */ { {{0},0},
                  {{0},0},
                  bdict_cb::OmitLast9},
      /*  65 */ { {{0x20},1},
                  {{0x2c,0x20},2},
                  bdict_cb::FermentFirst},
      /*  66 */ { {{0},0},
                  {{0x22},1},
                  bdict_cb::FermentFirst},
      /*  67 */ { {{0x2e},1},
                  {{0x28},1},
                  bdict_cb::Identity},
      /*  68 */ { {{0},0},
                  {{0x20},1},
                  bdict_cb::FermentAll},
      /*  69 */ { {{0},0},
                  {{0x22,0x3e},2},
                  bdict_cb::FermentFirst},
      /*  70 */ { {{0},0},
                  {{0x3d,0x22},2},
                  bdict_cb::Identity},
      /*  71 */ { {{0x20},1},
                  {{0x2e},1},
                  bdict_cb::Identity},
      /*  72 */ { {{0x2e,0x63,0x6f,0x6d,0x2f},5},
                  {{0},0},
                  bdict_cb::Identity},
      /*  73 */ { {{0x20,0x74,0x68,0x65,0x20},5},
                  {{0x20,0x6f,0x66,0x20,0x74,0x68,0x65,0x20},8},
                  bdict_cb::Identity},
      /*  74 */ { {{0},0},
                  {{0x27},1},
                  bdict_cb::FermentFirst},
      /*  75 */ { {{0},0},
                  {{0x2e,0x20,0x54,0x68,0x69,0x73,0x20},7},
                  bdict_cb::Identity},
      /*  76 */ { {{0},0},
                  {{0x2c},1},
                  bdict_cb::Identity},
      /*  77 */ { {{0x2e},1},
                  {{0x20},1},
                  bdict_cb::Identity},
      /*  78 */ { {{0},0},
                  {{0x28},1},
                  bdict_cb::FermentFirst},
      /*  79 */ { {{0},0},
                  {{0x2e},1},
                  bdict_cb::FermentFirst},
      /*  80 */ { {{0},0},
                  {{0x20,0x6e,0x6f,0x74,0x20},5},
                  bdict_cb::Identity},
      /*  81 */ { {{0x20},1},
                  {{0x3d,0x22},2},
                  bdict_cb::Identity},
      /*  82 */ { {{0},0},
                  {{0x65,0x72,0x20},3},
                  bdict_cb::Identity},
      /*  83 */ { {{0x20},1},
                  {{0x20},1},
                  bdict_cb::FermentAll},
      /*  84 */ { {{0},0},
                  {{0x61,0x6c,0x20},3},
                  bdict_cb::Identity},
      /*  85 */ { {{0x20},1},
                  {{0},0},
                  bdict_cb::FermentAll},
      /*  86 */ { {{0},0},
                  {{0x3d,0x27},2},
                  bdict_cb::Identity},
      /*  87 */ { {{0},0},
                  {{0x22},1},
                  bdict_cb::FermentAll},
      /*  88 */ { {{0},0},
                  {{0x2e,0x20},2},
                  bdict_cb::FermentFirst},
      /*  89 */ { {{0x20},1},
                  {{0x28},1},
                  bdict_cb::Identity},
      /*  90 */ { {{0},0},
                  {{0x66,0x75,0x6c,0x20},4},
                  bdict_cb::Identity},
      /*  91 */ { {{0x20},1},
                  {{0x2e,0x20},2},
                  bdict_cb::FermentFirst},
      /*  92 */ { {{0},0},
                  {{0x69,0x76,0x65,0x20},4},
                  bdict_cb::Identity},
      /*  93 */ { {{0},0},
                  {{0x6c,0x65,0x73,0x73,0x20},5},
                  bdict_cb::Identity},
      /*  94 */ { {{0},0},
                  {{0x27},1},
                  bdict_cb::FermentAll},
      /*  95 */ { {{0},0},
                  {{0x65,0x73,0x74,0x20},4},
                  bdict_cb::Identity},
      /*  96 */ { {{0x20},1},
                  {{0x2e},1},
                  bdict_cb::FermentFirst},
      /*  97 */ { {{0},0},
                  {{0x22,0x3e},2},
                  bdict_cb::FermentAll},
      /*  98 */ { {{0x20},1},
                  {{0x3d,0x27},2},
                  bdict_cb::Identity},
      /*  99 */ { {{0},0},
                  {{0x2c},1},
                  bdict_cb::FermentFirst},
      /* 100 */ { {{0},0},
                  {{0x69,0x7a,0x65,0x20},4},
                  bdict_cb::Identity},
      /* 101 */ { {{0},0},
                  {{0x2e},1},
                  bdict_cb::FermentAll},
      /* 102 */ { {{0xc2,0xa0},2},
                  {{0},0},
                  bdict_cb::Identity},
      /* 103 */ { {{0x20},1},
                  {{0x2c},1},
                  bdict_cb::Identity},
      /* 104 */ { {{0},0},
                  {{0x3d,0x22},2},
                  bdict_cb::FermentFirst},
      /* 105 */ { {{0},0},
                  {{0x3d,0x22},2},
                  bdict_cb::FermentAll},
      /* 106 */ { {{0},0},
                  {{0x6f,0x75,0x73,0x20},4},
                  bdict_cb::Identity},
      /* 107 */ { {{0},0},
                  {{0x2c,0x20},2},
                  bdict_cb::FermentAll},
      /* 108 */ { {{0},0},
                  {{0x3d,0x27},2},
                  bdict_cb::FermentFirst},
      /* 109 */ { {{0x20},1},
                  {{0x2c},1},
                  bdict_cb::FermentFirst},
      /* 110 */ { {{0x20},1},
                  {{0x3d,0x22},2},
                  bdict_cb::FermentAll},
      /* 111 */ { {{0x20},1},
                  {{0x2c,0x20},2},
                  bdict_cb::FermentAll},
      /* 112 */ { {{0},0},
                  {{0x2c},1},
                  bdict_cb::FermentAll},
      /* 113 */ { {{0},0},
                  {{0x28},1},
                  bdict_cb::FermentAll},
      /* 114 */ { {{0},0},
                  {{0x2e,0x20},2},
                  bdict_cb::FermentAll},
      /* 115 */ { {{0x20},1},
                  {{0x2e},1},
                  bdict_cb::FermentAll},
      /* 116 */ { {{0},0},
                  {{0x3d,0x27},2},
                  bdict_cb::FermentAll},
      /* 117 */ { {{0x20},1},
                  {{0x2e,0x20},2},
                  bdict_cb::FermentAll},
      /* 118 */ { {{0x20},1},
                  {{0x3d,0x22},2},
                  bdict_cb::FermentFirst},
      /* 119 */ { {{0x20},1},
                  {{0x3d,0x27},2},
                  bdict_cb::FermentAll},
      /* 120 */ { {{0x20},1},
                  {{0x3d,0x27},2},
                  bdict_cb::FermentFirst}
    };

    /** @internal @brief Word table directory. */
    static
    bdict_fn const bdict_dir[25] = {
      bdict_null,
      bdict_null,
      bdict_null,
      bdict_null,
      bdict_access04,
      bdict_access05,
      bdict_access06,
      bdict_access07,
      bdict_access08,
      bdict_access09,

      bdict_access10,
      bdict_access11,
      bdict_access12,
      bdict_access13,
      bdict_access14,
      bdict_access15,
      bdict_access16,
      bdict_access17,
      bdict_access18,
      bdict_access19,

      bdict_access20,
      bdict_access21,
      bdict_access22,
      bdict_access23,
      bdict_access24
    };

    //BEGIN built-in dictionary / static
    unsigned char const* bdict_null(unsigned int ) {
      return nullptr;
    }
    //END   built-in dictionary / static

    //BEGIN built-in dictionary word / rule-of-zero
    bdict_word::bdict_word(void) noexcept
      : len(0u)
    {
      std::memset(p, 0, 38u*sizeof(unsigned char));
      return;
    }

    bdict_word::bdict_word(unsigned char const* s, size_t sz)
      : len(0u)
    {
      resize(sz);
      std::memcpy(p, s, sz);
      std::memset(p+sz, 0, (38u-sz)*sizeof(unsigned char));
      return;
    }

    void bdict_word::resize(size_t n, api_error& ae) noexcept {
      if (n > 37) {
        ae = api_error::Memory;
        return;
      }
      std::memset(p+n, 0, (38u-n)*sizeof(unsigned char));
      len = static_cast<unsigned short>(n);
      ae = api_error::Success;
      return;
    }
    //END   bdict_word / rule-of-six

    //BEGIN bdict_word / allocation
    void* bdict_word::operator new(std::size_t sz) {
      return ::operator new(sz);
    }

    void* bdict_word::operator new[](std::size_t sz) {
      return ::operator new(sz);
    }

    void bdict_word::operator delete(void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    void bdict_word::operator delete[](void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }
    //END   bdict_word / allocation

    //BEGIN bdict_word / range-based
    unsigned char* bdict_word::begin(void) noexcept {
      return this->p;
    }

    unsigned char const* bdict_word::begin(void) const noexcept {
      return this->p;
    }

    unsigned char* bdict_word::end(void) noexcept {
      return this->p+this->len;
    }

    unsigned char const* bdict_word::end(void) const noexcept {
      return this->p+this->len;
    }
    //END   bdict_word / range-based

    //BEGIN bdict_word / public
    size_t bdict_word::size(void) const noexcept {
      return this->len;
    }

    unsigned char& bdict_word::operator[](size_t i) noexcept {
      return this->p[i];
    }

    unsigned char const& bdict_word::operator[](size_t i) const noexcept {
      return this->p[i];
    }

    size_t bdict_word::max_size(void) const noexcept {
      return 37u;
    };

    bool bdict_word::operator==(bdict_word const& other) const noexcept {
      return len==other.len && (std::memcmp(p,other.p,len)==0);
    };
    //END   bdict_word / public

    //BEGIN bdict / namespace local
    unsigned int bdict_word_count(unsigned int j) noexcept {
      if (j >= 25u)
        return 0u;
      else return bdict_wordcounts[j];
    }

    struct bdict_word bdict_get_word(unsigned int j, unsigned int i) noexcept {
      if (j >= 25u)
        return bdict_word();
      else {
        unsigned char const* ptr = (*bdict_dir[j])(i);
        return ptr ? bdict_word(ptr, j) : bdict_word();
      }
    }

    void bdict_transform
      (struct bdict_word& buf, unsigned int k, api_error& ae) noexcept
    {
      if (k >= 121u) {
        ae = api_error::Param;
        return;
      } else if (buf.size() > 24u) {
        ae = api_error::Memory;
        return;
      } else {
        unsigned char out[38u];
        unsigned int outlen = 0u;
        /* index the arrays */
        struct bdict_formula const& formula = bdict_formulas[k];
        bdict_cb_do
          ( out, outlen, formula.front.p,
            formula.front.len, bdict_cb::Identity);
        bdict_cb_do(out, outlen, &buf[0],
            static_cast<unsigned int>(buf.size()), formula.cb);
        bdict_cb_do
          (out, outlen, formula.back.p, formula.back.len, bdict_cb::Identity);
        buf = bdict_word(out, outlen);
        ae = api_error::Success;
        return;
      }
    }
    //END   bdict / namespace local
  };
};
