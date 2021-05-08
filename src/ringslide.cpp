/**
 * @file src/ringslide.cpp
 * @brief Sliding window of past bytes
 */
#define TCMPLX_AP_WIN32_DLL_INTERNAL
#include "text-complex-plus/access/ringslide.hpp"
#include <new>
#include <cstring>

namespace text_complex {
  namespace access {
    //BEGIN slide_ring / rule-of-six
    slide_ring::slide_ring(uint32 qn)
      : n(qn), pos(0u), cap(0u), p(nullptr)
    {
      if (qn > 0x1000000)
        throw api_exception(api_error::Param);
      else return;
    }

    slide_ring::~slide_ring(void) {
      if (p)
        delete[] p;
      p = nullptr;
      cap = 0u;
      n = 0u;
      pos = 0u;
      return;
    }

    slide_ring::slide_ring(slide_ring const& other)
      : n(0u), pos(0u), cap(0u), p(nullptr)
    {
      duplicate(other);
      return;
    }

    slide_ring& slide_ring::operator=(slide_ring const& other) {
      duplicate(other);
      return *this;
    }

    slide_ring::slide_ring(slide_ring&& other) noexcept
      : n(0u), pos(0u), cap(0u), p(nullptr)
    {
      transfer(static_cast<slide_ring&&>(other));
      return;
    }

    slide_ring& slide_ring::operator=(slide_ring&& other) noexcept {
      transfer(static_cast<slide_ring&&>(other));
      return *this;
    }

    void slide_ring::duplicate(slide_ring const& other) {
      /* reconstruct */
      size_t const ncap = other.cap;
      uint32 const extent = other.n;
      uint32 const npos = other.pos;
      unsigned char* const ptr = new unsigned char[ncap];
      std::memcpy(ptr, other.p, ncap);
      /* close and set */{
        if (p)
          delete[] p;
        p = ptr;
        cap = ncap;
        n = extent;
        pos = npos;
      }
      return;
    }

    void slide_ring::transfer(slide_ring&& other) noexcept {
      /* */{
        unsigned char* const ptr = other.p;
        other.p = nullptr;
        if (p)
          delete[] p;
        p = ptr;
      }
      /* */{
        uint32 const x = other.n;
        other.n = 0u;
        n = x;
      }
      /* */{
        uint32 const x = other.pos;
        other.pos = 0u;
        pos = x;
      }
      /* */{
        size_t const x = other.cap;
        other.cap = 0u;
        cap = x;
      }
      return;
    }
    //END   slide_ring / rule-of-six

    //BEGIN slide_ring / allocation
    void* slide_ring::operator new(std::size_t sz) {
      return ::operator new(sz);
    }

    void* slide_ring::operator new[](std::size_t sz) {
      return ::operator new(sz);
    }

    void slide_ring::operator delete(void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    void slide_ring::operator delete[](void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    slide_ring* ringslide_new(uint32 n) noexcept {
      try {
        return new slide_ring(n);
      } catch (api_exception const& ) {
        return nullptr;
      } catch (std::bad_alloc const& ) {
        return nullptr;
      }
    }

    util_unique_ptr<slide_ring> ringslide_unique(uint32 n) noexcept {
      return util_unique_ptr<slide_ring>(ringslide_new(n));
    }

    void ringslide_destroy(slide_ring* x) noexcept {
      if (x) {
        delete x;
      }
    }
    //END   slide_ring / allocation

    //BEGIN slide_ring / vector-compat
    uint32 slide_ring::max_size(void) const noexcept {
      return 0x1000000u;
    }
    //END   slide_ring / vector-compat

    //BEGIN slide_ring / public
    uint32 slide_ring::extent(void) const noexcept {
      return n;
    }
    //END   slide_ring / public
  };
};
