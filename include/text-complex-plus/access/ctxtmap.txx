/**
 * @file tcmplx-access-plus/ctxtmap.txx
 * @brief Context map for compressed streams
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_CtxtMap_Txx_
#define hg_TextComplexAccessP_CtxtMap_Txx_

#include "ctxtmap.hpp"

namespace text_complex {
  namespace access {
#if  (!(defined TextComplexAccessP_NO_EXCEPT))
    inline
    size_t ctxtmap_distance_context(unsigned long int copylen) {
      api_error ae;
      size_t const out = ctxtmap_distance_context(copylen, ae);
      if (ae < api_error::Success) {
        throw api_exception(ae);
      } else return out;
    }

    inline
    size_t ctxtmap_literal_context
      (context_map_mode mode, unsigned char p1, unsigned char p2)
    {
      api_error ae;
      size_t const out = ctxtmap_literal_context(mode,p1,p2, ae);
      if (ae < api_error::Success) {
        throw api_exception(ae);
      } else return out;
    }

    inline
    context_map_mode operator++(context_map_mode& mode) noexcept {
      return mode = static_cast<context_map_mode>(static_cast<unsigned>(mode)+1);
    }
    inline
    context_map_mode operator++(context_map_mode& mode, int) noexcept {
      context_map_mode old = mode;
      mode = static_cast<context_map_mode>(static_cast<unsigned>(mode)+1);
      return old;
    }

    inline context_map_mode context_map::get_mode(std::size_t i) const {
      api_error ae;
      context_map_mode const out = get_mode(i, ae);
      if (ae < api_error::Success) {
        throw api_exception(ae);
      } else return out;
    }
    inline void context_map::set_mode(std::size_t i, context_map_mode v) {
      api_error ae;
      set_mode(i, v, ae);
      if (ae < api_error::Success) {
        throw api_exception(ae);
      } else return;
    }
#endif //TextComplexAccessP_NO_EXCEPT
  };
};

#endif //hg_TextComplexAccessP_CtxtMap_Txx_
