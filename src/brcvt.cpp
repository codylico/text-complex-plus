/**
 * @file src/brcvt.cpp
 * @brief Brotli conversion state
 */
#define TCMPLX_AP_WIN32_DLL_INTERNAL
#include "text-complex-plus/access/brcvt.hpp"
#include "text-complex-plus/access/zutil.hpp"
#include <limits>
#include <new>
#include <algorithm>
#include <cassert>

namespace text_complex {
  namespace access {
    static
    api_error brcvt_in_bits
      ( brcvt_state& state, unsigned char y,
        unsigned char* to, unsigned char* to_end,
        unsigned char*& to_next);
    /**
     * @internal
     * @brief Bit iteration.
     * @param state the Brotli conversion state to use
     * @param[out] y bits to iterate
     * @param from source buffer
     * @param from_end pointer to end of source buffer
     * @param[out] from_next location of next byte to process
     * @return api_error::Success on success, api_error::Partial on
     *   incomplete bit sequence
     * @note The conversion state referred to by `state` is updated based
     *   on the conversion result, whether succesful or failed.
     */
    static
    api_error brcvt_out_bits
      ( brcvt_state& state,
        unsigned char const* from, unsigned char const* from_end,
        unsigned char const*& from_next,
        unsigned char& y);
    static
    unsigned char brcvt_clen[19] =
      {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
    static
    uint32 brcvt_cinfo(uint32 window_size);
    static
    bool brcvt_out_has_dictionary(brcvt_state const& state);
    static
    api_error brcvt_post_two
      (block_string& s, unsigned int x1, unsigned int x2) noexcept;
    /**
     * @brief Choose a failure code.
     * @param a one code
     * @param b another code
     * @return either `a` or `b`, whichever is nonzero
     */
    static api_error brcvt_nonzero(api_error a, api_error b) noexcept;
    /**
     * @brief Choose a failure code.
     * @param a one code
     * @param b another code
     * @param c another code
     * @return either `a` or `b` or `c`, whichever is nonzero
     */
    static api_error brcvt_nonzero
      (api_error a, api_error b, api_error c) noexcept;
    /**
     * @brief Make a code length sequence.
     * @param x the brcvt state
     * @return tcmplxA_Success on success, nonzero otherwise
     */
    static api_error brcvt_make_sequence(brcvt_state& state) noexcept;
    /**
     * @brief Add a code length sequence.
     * @param s sequence list
     * @param len code length
     * @param count repeat count
     * @return tcmplxA_Success on success, nonzero otherwise
     */
    static api_error brcvt_post_sequence
      (block_string& s, unsigned int len, unsigned int count) noexcept;

    //BEGIN brcvt / static
    api_error brcvt_in_bits
      ( brcvt_state& state, unsigned char y,
        unsigned char* to, unsigned char* to_end,
        unsigned char*& to_next)
    {
      unsigned int i;
      api_error ae = api_error::Success;
      for (i = state.bit_index; i < 8u && ae == api_error::Success; ++i) {
        unsigned int x = (y>>i)&1u;
        switch (state.state) {
        case 0:
          if (state.bit_length == 0)
            fixlist_codesort(state.wbits);
          if (state.bit_length < 7u) {
            state.bits = (state.bits<<1) | x;
            state.bit_length += 1;
            std::size_t const j = fixlist_codebsearch
              (state.wbits, state.bit_length, state.bits);
            if (j < 16) {
              state.wbits_select =
                static_cast<unsigned char>(state.wbits[j].value);
              state.state = 7;
              break;
            }
          }
          if (state.bit_length >= 7)
            ae = api_error::Sanitize;
          break;
        case 7: /* end of stream */
          ae = api_error::EndOfFile;
          break;
        case 3:
          if (state.count < 3u) {
            state.bits |= (x<<state.count);
            state.count += 1u;
          }
          if (state.count >= 3u) {
            unsigned int const end = state.bits&1u;
            unsigned int const btype = (state.bits>>1)&3u;
            if (btype == 3u) {
              ae = api_error::Sanitize;
            } else {
              state.h_end = end;
              if (btype == 0u) {
                state.state = 4;
              } else if (btype == 1u) {
                state.state = 8;
                /* fixed Huffman codes */{
                  size_t i;
                  for (i = 0u; i < 288u; ++i) {
                    prefix_line& line = state.literals[i];
                    line.value = static_cast<unsigned long int>(i);
                    if (i < 144u)
                      line.len = 8u;
                    else if (i < 256u)
                      line.len = 9u;
                    else if (i < 280u)
                      line.len = 7u;
                    else line.len = 8u;
                  }
                  for (i = 0u; i < 32u; ++i) {
                    prefix_line& line = state.distances[i];
                    line.code = static_cast<unsigned short>(i);
                    line.len = 5u;
                    line.value = static_cast<unsigned long int>(i);
                  }
                  api_error toss_ae;
                  fixlist_gen_codes(state.literals, toss_ae);
                  fixlist_codesort(state.literals, toss_ae);
                }
              } else state.state = 13;
              state.count = 0u;
              state.bits = 0u;
              state.backward = 0;
              state.bit_length = 0u;
            }
          }
          break;
        case 4: /* no compression: LEN and NLEN */
        case 6: /* */
          break;
        case 19: /* generate code trees */
          {
            fixlist_gen_codes(state.literals, ae);
            if (ae != api_error::Success)
              break;
            fixlist_codesort(state.literals, ae);
            if (ae != api_error::Success)
              break;
            fixlist_gen_codes(state.distances, ae);
            if (ae != api_error::Success)
              break;
            fixlist_codesort(state.distances, ae);
            if (ae != api_error::Success)
              break;
            state.state = 8;
          } [[fallthrough]];
        case 8: /* decode */
          if (state.bit_length < 15u) {
            state.bits = (state.bits<<1) | x;
            size_t const j = fixlist_codebsearch
              (state.literals, state.bit_length+1u, state.bits);
            if (j < std::numeric_limits<size_t>::max()) {
              unsigned const alpha =
                static_cast<unsigned>(state.literals[j].value);
              struct insert_copy_row& row = state.values[alpha];
              if (row.type == insert_copy_type::Stop) {
                if (state.h_end)
                  state.state = 6;
                else state.state = 3;
                state.count = 0u;
              } else if (row.type == insert_copy_type::Literal) {
                if (to_next < to_end) {
                  unsigned char const byt = static_cast<unsigned char>(alpha);
                  (*to_next) = byt;
                  state.checksum = zutil_adler32(1u, &byt, state.checksum);
                  to_next += 1u;
                  state.buffer.bypass(&byt, 1u, ae);
                  if (ae != api_error::Success)
                    break;
                } else {
                  state.state = 20;
                  state.bits = static_cast<unsigned char>(alpha);
                  ae = api_error::Partial;
                  break;
                }
              } else if (row.type == insert_copy_type::Copy
                  ||  row.type == insert_copy_type::CopyMinus1)
              {
                if (row.copy_bits > 0u) {
                  state.state = 9;
                  state.extra_length = row.copy_bits;
                } else state.state = 10;
                state.count = row.copy_first;
              } else ae = api_error::Sanitize;
              state.bit_length = 0u;
              state.backward = 0u;
              state.bits = 0u;
              break;
            }
            state.bit_length += 1u;
          }
          if (state.bit_length >= 15u) {
            ae = api_error::Sanitize;
          } break;
        case 20: /* alpha bringback */
          if (to_next < to_end) {
            unsigned char const byt = static_cast<unsigned char>(state.bits);
            (*to_next) = byt;
            state.checksum = zutil_adler32(1u, &byt, state.checksum);
            to_next += 1u;
            state.buffer.bypass(&byt, 1u, ae);
            state.state = 8;
            state.bits = 0u;
            state.bit_length = 0u;
            state.backward = 0u;
          } else {
            ae = api_error::Partial;
          } break;
        case 9: /* copy bits */
          if (state.bit_length < state.extra_length) {
            state.bits = (state.bits | (x<<state.bit_length));
            state.bit_length += 1u;
          }
          if (state.bit_length >= state.extra_length) {
            state.count += state.bits;
            state.bits = 0u;
            state.bit_length = 0u;
            state.state = 10;
          } break;
        case 10: /* backward */
          if (state.bit_length < 15u) {
            state.bits = (state.bits<<1) | x;
            size_t const j = fixlist_codebsearch
              (state.distances, state.bit_length+1u, state.bits);
            if (j < std::numeric_limits<size_t>::max()) {
              unsigned const alpha =
                static_cast<unsigned>(state.distances[j].value);
              unsigned int const extra_length = state.ring.bit_count(alpha);
              state.index = 0u;
              if (extra_length > 0u) {
                state.extra_length = extra_length;
                state.state = 11;
                state.backward = alpha;
              } else {
                state.state = 12;
                state.backward = state.ring.decode(alpha, 0u, ae);
                if (ae < api_error::Success)
                  break;
                else state.backward -= 1u;
                for (; state.index < state.count && to_next < to_end;
                    ++state.index)
                {
                  unsigned char const byt = state.buffer.peek(state.backward);
                  (*to_next) = byt;
                  state.checksum = zutil_adler32(1u, &byt, state.checksum);
                  to_next += 1u;
                  state.buffer.bypass(&byt, 1u, ae);
                  if (ae != api_error::Success)
                    break;
                }
                if (state.index >= state.count) {
                  state.state = 8;
                  state.count = 0u;
                } else ae = api_error::Partial;
              }
              state.bit_length = 0u;
              state.bits = 0u;
              break;
            }
            state.bit_length += 1u;
          }
          if (state.bit_length >= 15u) {
            ae = api_error::Sanitize;
          } break;
        case 11: /* distance bits */
          if (state.bit_length < state.extra_length) {
            state.bits = (state.bits | (x<<state.bit_length));
            state.bit_length += 1u;
          }
          if (state.bit_length >= state.extra_length) {
            state.backward = state.ring.decode(state.backward, state.bits, ae);
            if (ae < api_error::Success)
              break;
            else state.backward -= 1u;
            state.bits = 0u;
            state.bit_length = 0u;
            state.state = 12;
            state.index = 0u;
          } else break;
          [[fallthrough]];
        case 12: /* output from backward */
          for (; state.index < state.count && to_next < to_end;
              ++state.index)
          {
            unsigned char const byt = state.buffer.peek(state.backward, ae);
            if (ae != api_error::Success)
              break;
            (*to_next) = byt;
            state.checksum = zutil_adler32(1u, &byt, state.checksum);
            to_next += 1u;
            state.buffer.bypass(&byt, 1u, ae);
            if (ae != api_error::Success)
              break;
          }
          if (state.index >= state.count) {
            state.state = 8;
            state.bits = 0u;
            state.bit_length = 0u;
            state.count = 0u;
          } else ae = api_error::Partial;
          break;
        case 13: /* hcounts */
          if (state.bit_length < 14u) {
            state.bits = (state.bits | (x<<state.bit_length));
            state.bit_length += 1u;
          }
          if (state.bit_length >= 14u) {
            unsigned int const hclen = ((state.bits>>10)&15u) + 4u;
            state.backward = state.bits&1023u;
            state.bits = 0u;
            state.bit_length = 0u;
            state.count = hclen;
            state.state = 14;
            state.index = 0u;
            /* */{
              size_t j;
              for (j = 0u; j < 19u; ++j) {
                prefix_line& line = state.sequence[j];
                line.value = static_cast<unsigned long int>(j);
                line.len = 0u;
              }
            }
          } break;
        case 14: /* code lengths code lengths */
          if (state.bit_length < 3u) {
            state.bits = (state.bits | (x<<state.bit_length));
            state.bit_length += 1u;
          }
          if (state.bit_length >= 3u) {
            state.sequence[brcvt_clen[state.index]].len = (state.bits&7u);
            state.index += 1u;
            state.bits = 0u;
            state.bit_length = 0u;
          }
          if (state.index >= state.count) {
            state.index = 0u;
            state.state = 15;
            state.count = (state.backward&31u)+257u;
            state.backward = ((state.backward>>5)&31u) + 1u + state.count;
            state.extra_length = 0u;
            /* dynamic Huffman codes */{
              size_t j;
              for (j = 0u; j < 288u; ++j) {
                prefix_line& line = state.literals[j];
                line.value = static_cast<unsigned long int>(j);
                line.len = 0u;
              }
              for (j = 0u; j < 32u; ++j) {
                prefix_line& line = state.distances[j];
                line.len = 0u;
                line.value = static_cast<unsigned long int>(j);
              }
            }
            fixlist_gen_codes(state.sequence, ae);
            if (ae != api_error::Success)
              break;
            else fixlist_codesort(state.sequence, ae);
          } break;
        case 15: /* literals and distances */
          if (state.bit_length < 15u) {
            state.bits = (state.bits<<1) | x;
            size_t const j = fixlist_codebsearch
              (state.sequence, state.bit_length+1u, state.bits);
            if (j < std::numeric_limits<size_t>::max()) {
              unsigned const alpha =
                static_cast<unsigned>(state.sequence[j].value);
              if (alpha <= 15u) {
                if (state.index >= state.count)
                  state.distances[state.index-state.count].len = alpha;
                else state.literals[state.index].len = alpha;
                state.extra_length = alpha;
                state.index += 1u;
              } else state.state = alpha;
              state.bit_length = 0u;
              state.bits = 0u;
            } else state.bit_length += 1u;
          }
          if (state.bit_length >= 15u) {
            ae = api_error::Sanitize;
          } else if (state.index >= state.backward) {
            state.state = 19;
            state.count = 0u;
            state.index = 0u;
            state.backward = 0u;
            state.extra_length = 0u;
          } break;
        case 16: /* copy previous code length */
          if (state.bit_length < 2u) {
            state.bits = (state.bits | (x<<state.bit_length));
            state.bit_length += 1u;
          }
          if (state.bit_length >= 2u) {
            unsigned int const n = state.bits+3u;
            unsigned int j;
            for (j = 0u; j < n && state.index < state.backward;
                ++j, ++state.index)
            {
              unsigned int const alpha = state.extra_length;
              if (state.index >= state.count)
                state.distances[state.index-state.count].len = alpha;
              else state.literals[state.index].len = alpha;
            }
            if (j < n)
              ae = api_error::Sanitize;
            else if (state.index >= state.backward)
              state.state = 19;
            else state.state = 15;
            state.bits = 0u;
            state.bit_length = 0u;
          } break;
        case 17: /* copy zero length */
          if (state.bit_length < 3u) {
            state.bits = (state.bits | (x<<state.bit_length));
            state.bit_length += 1u;
          }
          if (state.bit_length >= 3u) {
            unsigned int const n = state.bits+3u;
            unsigned int j;
            for (j = 0u; j < n && state.index < state.backward;
                ++j, ++state.index)
            {
              if (state.index >= state.count)
                state.distances[state.index-state.count].len = 0u;
              else state.literals[state.index].len = 0u;
            }
            state.extra_length = 0u;
            if (j < n)
              ae = api_error::Sanitize;
            else if (state.index >= state.backward)
              state.state = 19;
            else state.state = 15;
            state.bits = 0u;
            state.bit_length = 0u;
          } break;
        case 18: /* copy zero length + 11 */
          if (state.bit_length < 7u) {
            state.bits = (state.bits | (x<<state.bit_length));
            state.bit_length += 1u;
          }
          if (state.bit_length >= 7u) {
            unsigned int const n = state.bits+11u;
            unsigned int j;
            for (j = 0u; j < n && state.index < state.backward;
                ++j, ++state.index)
            {
              if (state.index >= state.count)
                state.distances[state.index-state.count].len = 0u;
              else state.literals[state.index].len = 0u;
            }
            state.extra_length = 0u;
            if (j < n)
              ae = api_error::Sanitize;
            else if (state.index >= state.backward)
              state.state = 19;
            else state.state = 15;
            state.bits = 0u;
            state.bit_length = 0u;
          } break;
        }
        if (ae > api_error::Success)
          /* halt the read position here: */break;
      }
      state.bit_index = i&7u;
      return ae;
    }

    unsigned int brcvt_cinfo(uint32 window_size) {
      unsigned int out = 0u;
      uint32 v;
      if (window_size > 32768u)
        return 8u;
      else for (v = 1u; v < window_size; v<<=1, ++out) {
        continue;
      }
      return out > 8u ? out-8u : 0u;
    }

    bool brcvt_out_has_dictionary(brcvt_state const& state) {
      return state.buffer.ring_size() > 0u;
    }

    api_error brcvt_nonzero(api_error a, api_error b) noexcept {
      return a!=api_error::Success ? a : b;
    }
    api_error brcvt_nonzero(api_error a, api_error b, api_error c) noexcept {
      return brcvt_nonzero(a,brcvt_nonzero(b,c));
    }

    api_error brcvt_make_sequence(brcvt_state& state) noexcept {
      unsigned int len = ~0u, len_count = 0u;
      size_t const lit_sz = state.literals.size();
      size_t const dist_sz = state.distances.size();
      size_t i;
      state.sequence_list.clear();
      for (i = 0u; i < lit_sz; ++i) {
        unsigned int const n = state.literals[i].len;
        if (len != n) {
          api_error const ae = brcvt_post_sequence
            (state.sequence_list, len, len_count);
          if (ae != api_error::Success)
            return ae;
          else {
            len = n;
            len_count = 1u;
          }
        }
      }
      for (i = 0u; i < dist_sz; ++i) {
        unsigned int const n = state.distances[i].len;
        if (len != n) {
          api_error const ae = brcvt_post_sequence
            (state.sequence_list, len, len_count);
          if (ae != api_error::Success)
            return ae;
          else {
            len = n;
            len_count = 1u;
          }
        }
      }
      return brcvt_post_sequence(state.sequence_list, len, len_count);
    }

    api_error brcvt_post_two
      (block_string& s, unsigned int x1, unsigned int x2) noexcept
    {
      api_error append_ae[2];
      s.push_back(static_cast<unsigned char>(x1), append_ae[0]);
      s.push_back(static_cast<unsigned char>(x2), append_ae[1]);
      return brcvt_nonzero(append_ae[0], append_ae[1]);
    }

    api_error brcvt_post_sequence
      (block_string& s, unsigned int len, unsigned int count) noexcept
    {
      if (count == 0u)
        return api_error::Success;
      else if ((len==0u && count < 3u) || count < 4u) {
        unsigned int i;
        for (i = 0u; i < count; ++i) {
          api_error ae;
          s.push_back(static_cast<unsigned char>(len), ae);
          if (ae != api_error::Success)
            return ae;
        }
        return api_error::Success;
      } else if (len == 0u) {
        unsigned int i;
        api_error ae = api_error::Success;
        for (i = 0u; i < len && ae == api_error::Success; ) {
          unsigned int const x = len-i;
          if (x > 138u) {
            ae = brcvt_post_two(s, 18u, 127u);
            i += 138u;
          } else if (x >= 11u) {
            ae = brcvt_post_two(s, 18u, x-11u);
            i += x;
          } else if (x >= 3u) {
            ae = brcvt_post_two(s, 17u, x-3u);
            i += x;
          } else {
            s.push_back(0u, ae);
            i += 1u;
          }
        }
        return ae;
      } else {
        unsigned int i;
        api_error ae;
        s.push_back(static_cast<unsigned char>(len), ae);
        for (i = 1u; i < len && ae == api_error::Success; ) {
          unsigned int const x = len-i;
          if (x > 6u) {
            ae = brcvt_post_two(s, 16u, 3u);
            i += 6u;
          } else if (x >= 3u) {
            ae = brcvt_post_two(s, 17u, x-3u);
            i += x;
          } else {
            s.push_back(static_cast<unsigned char>(len), ae);
            i += 1u;
          }
        }
        return ae;
      }
    }

    api_error brcvt_out_bits
      ( brcvt_state& state,
        unsigned char const* from, unsigned char const* from_end,
        unsigned char const*& p, unsigned char& y)
    {
      unsigned int i;
      api_error ae = api_error::Success;
      for (i = state.bit_index; i < 8u && ae == api_error::Success; ++i) {
        unsigned int x = 0u;
        if ((!(state.h_end&1u))/* if end marker not activated yet */
        &&  (state.state == 3)/* and not inside a block */)
        {
          uint32 const input_space =
              state.buffer.capacity() - state.buffer.input_size();
          std::size_t const from_count = from_end - p;
          std::size_t const min_count = (input_space < from_count)
            ? static_cast<std::size_t>(input_space) : from_count;
          state.buffer.write(p, min_count, ae);
          if (ae != api_error::Success)
            break;
          else state.checksum = zutil_adler32(min_count, p, state.checksum);
        }
        switch (state.state) {
        case 0: /* WBITS */
          if (state.bit_length == 0u) {
            prefix_list& wbits = state.wbits;
            fixlist_valuesort(wbits);
            assert(state.wbits_select >= 10 && state.wbits_select <= 24);
            prefix_line const& line = wbits[state.wbits_select-10];
            state.bit_length = line.len;
            state.count = 1u;
            state.bits = line.code;
          }
          if (state.count <= state.bit_length) {
            x = (state.bits>>(state.bit_length-state.count))&1u;
            state.count += 1u;
          }
          if (state.count > state.bit_length) {
            state.state = 7;
            state.bit_length = 0;
            ae = api_error::EndOfFile;
          }
          break;
        case 7: /* end of stream */
          x = 0;
          ae = api_error::EndOfFile;
          break;
        }
        if (ae > api_error::Success)
          /* halt the read position here: */break;
        else y |= (x<<i);
      }
      state.bit_index = i&7u;
      return ae;
    }
    //END   brcvt / static

    //BEGIN brcvt_state / rule-of-zero
    brcvt_state::brcvt_state(uint32 block_size, uint32 n, size_t chain_length)
      : buffer(block_size, n, chain_length, false),
        literals(288u), distances(32u), sequence(19u),
        wbits(15u), values(286u),
        ring(false,4,0), try_ring(false,4,0),
        lit_histogram(288u), dist_histogram(32u), seq_histogram(19u),
        bits(0u), bit_length(0u), state(0u), bit_index(0u),
        backward(0u), count(0u), checksum(0u),
        bit_cap(0u)
    {
      if (n > 16777200u)
        n = 16777200u;
      wbits_select = 24;
      for (unsigned char i = 10; i < 24; ++i) {
        if (n <= (1ul<<i)-16) {
          wbits_select = i;
          break;
        }
      }
      fixlist_preset(wbits, prefix_preset::BrotliWBits);
      inscopy_preset(values, insert_copy_preset::Deflate);
      inscopy_codesort(values);
      sequence_list.reserve(286u+30u);
      return;
    }
    //END   brcvt_state / rule-of-zero

    //BEGIN brcvt_state / allocation
    void* brcvt_state::operator new(std::size_t sz) {
      return ::operator new(sz);
    }

    void* brcvt_state::operator new[](std::size_t sz) {
      return ::operator new(sz);
    }

    void brcvt_state::operator delete(void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    void brcvt_state::operator delete[](void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    brcvt_state* brcvt_new
        (uint32 block_size, uint32 n, size_t chain_length) noexcept
    {
      try {
        return new brcvt_state(block_size,n,chain_length);
      } catch (api_exception const& ) {
        return nullptr;
      } catch (std::bad_alloc const& ) {
        return nullptr;
      }
    }

    util_unique_ptr<brcvt_state> brcvt_unique
        (uint32 block_size, uint32 n, size_t chain_length) noexcept
    {
      return util_unique_ptr<brcvt_state>(brcvt_new(block_size,n,chain_length));
    }

    void brcvt_destroy(brcvt_state* x) noexcept {
      if (x) {
        delete x;
      }
    }
    //END   brcvt_state / allocation

    //BEGIN brcvt_state / public
    //END   brcvt_state / public

    //BEGIN brcvt / namespace local
    api_error brcvt_in(brcvt_state& state,
        unsigned char const* from, unsigned char const* from_end,
        unsigned char const*& from_next,
        unsigned char* to, unsigned char* to_end,
        unsigned char*& to_next)
    {
      api_error ae = api_error::Success;
      unsigned char const* p;
      unsigned char* to_out = to;
      for (p = from; p < from_end && ae == api_error::Success; ++p) {
        switch (state.state) {
        case 0: /* WBITS */
          ae = brcvt_in_bits(state, (*p), to, to_end, to_out);
          break;
        case 1: /* dictionary checksum */
          if (state.count < 4u) {
            state.checksum = (state.checksum<<8) | (*p);
            state.count += 1u;
          }
          if (state.count >= 4u) {
            state.state = 2;
            ae = api_error::ZDictionary;
          } break;
        case 2: /* check dictionary checksum */
        case 3: /* block start */
          if (state.state == 2) {
            if (state.backward != state.checksum) {
              ae = api_error::Sanitize;
              break;
            } else {
              state.state = 3;
              state.count = 0u;
              state.bits = 0u;
              state.checksum = 1u;
            }
          }
          ae = brcvt_in_bits(state, (*p), to, to_end, to_out);
          break;
        case 4: /* no compression: LEN and NLEN */
          if (state.count < 4u) {
            state.backward = (state.backward <<8) | (*p);
            state.count += 1u;
          }
          if (state.count >= 4u) {
            unsigned int const len = (state.backward>>16)&65535u;
            unsigned int const nlen = (~state.backward)&65535u;
            if (len != nlen) {
              ae = api_error::Sanitize;
            } else {
              state.backward = len;
              state.state = 5;
              state.count = 0u;
            }
          } break;
        case 5: /* no compression: copy bytes */
          if (state.count < state.backward) {
            if (to_out < to_end) {
              (*to_out) = (*p);
              state.checksum = zutil_adler32(1u, p, state.checksum);
              to_out += 1u;
              state.buffer.bypass(p, 1u, ae);
              if (ae != api_error::Success)
                break;
            } else {
              ae = api_error::Partial;
              break;
            }
            state.count += 1u;
          }
          if (state.count >= state.backward) {
            if (state.h_end)
              state.state = 6;
            else state.state = 3;
            state.count = 0u;
            state.backward = 0u;
          } break;
        case 6: /* end-of-stream checksum */
          if (state.count < 4u) {
            state.backward = (state.backward<<8) | (*p);
            state.count += 1u;
          }
          if (state.count >= 4u) {
            if (state.checksum != state.backward) {
              ae = api_error::Sanitize;
            } else state.state = 7;
          } break;
        case 7:
          ae = api_error::EndOfFile;
          break;
        case 8: /* decode */
        case 9: /* copy bits */
        case 10: /* backward */
        case 11: /* distance bits */
        case 12: /* output from backward */
        case 13: /* hcounts */
        case 14: /* code lengths code lengths */
        case 15: /* literals and distances */
        case 16: /* copy previous code length */
        case 17: /* copy zero length */
        case 18: /* copy zero length + 11 */
        case 19: /* generate code trees */
        case 20: /* alpha bringback */
          ae = brcvt_in_bits(state, (*p), to, to_end, to_out);
          break;
        }
        if (ae > api_error::Success)
          /* */break;
      }
      from_next = p;
      to_next = to_out;
      return ae;
    }

    size_t brcvt_bypass
      (brcvt_state& state, unsigned char const* buf, size_t sz, api_error& ae)
      noexcept
    {
      if (state.state >= 2u) {
        ae = api_error::Sanitize;
        return 0u;
      } else {
        size_t const n = state.buffer.bypass(buf, sz, ae);
        if (state.state == 0u) {
          /* output direction */
          state.checksum = zutil_adler32(sz, buf, state.checksum);
        } else {
          /* input direction */
          state.backward = zutil_adler32(sz, buf, state.backward);
        }
        return n;
      }
    }

    api_error brcvt_out(brcvt_state& state,
        unsigned char const* from, unsigned char const* from_end,
        unsigned char const*& from_next,
        unsigned char* to, unsigned char* to_end,
        unsigned char*& to_next)
    {
      api_error ae = api_error::Success;
      unsigned char* to_out;
      unsigned char const* p = from;
      for (to_out = to; to_out < to_end && ae == api_error::Success;
          ++to_out)
      {
        switch (state.state) {
        case 0: /* WBITS */
          ae = brcvt_out_bits(state, from, from_end, from_next, *to_out);
          break;
        case 1: /* dictionary checksum */
          if (state.count < 4u) {
            *to_out = static_cast<unsigned char>(
                  (state.checksum>>(32u-state.count*8u))&255u
                );
            state.count += 1u;
          }
          if (state.count >= 4u) {
            state.state = 3u;
            state.checksum = 1u;
            state.count = 0u;
          } break;
        case 2: /* check dictionary checksum */
        case 3: /* block start */
          state.state = 3u;
          ae = brcvt_out_bits(state, from, from_end, from_next, *to_out);
          break;
        case 4: /* no compression: LEN and NLEN */
          if (state.count == 0u) {
            uint32 const diff = state.backward - state.index;
            state.extra_length =
              static_cast<unsigned short>(diff<65535u ? diff : 65535u);
          }
          if (state.count < 4u) {
            *to_out = static_cast<unsigned char>(
                  state.extra_length>>(8u*state.count)
                );
            state.count += 1u;
          } else if (state.count < 4u) {
            *to_out = static_cast<unsigned char>(
                  ~(state.extra_length>>(8u*(2u-state.count)))
                );
            state.count += 1u;
          }
          if (state.count == 4u) {
            state.state = 5u;
          } break;
        case 5: /* no compression: copy bytes */
          if (state.extra_length > 0u) {
            state.extra_length -= 1u;
            *to_out = state.buffer.str()[state.index];
            state.index += 1u;
          }
          if (state.extra_length == 0u) {
            if (state.index < state.backward) {
              state.state = 4u;
            } else if (state.h_end & 1u) {
              state.state = 6u;
            } else {
              state.state = 3u;
              state.bits = 0u;
            }
            state.count = 0u;
          }
        case 6: /* end-of-stream checksum */
          if (state.count < 4u) {
            *to_out = (state.checksum>>(8u*state.count));
            state.count += 1u;
          }
          if (state.count >= 4u) {
            state.state = 7u;
            state.count = 0u;
          } break;
        case 7:
          ae = api_error::EndOfFile;
          break;
        case 8: /* encode */
        case 13: /* hcounts */
        case 14: /* code lengths code lengths */
        case 15: /* literals and distances */
        case 16: /* copy previous code length */
        case 17: /* copy zero length */
        case 18: /* copy zero length + 11 */
        case 19: /* generate code trees */
          ae = brcvt_out_bits(state, from, from_end, from_next, *to_out);
          break;
        }
        if (ae > api_error::Success)
          break;
      }
      from_next = p;
      to_next = to_out;
      return ae;
    }

    api_error brcvt_unshift(brcvt_state& state,
        unsigned char* to, unsigned char* to_end,
        unsigned char*& to_next)
    {
      unsigned char const tmp[1] = {0u};
      unsigned char const* tmp_next = &tmp[0];
      /* set the end flag: */state.h_end |= 2u;
      return brcvt_out(state, &tmp[0], &tmp[0], tmp_next, to, to_end, to_next);
    }
    //END   brcvt / namespace local
  };
};
