/**
 * @file text-complex-plus/access/gaspvec.hpp
 * @brief Prefix gasp vector.
 */
#include "text-complex-plus/access/gaspvec.hpp"
#include "text-complex-plus/access/fixlist.hpp"
#include "text-complex-plus/access/util.hpp"
#include <limits>

namespace text_complex {
  namespace access {
#pragma region("gasp_vector / rule-of-six")
    gasp_vector::gasp_vector(size_t n)
      : p(nullptr), n(0u)
    {
      resize(n);
      return;
    }

    gasp_vector::~gasp_vector(void) {
      if (this->p) {
        delete[] this->p;
      }
      this->p = nullptr;
      this->n = 0u;
      return;
    }

    gasp_vector::gasp_vector(gasp_vector const& other)
      : p(nullptr), n(0u)
    {
      copy(other);
      return;
    }

    gasp_vector& gasp_vector::operator=(gasp_vector const& other) {
      copy(other);
      return *this;
    }

    gasp_vector::gasp_vector(gasp_vector&& other) noexcept
      : p(util_exchange(other.p,nullptr)),
        n(util_exchange(other.n,0u))
    {
    }

    gasp_vector& gasp_vector::operator=(gasp_vector&& other) noexcept {
      prefix_list* const new_p = util_exchange(other.p, nullptr);
      std::size_t const new_n = util_exchange(other.n, 0u);
      if (this->p) {
        delete[] this->p;
      }
      this->p = new_p;
      this->n = new_n;
      return *this;
    }

    void gasp_vector::copy(gasp_vector const& other) {
      if (this == &other)
        return;
      resize(other.n);
      size_t i;
      for (i = 0; i < n; ++i) {
        p[i] = other.p[i];
      }
      return;
    }

    void gasp_vector::resize(size_t n) {
      prefix_list *ptr = nullptr;
      if (n >= util_size_limit<prefix_list>()) {
        throw std::bad_alloc();
      }
      ptr = n ? new prefix_list[n] : nullptr;
      if (this->p) {
        delete[] this->p;
      }
      this->p = ptr;
      this->n = n;
      return;
    }
#pragma endregion

#pragma region("gasp_vector / allocation")
    void* gasp_vector::operator new(std::size_t sz) {
      return ::operator new(sz);
    }

    void* gasp_vector::operator new[](std::size_t sz) {
      return ::operator new(sz);
    }

    void gasp_vector::operator delete(void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    void gasp_vector::operator delete[](void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    gasp_vector* gaspvec_new(std::size_t n) noexcept {
      try {
        return new gasp_vector(n);
      } catch (api_exception const& ) {
        return nullptr;
      } catch (std::bad_alloc const& ) {
        return nullptr;
      }
    }

    util_unique_ptr<gasp_vector> gaspvec_unique(std::size_t n) noexcept {
      return util_unique_ptr<gasp_vector>(gaspvec_new(n));
    }

    void gaspvec_destroy(gasp_vector* x) noexcept {
      if (x)
        delete x;
    }
#pragma endregion

#pragma region("gasp_vector / range-based")
    prefix_list* gasp_vector::begin(void) noexcept {
      return this->p;
    }

    prefix_list const* gasp_vector::begin(void) const noexcept {
      return this->p;
    }

    prefix_list* gasp_vector::end() noexcept {
      return this->p+this->n;
    }

    prefix_list const* gasp_vector::end() const noexcept {
      return this->p+this->n;
    }
#pragma endregion

#pragma region("gasp_vector / array-compat")
    size_t gasp_vector::size(void) const noexcept {
      return this->n;
    }

    prefix_list& gasp_vector::operator[](size_t i) noexcept {
      return this->p[i];
    }

    prefix_list const& gasp_vector::operator[](size_t i) const noexcept {
      return this->p[i];
    }
#pragma endregion
  }
}