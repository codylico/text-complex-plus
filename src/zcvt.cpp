/**
 * @file src/zcvt.cpp
 * @brief zlib conversion state
 */
#define TCMPLX_AP_WIN32_DLL_INTERNAL
#include "text-complex-plus/access/zcvt.hpp"
#include "text-complex-plus/access/zutil.hpp"
#include <limits>
#include <new>
#include <algorithm>

namespace text_complex {
  namespace access {
    static
    api_error zcvt_in_bits
      ( zcvt_state& state, unsigned char y,
        unsigned char* to, unsigned char* to_end,
        unsigned char*& to_next);
    /**
     * @internal
     * @brief Bit iteration.
     * @param state the zlib conversion state to use
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
    api_error zcvt_out_bits
      ( zcvt_state& state,
        unsigned char const* from, unsigned char const* from_end,
        unsigned char const*& from_next,
        unsigned char& y);
    static
    unsigned char zcvt_clen[19] =
      {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
    static
    uint32 zcvt_cinfo(uint32 window_size);
    static
    bool zcvt_out_has_dictionary(zcvt_state const& state);
    static
    api_error zcvt_post_two
      (block_string& s, unsigned int x1, unsigned int x2) noexcept;
    /**
     * @brief Choose a failure code.
     * @param a one code
     * @param b another code
     * @return either `a` or `b`, whichever is nonzero
     */
    static api_error zcvt_nonzero(api_error a, api_error b) noexcept;
    /**
     * @brief Choose a failure code.
     * @param a one code
     * @param b another code
     * @param c another code
     * @return either `a` or `b` or `c`, whichever is nonzero
     */
    static api_error zcvt_nonzero
      (api_error a, api_error b, api_error c) noexcept;
    /**
     * @brief Make a code length sequence.
     * @param x the zcvt state
     * @return tcmplxA_Success on success, nonzero otherwise
     */
    static api_error zcvt_make_sequence(zcvt_state& state) noexcept;
    /**
     * @brief Add a code length sequence.
     * @param s sequence list
     * @param len code length
     * @param count repeat count
     * @return tcmplxA_Success on success, nonzero otherwise
     */
    static api_error zcvt_post_sequence
      (block_string& s, unsigned int len, unsigned int count) noexcept;
    /**
     * @brief Move from an output noconvert block to next state.
     * @param state the zcvt state to update
     */
    static void zcvt_noconv_next(zcvt_state& state) noexcept;

    static constexpr unsigned ZCvt_LitDynamicConst = 286u;
    static constexpr unsigned ZCvt_DistDynamicConst = 30u;


