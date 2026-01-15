/**
 * @file src/blockbuf.cpp
 * @brief DEFLATE block buffer
 */
#define TCMPLX_AP_WIN32_DLL_INTERNAL
#include "text-complex-plus/access/blockbuf.hpp"
#include "text-complex-plus/access/hashchain.hpp"
#include <limits>
#include <memory>
#include <utility>
#include <new>
#include <algorithm>
#include <cstring>

namespace text_complex {
  namespace access {
    /**
     * @internal
     * Format: @verbatim
      Each command represented as a sequence of bytes
        [X][...]
      where
        X>=128 represents an insert command, and
        X<128 represents a copy command.
      Either way, the length is encoded as such:
        (X&64) == 0 -> length is (X&63)
        (X&64) == 64 -> length is ((X&63)<<8) + (Y&255) + 64
      where Y is the byte immediately following X.

      Insert commands are made up of an [X] or [X][Y] sequence
      followed by the literal bytes. A copy command is made up of
      an [X] or [X][Y] sequence followed by an encoded distance
      value.

      Distance values are represented as a sequence of bytes
        [R][...]
      where
        R<128 represents a bdict reference,
        (R&192) == 128 represents a 14-bit distance
        R>=192 represents a 30-bit distance.
      A bdict reference uses two extra bytes:
        [R][B1][B2]
      The filter is encoded in R as (R&127), the word index as (B1<<8)+B2,
      and the word length is the copy length.

      The 14-bit distance can be extracted from a two-byte sequence:
        [R][Q]
      as ((R&63)<<8) + (Q&255).
      The 30-bit distance can be extracted from a four-byte sequence:
        [R][S1][S2][S3]
      as ((R&63)<<24) + ((S1&255)<<16) + ((S2&255)<<8) + (S3&255) + 16384.
     @endverbatim
     *
     * Examples (values in hexadecimal): @verbatim
     (03)(41)(62)(63)         -> "Abc"
     (83)(80)(01)             -> copy 3 bytes, distance 1
     (01)(54)(83)(80)(01)     -> "TTTT"
     (C0)(05)(90)(02)         -> copy 69 bytes, distance 4098
     (C0)(06)(C0)(00)(00)(03) -> copy 70 bytes, distance 16387
     (84)(05)(00)(02)         -> "life the " (bdict length 4 filter 5 word 2)
     @endverbatim
     */

    static
    constexpr uint32 blockbuf_size_max =
        std::numeric_limits<size_t>::max()>0x7FffFFffu
      ? 0x7FffFFffu : std::numeric_limits<size_t>::max();
    static
    constexpr uint32 blockbuf_out_code_max = 16447u;

    /**
     * @brief Add a copy command to an output buffer.
     * @param x the output buffer
     * @param match_size the copy length
     * @param v backward distance
     * @return api_error::Success on success
     */
    static
    api_error blockstr_add_copy(block_string& x, uint32 match_size, uint32 v);
    static
    api_error blockstr_update_literal
      (block_string& x, unsigned char b, uint32 &j);

    //BEGIN blockbuf / static
    api_error blockstr_add_copy
        (block_string& x, uint32 match_size, uint32 v)
    {
      unsigned char buf[6];
      unsigned int i;
      if (match_size >= 64u) {
        uint32 const t = match_size-64u;
        buf[0] = static_cast<unsigned char>((t&63u)>>8)|192u;
        buf[1] = static_cast<unsigned char>(t&255u);
        i = 2u;
      } else {
        buf[0] = static_cast<unsigned char>(match_size | 128u);
        i = 1u;
      }
      if (v >= 16384u) {
        uint32 const t = v-16384u;
        buf[i++] = static_cast<unsigned char>(((t>>24)&63u) | 192u);
        buf[i++] = static_cast<unsigned char>((t>>16)&255u);
        buf[i++] = static_cast<unsigned char>((t>>8)&255u);
        buf[i++] = static_cast<unsigned char>(t&255u);
      } else {
        buf[i++] = static_cast<unsigned char>(((v>>8)&63u) | 128u);
        buf[i++] = static_cast<unsigned char>(v&255u);
      }
      api_error ae = api_error::Success;
      unsigned int j;
      for (j = 0u; j < i && ae == api_error::Success; ++j) {
        x.push_back(buf[j], ae);
      }
      return ae;
    }

