/*
 * \file tcmplx-access-plus/util.cpp
 * \brief Utility functions for `text-complex-plus`
 * \author Cody Licorish (svgmovement@gmail.com)
 */
#define TCMPLX_AP_WIN32_DLL_INTERNAL
#include "util.hpp"
#include <new>

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

    void util_op_delete(void* p) noexcept {
      if (p == nullptr)
        return;
      else ::operator delete(p);
    }
    //END   allocation
  };
};
