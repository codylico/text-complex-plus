/*
 * \file tcmplx-access-plus/offtable.cpp
 * \brief Access point for TrueType files
 * \author Cody Licorish (svgmovement@gmail.com)
 */
#define TCMPLX_AP_WIN32_DLL_INTERNAL
#include "offtable.hpp"
#include <new>
#include <limits>
#include <stdexcept>

namespace text_complex {
  namespace access {
    //BEGIN offset_table / rule-of-six
    offset_table::offset_table(size_t n)
      : p(nullptr), n(0u)
    {
      resize(n);
      return;
    }

    offset_table::~offset_table(void) {
      if (this->p) {
        delete[] this->p;
      }
      this->p = nullptr;
      this->n = 0u;
      return;
    }

    offset_table::offset_table(offset_table const& other)
      : p(nullptr), n(0u)
    {
      duplicate(other);
      return;
    }

    offset_table& offset_table::operator=(offset_table const& other) {
      duplicate(other);
      return *this;
    }

    offset_table::offset_table(offset_table&& other) noexcept
      : p(nullptr), n(0u)
    {
      transfer(static_cast<offset_table&&>(other));
      return;
    }

    offset_table& offset_table::operator=(offset_table&& other) noexcept {
      transfer(static_cast<offset_table&&>(other));
      return *this;
    }

    void offset_table::duplicate(offset_table const& other) {
      if (this == &other)
        return;
      resize(other.n);
      size_t i;
      for (i = 0; i < n; ++i) {
        p[i] = other.p[i];
      }
      return;
    }

    void offset_table::transfer(offset_table&& other) noexcept {
      if (this == &other)
        return;
      if (this->p) {
        delete[] this->p;
      }
      this->p = other.p;
      this->n = other.n;
      return;
    }

    void offset_table::resize(size_t n) {
      struct offset_line *ptr;
      if (n == 0u) {
        if (this->p) {
          delete[] this->p;
        }
        this->p = nullptr;
        this->n = 0u;
        return;
      }
      if (n >= std::numeric_limits<size_t>::max()/
          sizeof(struct offset_line))
      {
        throw std::bad_alloc();
      }
      ptr = new struct offset_line[n];
      if (this->p) {
        delete[] this->p;
      }
      this->p = ptr;
      this->n = n;
      return;
    }
    //END   offset_table / rule-of-six

    //BEGIN offset_table / allocation
    void* offset_table::operator new(std::size_t sz) {
      return ::operator new(sz);
    }

    void* offset_table::operator new[](std::size_t sz) {
      return ::operator new(sz);
    }

    void offset_table::operator delete(void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    void offset_table::operator delete[](void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    offset_table* offtable_new(size_t n) noexcept {
      try {
        return new offset_table(n);
      } catch (std::bad_alloc const& ) {
        return nullptr;
      }
    }

    util_unique_ptr<offset_table> offtable_unique(size_t n) noexcept {
      return util_unique_ptr<offset_table>(offtable_new(n));
    }

    void offtable_destroy(offset_table* x) noexcept {
      if (x) {
        delete x;
      }
    }
    //END   offset_table / allocation

    //BEGIN offset_table / range-based
    offset_line* offset_table::begin(void) noexcept {
      return this->p;
    }

    offset_line const* offset_table::begin(void) const noexcept {
      return this->p;
    }

    offset_line* offset_table::end(void) noexcept {
      return this->p+this->n;
    }

    offset_line const* offset_table::end(void) const noexcept {
      return this->p+this->n;
    }
    //END   offset_table / range-based

    //BEGIN offset_table / public
    size_t offset_table::size(void) const noexcept {
      return this->n;
    }

    offset_line& offset_table::operator[](size_t i) noexcept {
      return this->p[i];
    }

    offset_line const& offset_table::operator[](size_t i) const noexcept {
      return this->p[i];
    }

    offset_line& offset_table::at(size_t i) {
      if (i >= this->n)
        throw std::out_of_range("text_complex::access::offset_table::at");
      return this->p[i];
    }

    offset_line const& offset_table::at(size_t i) const {
      if (i >= this->n)
        throw std::out_of_range("text_complex::access::offset_table::at");
      return this->p[i];
    }
    //END   offset_table / public
  };
};
