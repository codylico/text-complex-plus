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
    //END   bdict / namespace local
  };
};