    api_error blockstr_update_literal
        (block_string& x, unsigned char b, uint32 &j)
    {
      api_error res;
      if (j == x.size()) {
        /* add a new literal */
        x.push_back(1u, res);
        if (res != api_error::Success)
          return res;
        x.push_back(b, res);
      } else if (x[j] == 63u) {
        /* expand */
        x.push_back(1u, res);
        if (res != api_error::Success)
          return res;
        x.push_back(b, res);
        if (res != api_error::Success)
          return res;
        std::memmove(&x[j+2],&x[j+1], 63u*sizeof(unsigned char));
        x[j] = 64u;
        x[j+1u] = 0u;
      } else if (x[j] >= 64u) {
        /* long sequence */
        unsigned short int len = (x[j] << 8)|x[j+1u];
        x.push_back(b, res);
        len += 1u;
        x[j] = static_cast<unsigned char>(len>>8);
        x[j+1u] = static_cast<unsigned char>(len&255u);
        if (len == 0x7Fff)
          j = x.size();
      } else {
        x.push_back(b, res);
        x[j] += 1u;
      }
      return res;
    }
    //END   blockbuf / static

    //BEGIN block_string / rule-of-six
    block_string::block_string(void) noexcept
      : p(nullptr), sz(0u), cap(0u)
    {
    }

    block_string::~block_string(void) {
      if (p)
        delete[] p;
      p = nullptr;
      sz = 0u;
      cap = 0u;
    }

    block_string::block_string(block_string const& s)
      : p(nullptr), sz(0u), cap(0u)
    {
      duplicate(s);
      return;
    }

    block_string& block_string::operator=(block_string const& s) {
      duplicate(s);
      return *this;
    }

    block_string::block_string(block_string && s) noexcept
      : p(nullptr), sz(0u), cap(0u)
    {
      transfer(std::move(s));
      return;
    }

    block_string& block_string::operator=(block_string && s) noexcept {
      transfer(std::move(s));
      return *this;
    }

    void block_string::duplicate(block_string const& s) {
      reserve(s.sz);
      std::memcpy(p, s.p, s.sz*sizeof(unsigned char));
      return;
    }

    void block_string::transfer(block_string&& other) noexcept {
      unsigned char* const new_p = util_exchange(other.p, nullptr);
      uint32 const new_sz = util_exchange(other.sz, 0u);
      uint32 const new_cap = util_exchange(other.cap, 0u);
      /* drop old */{
        if (p)
          delete[] p;
      }
      /* configure new */{
        p = new_p;
        sz = new_sz;
        cap = new_cap;
      }
      return;
    }
    //END   block_string / rule-of-six

    //BEGIN block_string / allocation
    void* block_string::operator new(std::size_t sz) {
      return ::operator new(sz);
    }

    void* block_string::operator new[](std::size_t sz) {
      return ::operator new(sz);
    }