    //BEGIN zcvt / static
    api_error zcvt_in_bits
      ( zcvt_state& state, unsigned char y,
        unsigned char* to, unsigned char* to_end,
        unsigned char*& to_next)
    {
      unsigned int i;
      api_error ae = api_error::Success;
      for (i = state.bit_index; i < 8u && ae == api_error::Success; ++i) {
        unsigned int x = (y>>i)&1u;
        switch (state.state) {
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
                state.backward = state.ring.decode(alpha, 0u, 0, ae);
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
            state.backward = state.ring.decode(state.backward, state.bits, 0, ae);
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
            state.sequence[zcvt_clen[state.index]].len = (state.bits&7u);
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

    unsigned int zcvt_cinfo(uint32 window_size) {
      unsigned int out = 0u;
      uint32 v;
      if (window_size > 32768u)
        return 8u;
      else for (v = 1u; v < window_size; v<<=1, ++out) {
        continue;
      }
      return out > 8u ? out-8u : 0u;
    }

    bool zcvt_out_has_dictionary(zcvt_state const& state) {
      return state.buffer.ring_size() > 0u;
    }

    api_error zcvt_nonzero(api_error a, api_error b) noexcept {
      return a!=api_error::Success ? a : b;
    }
    api_error zcvt_nonzero(api_error a, api_error b, api_error c) noexcept {
      return zcvt_nonzero(a,zcvt_nonzero(b,c));
    }

    api_error zcvt_make_sequence(zcvt_state& state) noexcept {
      unsigned int len = ~0u, len_count = 0u;
      state.sequence_list.clear();
      for (unsigned i = 0; i < ZCvt_LitDynamicConst; ++i) {
        unsigned int const n = (i >= state.literals.size()) ? 0
          : state.literals[i].len;
        if (len != n) {
          api_error const ae = zcvt_post_sequence
            (state.sequence_list, len, len_count);
          if (ae != api_error::Success)
            return ae;
          len = n;
          len_count = 1u;
        } else len_count += 1u;
      }
      for (unsigned i = 0; i < ZCvt_DistDynamicConst; ++i) {
        unsigned int const n = (i >= state.distances.size()) ? 0
          : state.distances[i].len;
        if (len != n) {
          api_error const ae = zcvt_post_sequence
            (state.sequence_list, len, len_count);
          if (ae != api_error::Success)
            return ae;
          len = n;
          len_count = 1u;
        } else len_count += 1u;
      }
      return zcvt_post_sequence(state.sequence_list, len, len_count);
    }

    api_error zcvt_post_two
      (block_string& s, unsigned int x1, unsigned int x2) noexcept
    {
      api_error append_ae[2];
      s.push_back(static_cast<unsigned char>(x1), append_ae[0]);
      s.push_back(static_cast<unsigned char>(x2), append_ae[1]);
      return zcvt_nonzero(append_ae[0], append_ae[1]);
    }

    api_error zcvt_post_sequence
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
            ae = zcvt_post_two(s, 18u, 127u);
            i += 138u;
          } else if (x >= 11u) {
            ae = zcvt_post_two(s, 18u, x-11u);
            i += x;
          } else if (x >= 3u) {
            ae = zcvt_post_two(s, 17u, x-3u);
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
        for (i = 1u; i < count && ae == api_error::Success; ) {
          unsigned int const x = count-i;
          if (x > 6u) {
            ae = zcvt_post_two(s, 16u, 3u);
            i += 6u;
          } else if (x >= 3u) {
            ae = zcvt_post_two(s, 17u, x-3u);
            i += x;
          } else {
            s.push_back(static_cast<unsigned char>(len), ae);
            i += 1u;
          }
        }
        return ae;
      }
    }

    api_error zcvt_out_bits
      ( zcvt_state& state,
        unsigned char const* from, unsigned char const* from_end,
        unsigned char const*& p, unsigned char& y)
    {
      unsigned int i;
      api_error ae = api_error::Success;
      // Restore in-progress byte.
      y = state.write_scratch;
      state.write_scratch = 0;
      for (i = state.bit_index; i < 8u && ae == api_error::Success; ++i) {
        unsigned int x = 0u;
        if ((!(state.h_end&1u))/* if end marker not activated yet */
        &&  (state.state == 3 && state.count == 0u)/* and not inside a block */)
        {
          uint32 const input_space =
              state.buffer.capacity() - state.buffer.input_size();
          std::size_t const from_count = from_end - p;
          std::size_t const min_count = (input_space < from_count)
            ? static_cast<std::size_t>(input_space) : from_count;
          state.buffer.write(p, min_count, ae);
          if (ae != api_error::Success)
            break;
          state.checksum = zutil_adler32(min_count, p, state.checksum);
          p += min_count;
        }
        switch (state.state) {
        case 3: /* block start */
          /* flip the switch */if (state.h_end & 2u) {
            state.h_end |= 1u;
          }
          /* try compress the data */if (state.count == 0u) {
            bool dynamic = false;
            if (state.buffer.input_size() == 0u && (!state.h_end)) {
              /* stash the current byte to the side */
              ae = api_error::Partial;
              break;
            } else {
              state.buffer.clear_output();
              state.buffer.try_block(ae);
            }
            if (ae == api_error::Success) {
              /* histogram */
              block_string const& buffer_str = state.buffer.str();
              uint32 const buffer_size = buffer_str.size();
              uint32 const buffer_m1 = buffer_size-1u;
              unsigned long int bit_count = 0u;
              /* prepare */{
                state.try_ring = state.ring;
                std::fill(state.lit_histogram.begin(),
                    state.lit_histogram.end(), 0u);
                std::fill(state.dist_histogram.begin(),
                    state.dist_histogram.end(), 0u);
                std::fill(state.seq_histogram.begin(),
                    state.seq_histogram.end(), 0u);
                inscopy_lengthsort(state.values, ae);
                if (ae != api_error::Success)
                  break;
              }
              /* calculate histogram */{
                for (uint32 buffer_pos = 0u; buffer_pos < buffer_size; ++buffer_pos) {
                  unsigned char const byt = buffer_str[buffer_pos];
                  bool const insert_flag = ((byt&128u)==0u);
                  unsigned short len;
                  unsigned short j;
                  if (byt&64u) {
                    if (buffer_pos == buffer_m1) {
                      ae = api_error::Sanitize;
                      break;
                    } else {
                      len = (((byt&63u)<<8)|(buffer_str[buffer_pos+1u]&255u))+64u;
                      buffer_pos += 1u;
                    }
                  } else len = byt&63u;
                  if (len == 0u)
                    continue;
                  else if (!insert_flag) {
                    size_t const lit_index =
                      inscopy_encode(state.values, 0u, len, false);
                    uint32 distance = 0u;
                    if (lit_index >= state.values.size()) {
                      ae = api_error::InsCopyMissing;
                      break;
                    } else {
                      insert_copy_row const& lit = state.values[lit_index];
                      state.lit_histogram[lit.code] += 1u;
                      bit_count += lit.copy_bits;
                    }
                    /* distance */switch (buffer_str[buffer_pos+1u] & 192u) {
                    case 128u:
                      if (buffer_pos+2u < buffer_size) {
                        distance = ((buffer_str[buffer_pos+1u]&63u)<<8) | buffer_str[buffer_pos+2u];
                        buffer_pos += 2u;
                      } else ae = api_error::BlockOverflow;
                      break;
                    case 192u:
                      if (buffer_pos+4u < buffer_size) {
                        distance = ((static_cast<uint32>(buffer_str[buffer_pos+1u]&63u)<<24)
                          | (static_cast<uint32>(buffer_str[buffer_pos+2u])<<16)
                          | (buffer_str[buffer_pos+3u]<<8) | buffer_str[buffer_pos+4u]) + 16384u;
                        buffer_pos += 2u;
                      } else ae = api_error::BlockOverflow;
                      break;
                    default:
                      /* Brotli dictionary not supported in ZLIB stream */
                      ae = api_error::Sanitize;
                    }
                    /* encode distance */if (ae == api_error::Success) {
                      uint32 extra;
                      unsigned int const dist_code =
                        state.try_ring.encode(distance+1, extra, 0, ae);
                      if (ae == api_error::Success) {
                        bit_count += extra;
                        state.dist_histogram[dist_code] += 1u;
                      } else break;
                    } else break;
                  } else for (j = 0u; j < len && buffer_pos<buffer_m1; ++j, ++buffer_pos) {
                    state.lit_histogram[buffer_str[buffer_pos+1u]] += 1u;
                  }
                }
                // Ensure the stop code.
                state.lit_histogram[256] = 1;
                if (ae != api_error::Success)
                  break;
                else/* plant two trees */{
                  /* dynamic Huffman codes */{
                    for (unsigned j = 0u; j < 288u; ++j) {
                      state.literals[j].value = j;
                    }
                    for (unsigned j = 0u; j < 32u; ++j) {
                      state.distances[j].value = j;
                    }
                    for (unsigned j = 0u; j < 19u; ++j) {
                      state.sequence[j].value = j;
                    }
                  }
                  /* lengths */{
                    api_error lit_ae;
                    api_error dist_ae;
                    fixlist_gen_lengths
                      (state.literals, state.lit_histogram, 15u, lit_ae);
                    fixlist_gen_lengths
                      (state.distances, state.dist_histogram, 15u, dist_ae);
                    ae = zcvt_nonzero(lit_ae, dist_ae);
                    if (ae != api_error::Success)
                      break;
                    fixlist_valuesort(state.literals, lit_ae);
                    fixlist_valuesort(state.distances, dist_ae);
                    ae = zcvt_nonzero(lit_ae, dist_ae);
                    if (ae != api_error::Success)
                      break;
                  }
                  /* guess a sequence */{
                    ae = zcvt_make_sequence(state);
                    if (ae != api_error::Success)
                      break;
                  }
                  /* the sequence histogram */{
                    uint32 j;
                    unsigned char const* const sequence_data =
                      state.sequence_list.data();
                    for (j = 0u; j < state.sequence_list.size(); ++j) {
                      unsigned int const v = sequence_data[j];
                      if (v >= 16u) {
                        unsigned char const v2[3] = {2u,3u,7u};
                        j += 1u;
                        bit_count += v2[v-16u];
                      }
                      state.seq_histogram[v] += 1u;
                    }
                  }
                  /* length lengths */{
                    fixlist_gen_lengths
                      (state.sequence, state.seq_histogram, 7u, ae);
                    if (ae != api_error::Success)
                      break;
                  }
                }
                /* estimate the block length */{
                  unsigned int j;
                  for (j = 0u; j < 288u; ++j) {
                    bit_count += state.lit_histogram[j]
                      * state.literals[j].len;
                  }
                  for (j = 0u; j < 32u; ++j) {
                    bit_count += state.dist_histogram[j]
                      * state.distances[j].len;
                  }
                  for (j = 0u; j < 19u; ++j) {
                    bit_count += state.seq_histogram[j]
                      * state.sequence[j].len;
                  }
                }
              }
              /* compare to text length */{
                unsigned long int const byte_count = ((bit_count+7u)>>3);
                if (byte_count < state.buffer.input_size()) {
                  dynamic = true;
                  fixlist_gen_codes(state.sequence, ae);
                } else {
                  dynamic = false;
                  state.buffer.clear_output();
                  state.buffer.noconv_block(ae);
                }
              }
              if (ae == api_error::Success) {
                state.bits = (state.h_end&1u) | (dynamic?4u:0u);
                state.buffer.clear_input();
              }
            } else break;
          }
          if (state.count < 3u) {
            x = (state.bits>>state.count)&1u;
            state.count += 1u;
          }
          if (state.count == 3u) {
            if (state.bits&6u) {
              state.state = 13u;
              state.count = 0u;
              state.bit_length = 0u;
              state.bits = 0u;
            } else {
              state.state = 4u;
              state.backward = state.buffer.str().size();
              state.index = 0u;
              state.count = 0u;
            }
          } break;
        case 4: /* no compression: LEN and NLEN */
        case 6: /* end-of-stream checksum */
          x = 0u;
          break;
        case 19: /* generate code trees */
          /* */{
            api_error lit_ae;
            api_error dist_ae;
            fixlist_gen_codes(state.literals, lit_ae);
            fixlist_gen_codes(state.distances, dist_ae);
            ae = zcvt_nonzero(lit_ae, dist_ae);
            if (ae != api_error::Success)
              break;
          }
          /* */{
            api_error lit_ae;
            api_error dist_ae;
            fixlist_valuesort(state.literals, lit_ae);
            fixlist_valuesort(state.distances, dist_ae);
            ae = zcvt_nonzero(lit_ae, dist_ae);
            if (ae != api_error::Success)
              break;
            state.state = 8u;
            state.backward = state.buffer.str().size();
            state.bit_length = 0u;
            state.index = 0u;
          } [[fallthrough]];
        case 8: /* encode */
          if (state.bit_length == 0u) {
            if (state.index >= state.backward) {
              prefix_line const& line = state.literals[256u];
              state.bit_cap = line.len;
              state.bits = line.code;
            } else {
              block_string const& data = state.buffer.str();
              unsigned char buf[2u] = {0u};
              unsigned int len;
              buf[0u] = data[state.index];
              if ((buf[0u]&64u) && (state.index+1u < state.backward)) {
                state.index += 1u;
                buf[1u] = data[state.index];
                len = buf[1u] + ((buf[0u]&63u)<<8) + 64u;
              } else len = buf[0u]&63u;
              if ((buf[0u]&128u) == 0u) {
                /* insert */
                if (state.index+1u >= state.backward || len == 0u) {
                  ae = api_error::Sanitize;
                  break;
                } else {
                  unsigned char const alpha = data[state.index+1u];
                  prefix_line const& line = state.literals[alpha];
                  state.state = 20u;
                  state.index += 1u;
                  state.count = len;
                  state.bit_cap = line.len;
                  state.bits = line.code;
                }
              } else {
                /* copy */
                size_t const copy_index = inscopy_encode
                  (state.values, 0u, len, false);
                if (copy_index == std::numeric_limits<size_t>::max()) {
                  ae = api_error::Sanitize;
                  break;
                } else {
                  insert_copy_row const& irow = state.values[copy_index];
                  unsigned short const alpha = irow.code;
                  prefix_line const& line = state.literals[alpha];
                  state.bit_cap = line.len;
                  state.bits = line.code;
                  state.count = len - irow.copy_first;
                  state.extra_length = irow.copy_bits;
                }
              }
            }
          }
          if (state.bit_length < state.bit_cap) {
            x = (state.bits>>(state.bit_cap-1u-state.bit_length))&1u;
            state.bit_length += 1u;
          }
          if (state.bit_length >= state.bit_cap) {
            if (state.index >= state.backward) {
              if (state.h_end & 1u) {
                state.state = 6u;
              } else {
                state.state = 3u;
                state.bits = 0u;
              }
              state.count = 0u;
            } else if (state.state == 20u) {
              state.count -= 1u;
              state.index += 1u;
              if (state.count == 0u) {
                state.state = 8u;
              }
              state.bit_length = 0u;
            } else {
              state.bit_length = 0u;
              state.state = (state.extra_length>0u ? 9u : 10u);
              state.index += 1u;
            }
          } break;
        case 20: /* alpha bringback */
          if (state.bit_length == 0u) {
            if (state.index < state.backward) {
              block_string const& data = state.buffer.str();
              unsigned char const alpha = data[state.index];
              prefix_line const& line = state.literals[alpha];
              state.bit_cap = line.len;
              state.bits = line.code;
            } else {
              ae = api_error::Sanitize;
              break;
            }
          }
          if (state.bit_length < state.bit_cap) {
            x = (state.bits>>(state.bit_cap-1u-state.bit_length))&1u;
            state.bit_length += 1u;
          }
          if (state.bit_length >= state.bit_cap) {
            state.index += 1u;
            state.count -= 1u;
            if (state.count == 0u)
              state.state = 8u;
            state.bit_length = 0u;
          } break;
        case 9: /* copy bits */
          if (state.bit_length < state.bit_cap) {
            x = (state.count>>state.bit_length)&1u;
            state.bit_length += 1;
          }
          if (state.bit_length >= state.bit_cap) {
            state.state = 10u;
            state.bit_length = 0u;
          }
          break;
        case 10: /* distance Huffman code */
          if (state.bit_length == 0u) {
            unsigned char buf[4] = {0};
            unsigned long distance = 0;
            unsigned char const* const data = state.buffer.str().data();
            assert(state.index < state.backward);
            buf[0u] = data[state.index];
            state.index += 1;
            if ((buf[0]&128u) == 0u) {
              /* zlib stream does not support Brotli references */
              ae = api_error::Sanitize;
              break;
            } else if (state.index >= state.backward) {
              /* also, index needs to be in range */
              ae = api_error::OutOfRange;
              break;
            }
            if (buf[0u] & 64u) {
              if (state.backward - state.index < 3u) {
                ae = api_error::OutOfRange;
                break;
              }
              std::memcpy(buf+1, data+state.index, 3);
              state.index += 2;
              distance = ((buf[0]&63ul)<<24)
                + ((buf[1]&255ul)<<16) + ((buf[2]&255u)<<8)
                + (buf[3]&255u) + 16384ul;
            } else {
              buf[1] = data[state.index];
              distance = ((buf[0]&63)<<8) + (buf[1]&255);
            }
            if (distance > 32768) {
              /* zlib lacks support for large distances */
              ae = api_error::Sanitize;
              break;
            } else {
              uint32 dist_extra = 0;
              unsigned const dist_index = state.ring.encode(
                static_cast<unsigned>(distance)+1u, dist_extra, 0, ae);
              if (ae != api_error::Success)
                break;
              prefix_line const& line = state.distances[dist_index];
              state.bit_cap = line.len;
              state.bits = line.code;
              state.count = dist_extra;
              state.extra_length = state.ring.bit_count(dist_index);
            }
          }
          if (state.bit_length < state.bit_cap) {
            x = (state.bits>>(state.bit_cap-1u-state.bit_length))&1u;
            state.bit_length += 1u;
          }
          if (state.bit_length >= state.bit_cap) {
            state.bit_length = 0u;
            state.state = (state.extra_length>0u ? 11u : 8u);
            state.index += 1u;
          } break;
        case 11: /* distance extra bits */
          if (state.bit_length < state.extra_length) {
            x = (state.count>>state.bit_length)&1u;
            state.bit_length += 1;
          }
          if (state.bit_length >= state.extra_length) {
            state.state = 8u;
            state.bit_length = 0u;
          }
          break;
        case 13: /* hcounts */
          if (state.bit_length == 0u) {
            state.count = 19u;
            state.bits = (((state.count-4u) << 10)
              | ((ZCvt_DistDynamicConst-1)<<5) | (ZCvt_LitDynamicConst-257));
          }
          if (state.bit_length < 14u) {
            x = (state.bits>>state.bit_length)&1u;
            state.bit_length += 1u;
          }
          if (state.bit_length >= 14u) {
            state.backward = (state.bits&1023u);
            state.state = 14u;
            state.index = 0u;
            state.bit_length = 0u;
            state.bits = 0u;
            fixlist_valuesort(state.sequence, ae);
          } break;
        case 14: /* code lengths code lengths */
          if (state.bit_length == 0u) {
            state.bits = state.sequence[zcvt_clen[state.index]].len;
          }
          if (state.bit_length < 3u) {
            x = (state.bits>>state.bit_length)&1u;
            state.bit_length += 1u;
          }
          if (state.bit_length == 3u) {
            state.index += 1u;
            state.bit_length = 0u;
          }
          if (state.index >= state.count) {
            state.state = 15u;
            state.count = (state.backward&31u)+257u;
            state.backward = ((state.backward>>5)&31u) + 1u + state.count;
            state.extra_length = 0u;
            state.index = 0u;
            state.bits = 0u;
            state.bit_length = 0u;
          } break;
        case 15: /* literals and distances */
          if (state.bit_length == 0u) {
            unsigned int const alpha = state.sequence_list[state.index];
            prefix_line const& line = state.sequence[alpha];
            state.bits = line.code;
            state.extra_length = line.len;
          }
          if (state.bit_length < state.extra_length) {
            x = (state.bits>>(state.extra_length-1u-state.bit_length))&1u;
            state.bit_length += 1u;
          }
          if (state.bit_length >= state.extra_length) {
            unsigned char const alpha = state.sequence_list[state.index];
            state.index += 1u;
            state.bit_length = 0u;
            if (alpha >= 16u && alpha <= 18u) {
              state.state = alpha;
            } else if (state.index >= state.sequence_list.size()) {
              state.state = 19u;
            } else state.state = 15u;
          } break;
        case 16: /* copy previous code length */
        case 17: /* copy zero length */
        case 18: /* copy zero length + 11 */
          if (state.bit_length == 0u) {
            constexpr unsigned char lens[3] = {2u,3u,7u};
            state.extra_length = lens[state.state-16u];
            state.bits = state.sequence_list[state.index];
          }
          if (state.bit_length < state.extra_length) {
            x = (state.bits>>state.bit_length)&1u;
            state.bit_length += 1u;
          }
          if (state.bit_length >= state.extra_length) {
            state.index += 1u;
            state.bit_length = 0u;
            if (state.index >= state.sequence_list.size()) {
              state.state = 19u;
            } else state.state = 15u;
          } break;
        default:
          ae = api_error::Unknown;
          break;
        }
        if (ae > api_error::Success) {
          if (ae == api_error::Partial)
            state.write_scratch = y;
          /* halt the read position here: */break;
        }
        else y |= (x<<i);
      }
      state.bit_index = i&7u;
      return ae;
    }


    void zcvt_noconv_next(zcvt_state& state) noexcept {
      if (state.index < state.backward) {
        state.state = 4u;
      } else if (state.h_end & 1u) {
        state.state = 6u;
      } else {
        state.state = 3u;
        state.bits = 0u;
      }
      state.count = 0u;
      return;
    }
    //END   zcvt / static

    //BEGIN zcvt_state / rule-of-zero
    zcvt_state::zcvt_state(uint32 block_size, uint32 n, size_t chain_length)
      : buffer(block_size, n, chain_length, false),
        literals(288u), distances(32u), sequence(19u), values(286u),
        ring(false,4,0), try_ring(false,4,0),
        lit_histogram(288u), dist_histogram(32u), seq_histogram(19u),
        bits(0u), bit_length(0u), state(0u), bit_index(0u),
        backward(0u), count(0u), checksum(0u),
        bit_cap(0u), write_scratch(0)
    {
      inscopy_preset(values, insert_copy_preset::Deflate);
      inscopy_codesort(values);
      sequence_list.reserve(286u+30u);
      return;
    }
    //END   zcvt_state / rule-of-zero

    //BEGIN zcvt_state / allocation
    void* zcvt_state::operator new(std::size_t sz) {
      return ::operator new(sz);
    }

    void* zcvt_state::operator new[](std::size_t sz) {
      return ::operator new(sz);
    }

    void zcvt_state::operator delete(void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    void zcvt_state::operator delete[](void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    zcvt_state* zcvt_new
        (uint32 block_size, uint32 n, size_t chain_length) noexcept
    {
      try {
        return new zcvt_state(block_size,n,chain_length);
      } catch (api_exception const& ) {
        return nullptr;
      } catch (std::bad_alloc const& ) {
        return nullptr;
      }
    }

    util_unique_ptr<zcvt_state> zcvt_unique
        (uint32 block_size, uint32 n, size_t chain_length) noexcept
    {
      return util_unique_ptr<zcvt_state>(zcvt_new(block_size,n,chain_length));
    }

    void zcvt_destroy(zcvt_state* x) noexcept {
      if (x) {
        delete x;
      }
    }
    //END   zcvt_state / allocation

    //BEGIN zcvt_state / public
    //END   zcvt_state / public

    //BEGIN zcvt / namespace local
    api_error zcvt_in(zcvt_state& state,
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
        case 0: /* initial state */
          if (state.count < 2u) {
            state.backward = (state.backward<<8) | (*p);
            state.count += 1u;
          }
          if (state.count >= 2u) {
            unsigned int const cm = (state.backward>>8)&15u;
            unsigned int const cinfo = (state.backward>>12)&15u;
            if (state.backward%31u != 0u) {
              ae = api_error::Sanitize;
            } else if (cm != 8u || cinfo > 7u) {
              ae = api_error::Sanitize;
            } else if (state.backward&32u) {
              ae = api_error::Success;
              state.state = 1;
              state.count = 0;
              state.backward = state.checksum;
              state.checksum = 1u;
            } else {
              state.count = 0u;
              state.state = 3;
              state.bits = 0u;
              state.checksum = 1u;
            }
          } break;
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
          ae = zcvt_in_bits(state, (*p), to, to_end, to_out);
          break;
        case 4: /* no compression: LEN and NLEN */
          if (state.count < 4u) {
            state.backward |= (*p << (state.count*8));
            state.count += 1u;
          }
          if (state.count >= 4u) {
            unsigned int const len = (state.backward)&65535u;
            unsigned int const nlen = (~state.backward>>16)&65535u;
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
          ae = zcvt_in_bits(state, (*p), to, to_end, to_out);
          break;
        }
        if (ae > api_error::Success)
          /* */break;
      }
      from_next = p;
      to_next = to_out;
      return ae;
    }

    size_t zcvt_bypass
      (zcvt_state& state, unsigned char const* buf, size_t sz, api_error& ae)
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

    api_error zcvt_out(zcvt_state& state,
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
        if (state.state == 7) {
          ae = api_error::EndOfFile;
          break;
        }
        switch (state.state) {
        case 0: /* initial state */
          if (state.count == 0u) {
            /* make a header */
            unsigned int header = 2048u;
            uint32 const extent = state.buffer.extent();
            unsigned int const cinfo = zcvt_cinfo(extent);
            inscopy_lengthsort(state.values, ae);
            if (ae != api_error::Success)
              break;
            if (cinfo > 7u)
              ae = api_error::Sanitize;
            header |= ((cinfo<<12)
                | (zcvt_out_has_dictionary(state) ? 32u : 0u));
            header += (31u-(header%31u));
            state.backward = header;
          }
          if (state.count < 2u) {
            *to_out = static_cast<unsigned char>(
                  (state.backward>>(8u-state.count*8u))&255u
                );
            state.count += 1u;
          }
          if (state.count >= 2u) {
            unsigned int const fdict = state.backward&32u;
            if (fdict != 0u) {
              state.state = 1u;
            } else {
              state.state = 3u;
              state.checksum = 1u;
            }
            state.backward = 0u;
            state.count = 0u;
          } break;
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
          ae = zcvt_out_bits(state, from, from_end, p, *to_out);
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
                  ~(state.extra_length>>(8u*(state.count-2u)))
                );
            state.count += 1u;
          }
          if (state.count == 4u) {
            state.state = 5u;
            if (state.extra_length == 0u)
              zcvt_noconv_next(state);
          } break;
        case 5: /* no compression: copy bytes */
          if (state.extra_length > 0u) {
            state.extra_length -= 1u;
            *to_out = state.buffer.str()[state.index];
            state.index += 1u;
          }
          if (state.extra_length == 0u) {
            zcvt_noconv_next(state);
          }
        case 6: /* end-of-stream checksum */
          if (state.count < 4u) {
            *to_out = (state.checksum>>(24u-8u*state.count));
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
        case 9:
        case 10:
        case 11:
        case 13: /* hcounts */
        case 14: /* code lengths code lengths */
        case 15: /* literals and distances */
        case 16: /* copy previous code length */
        case 17: /* copy zero length */
        case 18: /* copy zero length + 11 */
        case 19: /* generate code trees */
        case 20:
          ae = zcvt_out_bits(state, from, from_end, p, *to_out);
          break;
        }
        if (ae >= api_error::Partial)
          break;
      }
      from_next = p;
      to_next = to_out;
      return ae;
    }

    api_error zcvt_unshift(zcvt_state& state,
        unsigned char* to, unsigned char* to_end,
        unsigned char*& to_next)
    {
      unsigned char const tmp[1] = {0u};
      unsigned char const* tmp_next = &tmp[0];
      /* set the end flag: */state.h_end |= 2u;
      return zcvt_out(state, &tmp[0], &tmp[0], tmp_next, to, to_end, to_next);
    }
    //END   zcvt / namespace local
  };
};
