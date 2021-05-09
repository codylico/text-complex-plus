/*
 * \file tcmplx-access/util.txx
 * \brief Utility functions for `text-complex-plus`
 * \author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_Util_Txx_
#define hg_TextComplexAccessP_Util_Txx_

#include "util.hpp"

namespace text_complex {
  namespace access {
    //BEGIN allocation
    template <typename t, typename ...u>
    void* util_op_new_type(std::size_t n, u&& ...args)
      noexcept(noexcept(t(util_declval<u>()...)))
    {
      t* out = util_op_new_count(n, sizeof(t));
      if (out) {
        if (noexcept(t(util_declval<u>()...))) {
          std::size_t i;
          for (i = 0u; i < n; ++i) {
            new (out+i) t(args...);
          }
        } else {
          std::size_t i;
          for (i = 0u; i < n; ++i) {
            try {
              new (out+i) t(args...);
            } catch (...) {
              std::size_t j;
              for (j = i; j > 0u; --j) {
                out[j-1u].~t();
              }
              util_op_delete(out);
              throw;
            }
          }
        }
      }
      return out;
    }

    template <typename t>
    void util_op_delete_type(std::size_t n, t* p) noexcept {
      std::size_t i;
      for (i = 0u; i < n; ++i) {
        p[i].~t();
      }
      util_op_delete(p);
      return;
    }
    //END   allocation

    //BEGIN util_unique_ptr<t> / rule-of-six
    template <typename t>
    constexpr util_unique_ptr<t>::util_unique_ptr(void) noexcept
        : p(nullptr)
    {
    }

    template <typename t>
    constexpr util_unique_ptr<t>::util_unique_ptr(std::nullptr_t) noexcept
        : p(nullptr)
    {
    }

    template <typename t>
    util_unique_ptr<t>::util_unique_ptr(t* r) noexcept
        : p(r)
    {
      return;
    }

    template <typename t>
    util_unique_ptr<t>::util_unique_ptr(util_unique_ptr<t>&& r) noexcept
        : p(r.release())
    {
      return;
    }

    template <typename t>
    util_unique_ptr<t>& util_unique_ptr<t>::operator=
        (util_unique_ptr<t>&& r) noexcept
    {
      reset(r.release());
      return *this;
    }

    template <typename t>
    template <typename u, typename v>
    util_unique_ptr<t>::util_unique_ptr(u&& r)
        noexcept(noexcept(util_declval<u>().release()))
        : p(r.release())
    {
      return;
    }

    template <typename t>
    template <typename u, typename v>
    util_unique_ptr<t>& util_unique_ptr<t>::operator=(u&& r)
        noexcept(noexcept(util_declval<u>().release()))
    {
      reset(r.release());
      return *this;
    }

    template <typename t>
    util_unique_ptr<t>::~util_unique_ptr(void) noexcept {
      if (p) {
        delete p;
        p = nullptr;
      }
      return;
    }

    template <typename t>
    template <typename u, typename v>
    util_unique_ptr<t>::operator u(void) &&
      noexcept(noexcept(u().reset(util_declval<t*>())))
    {
      u out;
      static_cast<v>(out.reset(release()) );
      return out;
    }

    template <typename t>
    t* util_unique_ptr<t>::release(void)
        noexcept
    {
      pointer out = p;
      p = nullptr;
      return out;
    }

    template <typename t>
    void util_unique_ptr<t>::reset(t* r)
        noexcept
    {
      if (p) {
        delete p;
      }
      p = r;
      return;
    }
    //END   util_unique_ptr<t> / rule-of-six

    //BEGIN util_unique_ptr<t> / unique_ptr-matching
    template <typename t>
    util_unique_ptr<t>::operator bool(void) const noexcept {
      return p != nullptr;
    }

    template <typename t>
    t& util_unique_ptr<t>::operator*(void) const
        noexcept
    {
      return *p;
    }
    template <typename t>
    t* util_unique_ptr<t>::operator->(void) const noexcept {
      return p;
    }
    template <typename t>
    t* util_unique_ptr<t>::get(void) const noexcept {
      return p;
    }
    //END   util_unique_ptr<t> / unique_ptr-matching

    //BEGIN util_unique_ptr<t[]> / rule-of-six
    template <typename t>
    constexpr util_unique_ptr<t[]>::util_unique_ptr(void) noexcept
        : p(nullptr)
    {
    }

    template <typename t>
    constexpr util_unique_ptr<t[]>::util_unique_ptr(std::nullptr_t) noexcept
        : p(nullptr)
    {
    }

    template <typename t>
    util_unique_ptr<t[]>::util_unique_ptr
        (util_unique_ptr<t[]>::pointer r) noexcept
        : p(r)
    {
      return;
    }

    template <typename t>
    util_unique_ptr<t[]>::util_unique_ptr(util_unique_ptr<t[]>&& r) noexcept
        : p(r.release())
    {
      return;
    }

    template <typename t>
    util_unique_ptr<t[]>& util_unique_ptr<t[]>::operator=
        (util_unique_ptr<t[]>&& r) noexcept
    {
      reset(r.release());
      return *this;
    }

    template <typename t>
    template <typename u, typename v>
    util_unique_ptr<t[]>::util_unique_ptr(u&& r)
        noexcept(noexcept(util_declval<u>().release()))
        : p(static_cast<v>(r.release()))
    {
      return;
    }

    template <typename t>
    template <typename u, typename v>
    util_unique_ptr<t[]>& util_unique_ptr<t[]>::operator=(u&& r)
        noexcept(noexcept(util_declval<u>().release()))
    {
      reset(r.release());
      return *this;
    }

    template <typename t>
    util_unique_ptr<t[]>::~util_unique_ptr(void) noexcept {
      if (p) {
        delete[] p;
        p = nullptr;
      }
      return;
    }

    template <typename t>
    template <typename u, typename v>
    util_unique_ptr<t[]>::operator u(void) &&
      noexcept(noexcept(u().reset(util_declval<t*>())))
    {
      u out;
      static_cast<v>(out.reset(release()) );
      return out;
    }

    template <typename t>
    t* util_unique_ptr<t[]>::release(void) noexcept {
      pointer out = p;
      p = nullptr;
      return out;
    }

    template <typename t>
    void util_unique_ptr<t[]>::reset(t* r) noexcept {
      if (p) {
        delete[] p;
      }
      p = r;
      return;
    }
    //END   util_unique_ptr<t> / rule-of-six

    //BEGIN util_unique_ptr<t[]> / unique_ptr-matching
    template <typename t>
    util_unique_ptr<t[]>::operator bool(void) const noexcept
    {
      return p != nullptr;
    }

    template <typename t>
    t& util_unique_ptr<t[]>::operator[](size_t i) const noexcept
    {
      return p[i];
    }
    template <typename t>
    t* util_unique_ptr<t[]>::get(void) const noexcept {
      return p;
    }
    //END   util_unique_ptr<t[]> / unique_ptr-matching

    //BEGIN utility templates
    template <typename t, typename ...a>
    util_unique_ptr<t> util_make_unique(a&&... x) {
      return util_unique_ptr<t>(new t(static_cast<a&&>(x)...));
    };
    //END   utility templates

    //BEGIN utility tools
    template <typename t, typename u>
    t util_exchange(t& x, u&& y) {
      t old(static_cast<t&&>(x));
      x = static_cast<u&&>(y);
      return old;
    }
    //END   utility tools
  };
};

#endif /*hg_TextComplexAccessP_Util_Txx_*/