    void block_string::operator delete(void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    void block_string::operator delete[](void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }
    //END   block_string / allocation

    //BEGIN block_string / string-compat
    void block_string::reserve(uint32 cap, api_error &ae) noexcept {
      if (cap <= this->cap) {
        ae = api_error::Success;
        return;
      } else if (cap > blockbuf_size_max) {
        ae = api_error::Memory;
        return;
      } else try {
        unsigned char* const new_p =
          new unsigned char[static_cast<size_t>(cap)];
        std::memcpy(new_p, p, sz*sizeof(unsigned char));
        if (p)
          delete[] p;
        p = new_p;
        this->cap = cap;
        ae = api_error::Success;
        return;
      } catch (std::bad_alloc const& ) {
        ae = api_error::Memory;
        return;
      }
    }

    uint32 block_string::capacity(void) const noexcept {
      return cap;
    }

    uint32 block_string::size(void) const noexcept {
      return sz;
    }

    unsigned char const* block_string::data(void) const noexcept {
      return p;
    }

    unsigned char* block_string::data(void) noexcept {
      return p;
    }

    void block_string::clear(void) noexcept {
      sz = 0u;
      return;
    }

    void block_string::resize
      (uint32 sz, unsigned char ch, api_error &ae) noexcept
    {
      uint32 const old_size = sz;
      api_error reserve_ae;
      reserve(sz, reserve_ae);
      if (reserve_ae != api_error::Success) {
        ae = reserve_ae;
        return;
      } else if (old_size < sz) {
        std::memset(p+old_size, ch, (sz-old_size)*sizeof(unsigned char));
      }
      this->sz = sz;
      ae = api_error::Success;
      return;
    }

    void block_string::push_back(unsigned char ch, api_error &ae) noexcept {
      if (sz >= cap) {
        api_error reserve_ae;
        uint32 const new_cap = (cap == 0u) ? 1u : cap*2u;
        reserve(new_cap, reserve_ae);
        if (reserve_ae != api_error::Success) {
          ae = reserve_ae;
          return;
        }
      }
      p[sz++] = ch;
      ae = api_error::Success;
      return;
    }

    unsigned char& block_string::operator[](uint32 i) noexcept {
      return p[i];
    }

    unsigned char const& block_string::operator[](uint32 i) const noexcept {
      return p[i];
    }
    //END   block_string / string-compat

    //BEGIN block_buffer / rule-of-six
    block_buffer::block_buffer
        (uint32 block_size, uint32 n, size_t chain_length, bool bdict_tf)
      : chain(nullptr), input(), output(), use_bdict(bdict_tf),
        input_block_size(0u)
    {
      /* truncate lengths */{
        if (block_size > blockbuf_size_max/2u)
          block_size = static_cast<uint32>(blockbuf_size_max/2u);
        input_block_size = block_size;
      }
      /* allocate things */{
        std::unique_ptr<hash_chain> chain(new hash_chain(n, chain_length));
        input.reserve(block_size);
        output.reserve(block_size*2u);
        /* possible implicit throw */;
        this->chain = chain.release();
      }
      return;
    }

    block_buffer::~block_buffer(void) {
      if (chain) {
        delete chain;
        chain = nullptr;
      }
      /* `input` has destructor */;
      /* `output` has destructor */;
      return;
    }

    block_buffer::block_buffer(block_buffer&& other) noexcept
      : chain(nullptr), input(), output(), use_bdict(false),
        input_block_size(0u)
    {
      transfer(static_cast<block_buffer&&>(other));
      return;
    }

    block_buffer& block_buffer::operator=(block_buffer&& other) noexcept {
      transfer(static_cast<block_buffer&&>(other));
      return *this;
    }

    void block_buffer::transfer(block_buffer&& other) noexcept {
      hash_chain* const new_chain = util_exchange(other.chain, nullptr);
      bool const new_bdict_tf = util_exchange(other.use_bdict, false);
      uint32 const new_size = util_exchange(other.input_block_size, 0u);
      /* drop old */{
        if (chain)
          delete chain;
      }
      /* configure new */{
        chain = new_chain;
        input = std::move(other.input);
        output = std::move(other.output);
        use_bdict = new_bdict_tf;
        input_block_size = new_size;
      }
      return;
    }
    //END   block_buffer / rule-of-six

    //BEGIN block_buffer / allocation
    void* block_buffer::operator new(std::size_t sz) {
      return ::operator new(sz);
    }

    void* block_buffer::operator new[](std::size_t sz) {
      return ::operator new(sz);
    }

    void block_buffer::operator delete(void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    void block_buffer::operator delete[](void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    block_buffer* blockbuf_new
        (uint32 block_size, uint32 n, size_t chain_length, bool bdict_tf)
        noexcept
    {
      try {
        return new block_buffer(block_size, n, chain_length, bdict_tf);
      } catch (api_exception const& ) {
        return nullptr;
      } catch (std::bad_alloc const& ) {
        return nullptr;
      }
    }

    util_unique_ptr<block_buffer> blockbuf_unique
        (uint32 block_size, uint32 n, size_t chain_length, bool bdict_tf)
        noexcept
    {
      return util_unique_ptr<block_buffer>(
            blockbuf_new(block_size, n, chain_length, bdict_tf)
          );
    }

    void blockbuf_destroy(block_buffer* x) noexcept {
      if (x) {
        delete x;
      }
    }
    //END   block_buffer / allocation

    //BEGIN block_buffer / public
    uint32 block_buffer::input_size(void) const noexcept {
      return input.size();
    }

    uint32 block_buffer::capacity(void) const noexcept {
      return input_block_size;
    }

    block_string const& block_buffer::input_data(void) const noexcept {
      return input;
    }
    //END   block_buffer / public

    //BEGIN block_buffer / ostringstream-compat
    block_buffer& block_buffer::flush(api_error& ae) noexcept {
      api_error try_ae;
      try_block(try_ae);
      if (try_ae == api_error::Success)
        clear_input();
      ae = try_ae;
      return *this;
    }

    block_buffer& block_buffer::write
        (unsigned char const* s, size_t count, api_error& ae) noexcept
    {
      if (count > input_block_size-input.size()) {
        ae = api_error::BlockOverflow;
      } else {
        uint32 const old_size = input.size();
        size_t i;
        api_error input_ae = api_error::Success;
        for (i = 0u; i < count; ++i) {
          input.push_back(s[i], input_ae);
          if (input_ae != api_error::Success) {
            input.resize(old_size, 0u, input_ae);
            ae = input_ae;
            return *this;
          }
        }
        ae = api_error::Success;
      }
      return *this;
    }

    block_string const& block_buffer::str(void) const noexcept {
      return output;
    }
    //END   block_buffer / ostringstream-compat

    //BEGIN block_buffer / methods
    void block_buffer::clear_output(void) noexcept {
      output.clear();
      return;
    }

    block_buffer& block_buffer::noconv_block(api_error& ae) noexcept {
      uint32 const sz = output.size();
      uint32 const i_sz = input.size();
      api_error resize_ae;
      if (i_sz > std::numeric_limits<size_t>::max()-sz) {
        ae = api_error::Memory;
        return *this;
      }
      output.resize(sz+i_sz, 0u, resize_ae);
      if (resize_ae == api_error::Success) {
        unsigned char const* const i_data = input.data();
        std::copy(i_data, i_data+i_sz, output.data()+sz);
      }
      ae = resize_ae;
      return *this;
    }

    block_buffer& block_buffer::try_block(api_error& ae) noexcept {
      api_error res = api_error::Success;
      uint32 const n = 51u;
      uint32 j = output.size();
      uint32 i;
      uint32 const input_sz = input.size();
      uint32 other_v = 0u;
      uint32 v = 0u;
      uint32 match_size = 0u;
      uint32 other_match_size = 0u;
      unsigned char state = 0;
      unsigned char skipped_byte = 0u;
      for (i = 0u; i < input_sz && res == api_error::Success; ++i) {
        switch (state) {
        case 2:
          /* two things */{
            unsigned char const q = (*chain)[v];
            unsigned char const other_q = (*chain)[other_v];
            if (q != input[i]) {
              res = blockstr_update_literal(output, skipped_byte, j);
              v = other_v;
              match_size = other_match_size;
              state = 1;
            } else if (other_q != input[i]
                || match_size >= blockbuf_out_code_max)
            {
              state = 1;
            } else {
              match_size += 1u;
              other_match_size += 1u;
              chain->push_front(input[i], res);
              break;
            }
          }
          if (res != api_error::Success)
            break;
          /* [[fallthrough]] */;
        case 1: /* one thing */
          {
            unsigned int const q = (*chain)[v];
            if (q == input[i] && match_size < blockbuf_out_code_max) {
              match_size += 1u;
              chain->push_front(input[i], res);
              break;
            } else {
              /* close the match */
              res = blockstr_add_copy(output, match_size, v);
              if (res != api_error::Success)
                break;
              else j = output.size();
              state = 0;
            }
          }
          /* [[fallthrough]] */;
        case 0:
          if (i <= input_sz - 4u && chain) {
            /* look for the item in the chain */
            v = chain->find(&input[i]);
            if (v != hash_chain::npos) {
              other_v = chain->find(&input[i+1]);
              if (other_v != hash_chain::npos) {
                other_v += 1u;
                other_match_size = 2u;
                state = 2;
              } else {
                other_match_size = 0u;
                state = 1;
              }
              match_size = 3u;
              skipped_byte = input[i];
              chain->push_front(input[i], res);
              if (res != api_error::Success)
                break;
              chain->push_front(input[i+1u], res);
              if (res != api_error::Success)
                break;
              chain->push_front(input[i+2u], res);
              i += 2u;
              break;
            }
          }
          /* literal */{
            res = blockstr_update_literal(output, input[i], j);
            if (res != api_error::Success)
              break;
            chain->push_front(input[i], res);
          } break;
        }
      }
      if (res == api_error::Success && state > 0) {
        /* close the match */
        res = blockstr_add_copy(output, match_size, v);
      }
      ae = res;
      return *this;
    }

    void block_buffer::clear_input(void) noexcept {
      input.clear();
      return;
    }
    //END   block_buffer / methods

    //BEGIN block_buffer / slide_ring access
    size_t block_buffer::bypass
        (unsigned char const* s, size_t count, api_error& ae) noexcept
    {
      if (chain) {
        api_error chain_ae = api_error::Success;
        size_t i;
        for (i = 0u; i < count && chain_ae == api_error::Success; ++i) {
          chain->push_front(s[i], chain_ae);
        }
        return i;
      } else {
        ae = api_error::Init;
        return 0u;
      }
    }

    unsigned char block_buffer::peek
        (uint32 i, api_error& ae) const noexcept
    {
      if (chain) {
        if (i >= chain->size()) {
          ae = api_error::OutOfRange;
          return 0u;
        } else {
          ae = api_error::Success;
          return (*chain)[i];
        }
      } else {
        ae = api_error::OutOfRange;
        return 0u;
      }
    }

    uint32 block_buffer::extent(void) const noexcept {
      return chain ? chain->extent() : 0u;
    }

    uint32 block_buffer::ring_size(void) const noexcept {
      return chain ? chain->size() : 0u;
    }
    //BEGIN block_buffer / slide_ring access
  };
};
