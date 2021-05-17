/**
 * @file text-complex-plus/access/blockbuf.txx
 * @brief DEFLATE block buffer
 */
#ifndef hg_TextComplexAccessP_BlockBuf_Txx_
#define hg_TextComplexAccessP_BlockBuf_Txx_

#include "blockbuf.hpp"
#include "api.hpp"

namespace text_complex {
  namespace access {
#if  (!(defined TextComplexAccessP_NO_EXCEPT))
    inline
    void block_string::reserve(uint32 v) {
      api_error ae;
      reserve(v, ae);
      if (ae < api_error::Success) {
        throw api_exception(ae);
      } else return;
    }

    inline
    void block_string::resize(uint32 v, unsigned char w) {
      api_error ae;
      resize(v, w, ae);
      if (ae < api_error::Success) {
        throw api_exception(ae);
      } else return;
    }

    inline
    void block_string::push_back(unsigned char w) {
      api_error ae;
      push_back(w, ae);
      if (ae < api_error::Success) {
        throw api_exception(ae);
      } else return;
    }

    inline
    block_buffer& block_buffer::flush(void) {
      api_error ae;
      flush(ae);
      if (ae < api_error::Success) {
        throw api_exception(ae);
      } else return *this;
    }

    inline
    block_buffer& block_buffer::write(unsigned char const* s, size_t count) {
      api_error ae;
      write(s, count, ae);
      if (ae < api_error::Success) {
        throw api_exception(ae);
      } else return *this;
    }
#endif //TextComplexAccessP_NO_EXCEPT
  };
};

#endif //hg_TextComplexAccessP_BlockBuf_Txx_
