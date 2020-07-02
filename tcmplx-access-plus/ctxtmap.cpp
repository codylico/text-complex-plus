/**
 * @file tcmplx-access-plus/ctxtmap.cpp
 * @brief Context map for compressed streams
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#define TCMPLX_AP_WIN32_DLL_INTERNAL
#include "ctxtmap.hpp"
#include <new>
#include <limits>
#include <cstring>
#include <stdexcept>

namespace text_complex {
  namespace access {
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
  };
};
