/**
 * @file src/zcvt.cpp
 * @brief zlib conversion state
 */
#define TCMPLX_AP_WIN32_DLL_INTERNAL
#include "text-complex-plus/access/zcvt.hpp"
#include "text-complex-plus/access/zutil.hpp"
#include <limits>
#include <new>

namespace text_complex {
  namespace access {
    static
    api_error zcvt_in_bits
      ( zcvt_state& state, unsigned char y,
        unsigned char* to, unsigned char* to_end,
        unsigned char*& to_next);
    static
    unsigned char zcvt_clen[19] =
      {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

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
    //END   zcvt / static

    //BEGIN zcvt_state / rule-of-zero
    zcvt_state::zcvt_state(uint32 block_size, uint32 n, size_t chain_length)
      : buffer(block_size, n, chain_length, false),
        literals(288u), distances(32u), sequence(19u), values(286u),
        ring(false,4,0),
        bits(0u), bit_length(0u), state(0u), bit_index(0u),
        backward(0u), count(0u), checksum(0u)
    {
      inscopy_preset(values, insert_copy_preset::Deflate);
      inscopy_codesort(values);
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
    //END   zcvt / namespace local
  };
};
