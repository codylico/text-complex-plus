/**
 * @file text-complex-plus/access/hashchain.txx
 * @brief Sliding window of past bytes
 */
#ifndef hg_TextComplexAccessP_HashChain_Txx_
#define hg_TextComplexAccessP_HashChain_Txx_

#include "hashchain.hpp"
#include "api.hpp"

namespace text_complex {
  namespace access {
#if  (!(defined TextComplexAccessP_NO_EXCEPT))
    inline
    void hash_chain::push_front(unsigned char v) {
      api_error ae;
      push_front(v, ae);
      if (ae < api_error::Success) {
        throw api_exception(ae);
      } else return;
    }

    inline
    unsigned char const& hash_chain::at(uint32 i) const {
      if (i >= size())
        throw api_exception(api_error::OutOfRange);
      else return operator[](i);
    }
#endif //TextComplexAccessP_NO_EXCEPT
  };
};

#endif //hg_TextComplexAccessP_HashChain_Txx_
