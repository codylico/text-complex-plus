/**
 * @file tcmplx-access-plus/fixlist.cpp
 * @brief Prefix code list
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#define TCMPLX_AP_WIN32_DLL_INTERNAL
#include "fixlist.hpp"
#include <new>
#include <stdexcept>
#include <limits>

namespace text_complex {
  namespace access {
    //BEGIN prefix_list / rule-of-six
    prefix_list::prefix_list(size_t n)
      : p(nullptr), n(0u)
    {
      resize(n);
      return;
    }

    prefix_list::~prefix_list(void) {
      if (this->p) {
        delete[] this->p;
      }
      this->p = nullptr;
      this->n = 0u;
      return;
    }

    prefix_list::prefix_list(prefix_list const& other)
      : p(nullptr), n(0u)
    {
      duplicate(other);
      return;
    }

    prefix_list& prefix_list::operator=(prefix_list const& other) {
      duplicate(other);
      return *this;
    }

    prefix_list::prefix_list(prefix_list&& other) noexcept
      : p(nullptr), n(0u)
    {
      transfer(static_cast<prefix_list&&>(other));
      return;
    }

    prefix_list& prefix_list::operator=(prefix_list&& other) noexcept {
      transfer(static_cast<prefix_list&&>(other));
      return *this;
    }

    void prefix_list::duplicate(prefix_list const& other) {
      if (this == &other)
        return;
      resize(other.n);
      size_t i;
      for (i = 0; i < n; ++i) {
        p[i] = other.p[i];
      }
      return;
    }

    void prefix_list::transfer(prefix_list&& other) noexcept {
      prefix_line* new_p;
      size_t new_n;
      /* release */{
        new_p = other.p;
        other.p = nullptr;
        new_n = other.n;
        other.n = 0u;
      }
      /* reset */{
        if (this->p) {
          delete[] this->p;
        }
        this->p = new_p;
        this->n = new_n;
      }
      return;
    }

    void prefix_list::resize(size_t n) {
      struct prefix_line *ptr;
      if (n == 0u) {
        if (this->p) {
          delete[] this->p;
        }
        this->p = nullptr;
        this->n = 0u;
        return;
      }
      if (n >= std::numeric_limits<size_t>::max()/
          sizeof(struct prefix_line))
      {
        throw std::bad_alloc();
      }
      ptr = new struct prefix_line[n];
      if (this->p) {
        delete[] this->p;
      }
      this->p = ptr;
      this->n = n;
      return;
    }
    //END   prefix_list / rule-of-six

    //BEGIN prefix_list / allocation
    void* prefix_list::operator new(std::size_t sz) {
      return ::operator new(sz);
    }

    void* prefix_list::operator new[](std::size_t sz) {
      return ::operator new(sz);
    }

    void prefix_list::operator delete(void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    void prefix_list::operator delete[](void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    prefix_list* fixlist_new(size_t n) noexcept {
      try {
        return new prefix_list(n);
      } catch (api_exception const& ) {
        return nullptr;
      } catch (std::bad_alloc const& ) {
        return nullptr;
      }
    }

    util_unique_ptr<prefix_list> fixlist_unique(size_t n) noexcept {
      return util_unique_ptr<prefix_list>(fixlist_new(n));
    }

    void fixlist_destroy(prefix_list* x) noexcept {
      if (x) {
        delete x;
      }
    }
    //END   prefix_list / allocation

    //BEGIN prefix_list / range-based
    prefix_line* prefix_list::begin(void) noexcept {
      return this->p;
    }

    prefix_line const* prefix_list::begin(void) const noexcept {
      return this->p;
    }

    prefix_line* prefix_list::end(void) noexcept {
      return this->p+this->n;
    }

    prefix_line const* prefix_list::end(void) const noexcept {
      return this->p+this->n;
    }
    //END   prefix_list / range-based

    //BEGIN prefix_list / array-compat
    size_t prefix_list::size(void) const noexcept {
      return this->n;
    }

    prefix_line& prefix_list::operator[](size_t i) noexcept {
      return this->p[i];
    }

    prefix_line const& prefix_list::operator[](size_t i) const noexcept {
      return this->p[i];
    }

    prefix_line& prefix_list::at(size_t i) {
      if (i >= this->n)
        throw std::out_of_range("text_complex::access::prefix_list::at");
      return this->p[i];
    }

    prefix_line const& prefix_list::at(size_t i) const {
      if (i >= this->n)
        throw std::out_of_range("text_complex::access::prefix_list::at");
      return this->p[i];
    }
    //END   prefix_list / array-compat
  };
};
