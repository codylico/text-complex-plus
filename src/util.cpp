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
#if (defined __cpp_lib_int_pow2)
#  include <bit>
#elif (defined _MSC_VER)
#  include <intrin.h>
#endif /*_MSC_VER*/

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

    unsigned util_bitwidth(unsigned x) noexcept {
#if (defined __cpp_lib_int_pow2)
      return std::bit_width(x);
#elif (defined __GNUC__)
      return x ? std::numeric_limits<unsigned>::digits - __builtin_clz(x) : 0;
#elif (defined _MSC_VER)
      unsigned long index = 0;
      if (!x)
        return 0;
      _BitScanReverse(&index, x);
      return (unsigned)index+1u;
#else
      unsigned int y = 0;
      for (; x > 0; x >>= 1)
        y += 1;
      return y;
#endif /*bitwidth*/
    }
  };
};
