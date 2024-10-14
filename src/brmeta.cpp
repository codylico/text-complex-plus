/*
 * \file tcmplx-access-plus/brmeta.cpp
 * \brief Brotli metadata storage
 * \author Cody Licorish (svgmovement@gmail.com)
 */
#define TCMPLX_AP_WIN32_DLL_INTERNAL
#include "text-complex-plus/access/brmeta.hpp"
#include "text-complex-plus/access/blockbuf.hpp"
#include <new>
#include <limits>
#include <utility>
#include <stdexcept>
#include <cstring>
#include <cassert>

namespace text_complex {
  namespace access {
    static api_error brmeta_ensure(block_string*& lines,
      std::size_t n, std::size_t& cap);



    api_error brmeta_ensure(block_string*& lines,
        std::size_t n, std::size_t& cap)
    {
      constexpr std::size_t cap_max = std::numeric_limits<int>::max()/2;
      block_string* ptr = nullptr;
      std::size_t const new_cap = (cap*2+1);
      if (n < cap)
        return api_error::Success;
      else if (cap >= cap_max/2)
        return api_error::Memory;
      try {
        block_string* const ptr = new block_string[new_cap];
        for (std::size_t i = 0; i < n; ++i)
          ptr[i] = std::move(lines[i]);
        delete[] lines;
        lines = ptr;
        cap = new_cap;
        return api_error::Success;
      } catch (std::bad_alloc const&) {
        return api_error::Memory;
      }
    }

    brotli_meta::brotli_meta(std::size_t reserve)
      : lines(nullptr), n(0u), cap(0u)
    {
      if (reserve > 0u) {
        lines = new block_string[reserve];
        cap = reserve;
      }
      return;
    }

    brotli_meta::~brotli_meta(void) {
      if (this->lines) {
        delete[] this->lines;
      }
      this->lines = nullptr;
      this->n = 0u;
      this->cap = 0u;
      return;
    }

    brotli_meta::brotli_meta(brotli_meta const& other)
      : lines(nullptr), n(0u), cap(0u)
    {
      if (other.n > 0u) {
        lines = new block_string[other.n];
        for (std::size_t i = 0; i < n; ++i)
          lines[i] = other.lines[i];
        n = other.n;
        cap = other.n;
      }
      return;
    }

    brotli_meta& brotli_meta::operator=(brotli_meta const& other) {
      block_string* new_lines = nullptr;
      if (other.n) {
        try {
          new_lines = new block_string[other.n];
          for (std::size_t i = 0; i < other.n; ++i)
            new_lines[i] = other.lines[i];
        } catch (std::bad_alloc const&) {
          if (new_lines)
            delete[] new_lines;
          throw;
        }
      }
      if (lines)
        delete[] lines;
      lines = new_lines;
      n = other.n;
      cap = other.n;
      return *this;
    }

    brotli_meta::brotli_meta(brotli_meta&& other) noexcept
      : lines(util_exchange(other.lines, nullptr)),
      n(util_exchange(other.n, 0u)),
      cap(util_exchange(other.cap, 0u))
    {
      return;
    }

    brotli_meta& brotli_meta::operator=(brotli_meta&& other) noexcept {
      block_string* const new_lines =
        util_exchange(other.lines, nullptr);
      if (lines)
        delete[] lines;
      lines = new_lines;
      n = util_exchange(other.n, 0u);
      cap = util_exchange(other.cap, 0u);
      return *this;
    }

    void* brotli_meta::operator new(std::size_t sz) {
      return ::operator new(sz);
    }

    void* brotli_meta::operator new[](std::size_t sz) {
      return ::operator new(sz);
    }

    void brotli_meta::operator delete(void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    void brotli_meta::operator delete[](void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    brotli_meta* brmeta_new(std::size_t n) noexcept {
      try {
        return new brotli_meta(n);
      } catch (std::bad_alloc const& ) {
        return nullptr;
      }
    }

    util_unique_ptr<brotli_meta> brmeta_unique(std::size_t n) noexcept {
      return util_unique_ptr<brotli_meta>(brmeta_new(n));
    }

    void brmeta_destroy(brotli_meta* x) noexcept {
      if (x) {
        delete x;
      }
    }

    void brotli_meta::emplace(std::size_t n, api_error& ae) noexcept {
      api_error const ensure_res = brmeta_ensure(lines, this->n, cap);
      if (ensure_res != api_error::Success) {
        ae = ensure_res;
        return;
      } else if (n > 16777216) {
        ae = api_error::Memory;
        return;
      } else if (n == 0) {
        ae = api_error::Param;
        return;
      } else try {
        block_string ptr;
        api_error resize_res;
        ptr.resize(n, 0, resize_res);
        if (resize_res != api_error::Success) {
          ae = resize_res;
          return;
        }
        std::memset(&ptr[0], 0, n);
        lines[this->n++] = std::move(ptr);
        ae = api_error::Success;
        return;
      } catch (std::bad_alloc const&) {
        ae = api_error::Memory;
        return;
      }
    }

    //BEGIN brotli_meta / range-based
    block_string* brotli_meta::begin(void) noexcept {
      return this->lines;
    }

    block_string const* brotli_meta::begin(void) const noexcept {
      return this->lines;
    }

    block_string* brotli_meta::end(void) noexcept {
      return this->lines+this->n;
    }

    block_string const* brotli_meta::end(void) const noexcept {
      return this->lines+this->n;
    }
    //END   brotli_meta / range-based

    std::size_t brotli_meta::size(void) const noexcept {
      return this->n;
    }

    block_string& brotli_meta::operator[](size_t i) noexcept {
      return this->lines[i];
    }

    block_string const& brotli_meta::operator[](size_t i) const noexcept {
      return this->lines[i];
    }

    block_string& brotli_meta::at(size_t i) {
      if (i >= this->n)
        throw std::out_of_range("text_complex::access::brotli_meta::at");
      return this->lines[i];
    }

    block_string const& brotli_meta::at(size_t i) const {
      if (i >= this->n)
        throw std::out_of_range("text_complex::access::brotli_meta::at");
      return this->lines[i];
    }
  };
};
