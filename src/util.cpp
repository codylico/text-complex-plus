/*
 * \file tcmplx-access-plus/util.cpp
 * \brief Utility functions for `text-complex-plus`
 * \author Cody Licorish (svgmovement@gmail.com)
 */
#define TCMPLX_AP_WIN32_DLL_INTERNAL
#include "text-complex-plus/access/util.hpp"
#include <new>
#include <limits>
#include <cstring>
#include <ios>

namespace text_complex {
  namespace access {
    //BEGIN allocation
    void* util_op_new(std::size_t sz) noexcept {
      if (sz == 0u)
        return nullptr;
      else try {
        return ::operator new(sz);
      } catch (std::bad_alloc const& ) {
        return nullptr;
      }
    }

    void* util_op_new_count(std::size_t n, std::size_t sz ) noexcept {
      if (n == 0u) {
        return nullptr;
      } else if (sz > std::numeric_limits<size_t>::max()/n) {
        return nullptr;
      } else return util_op_new(n*sz);
    }

    void util_op_delete(void* p) noexcept {
      if (p == nullptr)
        return;
      else ::operator delete(p);
    }
    //END   allocation

    //BEGIN char array
    void util_memmove(void* dst, void const* src, size_t sz) {
      std::memmove(dst,src,sz);
      return;
    }
    //END   char array

    //BEGIN limits
    size_t util_ssize_max(void) {
      return static_cast<size_t>(std::numeric_limits<std::streamsize>::max());
    }

    long int util_long_max(void) {
      return std::numeric_limits<long int>::max();
    }

    long int util_long_min(void) {
      return std::numeric_limits<long int>::min();
    }
    //END   limits
  };
};
