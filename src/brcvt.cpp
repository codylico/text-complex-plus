/**
 * @file src/brcvt.cpp
 * @brief Brotli conversion state
 */
#define TCMPLX_AP_WIN32_DLL_INTERNAL
#include "text-complex-plus/access/brcvt.hpp"
#include "text-complex-plus/access/zutil.hpp"
#include "text-complex-plus/access/util.hpp"
#include <array>
#include <limits>
#include <new>
#include <algorithm>
#include <cstring>
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
    unsigned char brcvt_clen[] =
      {1, 2, 3, 4, 0, 5, 17, 6, 16, 7, 8, 9, 10, 11, 12, 13, 14, 15};
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
     * @brief Configure a block count acquisition.
     * @param ps Brotli conversion state to update
     * @param value block code [0, 26)
     * @param next_state follow-up state in case of zero-length extra bits
     * @return base block count
     */
    static uint32 brcvt_config_count
      (brcvt_state& state, unsigned long value, unsigned char next_state);
    /**
     * @brief Make a code length sequence.
     * @param x the mini-state for tree transmission
     * @param literals primary tree to encode
     * @return tcmplxA_Success on success, nonzero otherwise
     */
    static api_error brcvt_make_sequence
      (brcvt_state::treety_box& state, prefix_list const& literals) noexcept;
    /**
     * @brief Add a code length sequence.
     * @param s sequence list
     * @param len code length
     * @param count repeat count
     * @return tcmplxA_Success on success, nonzero otherwise
     */
    static api_error brcvt_post_sequence
      (block_string& s, unsigned int len, unsigned int count) noexcept;
    /**
     * @brief Start the next block of output.
     * @param ps Brotli state
     */
    static void brcvt_next_block(brcvt_state& ps) noexcept;
    /**
     * @brief Determine if it's safe to add input.
     * @param ps compressor state to check
     * @return nonzero if safe, zero if unsafe
     */
    static bool brcvt_can_add_input(brcvt_state const& ps) noexcept;
    /**
     * @brief Resolve skip values.
     * @param prefixes prefix tree that may participate in code skipping
     * @return the value with the single nonzero code length, or NoSkip if unavailable
     */
    static unsigned short brcvt_resolve_skip(prefix_list const& prefixes) noexcept;
    /**
     * @brief Process a single bit of a prefix list description.
     * @param[in,out] treety mini-state for description parsing
     * @param[out] prefixes prefix list to update
     * @param x bit to process
     * @param alphabits minimum number of bits needed to represent all
     *   applicable symbols
     * @return EndOfFile at the end of a prefix list, Sanitize on
     *   parse failure, otherwise Success
     * @note At EOF, the extracted prefix list `prefixes` is sorted
     *   and ready to use.
     */
    static api_error brcvt_inflow19(brcvt_state::treety_box& treety,
      prefix_list& prefixes, unsigned x, unsigned alphabits);
    /**
     * @brief Transfer values for a simple prefix tree.
     * @param[out] prefixes tree to compose
     * @param nineteen value storage
     * @return EndOfFile on success, nonzero otherwise
     */
    static api_error brcvt_transfer19(prefix_list& prefixes,
      prefix_list const& nineteen);
    /**
     * @brief Post a code length to a prefix list.
     * @param[in,out] treety state machine
     * @param[out] prefixes prefix tree to update
     * @param value value to post
     * @return Success on success, Sanitize otherwise
     */
    static api_error brcvt_post19(brcvt_state::treety_box& treety,
      prefix_list& prefixes, unsigned short value);
    /**
     * @internal
     * @brief Emit a single bit of a prefix list description.
     * @param[in,out] treety mini-state for description parsing
     * @param[in,out] prefixes prefix list to update
     * @param[out] x bit to emit
     * @param alphabits minimum number of bits needed to represent all
     *   applicable symbols
     * @return EOF at the end of a prefix list, Sanitize or Memory on
     *   generator failure, otherwise Success
     */
    static api_error brcvt_outflow19(brcvt_state::treety_box& treety,
      prefix_list& prefixes, unsigned& x, unsigned alphabits);
    /**
     * @brief Reset a mini-state.
     * @param treety mini-state to reset
     */
    static void brcvt_reset19(brcvt_state::treety_box& treety);
    /**
     * @brief Check whether to emit compression.
     * @param ps Brotli conversion state
     * @return Success to proceed with compression, nonzero to emit uncompressed
     */
    static api_error brcvt_check_compress(brcvt_state& state);
    /**
     * @brief Encode a nonzero entry in a context map using run-length encoding.
     * @param[out] buffer storage of intermediate encoding
     * @param zeroes number of preceding zeroes
     * @param map_datum if nonzero, the entry to encode
     * @param[in,out] rlemax_ptr reference to integer holding maximum RLE keyword used
     * @return Success on success, nonzero on allocation failure
     */
    static api_error brcvt_encode_map(block_string& buffer, std::size_t zeroes,
      unsigned char map_datum, unsigned& rlemax_ptr) noexcept;
    /**
     * @brief Prepare for the inflow of a compressed stream.
     * @param state state to prepare
     * @param[out] ae error code to update on failure
     */
    static void brcvt_reset_compress(brcvt_state& state, api_error& ae) noexcept;
    /**
     * @brief Get the active forest.
     * @param ps state to inspect
     * @return a pointer to a forest if available
     */
    static gasp_vector& brcvt_active_forest(brcvt_state& ps) noexcept;
    /**
     * @brief Get the active skip code.
     * @param ps state to inspect
     * @return a pointer to a skip code if available
     */
    static unsigned short& brcvt_active_skip(brcvt_state& ps) noexcept;

    namespace {
      enum brcvt_istate {
        BrCvt_WBits = 0,
        BrCvt_MetaStart = 1,
        BrCvt_MetaLength = 2,
        BrCvt_MetaText = 3,
        BrCvt_LastCheck = 4,
        BrCvt_Nibbles = 5,
        BrCvt_InputLength = 6,
        BrCvt_Done = 7,
        BrCvt_CompressCheck = 8,
        BrCvt_Uncompress = 9,
        BrCvt_BadToken = 10,
        BrCvt_BlockTypesL = 16,
        BrCvt_BlockTypesLAlpha = 17,
        BrCvt_BlockCountLAlpha = 18,
        BrCvt_BlockStartL = 19,
        BrCvt_BlockTypesI = 20,
        BrCvt_BlockTypesIAlpha = 21,
        BrCvt_BlockCountIAlpha = 22,
        BrCvt_BlockStartI = 23,
        BrCvt_BlockTypesD = 24,
        BrCvt_BlockTypesDAlpha = 25,
        BrCvt_BlockCountDAlpha = 26,
        BrCvt_BlockStartD = 27,
        BrCvt_NPostfix = 28,
        BrCvt_ContextTypesL = 29,
        BrCvt_TreeCountL = 30,
        BrCvt_ContextRunMaxL = 31,
        BrCvt_ContextPrefixL = 32,
        BrCvt_ContextValuesL = 33,
        BrCvt_ContextRepeatL = 34,
        BrCvt_ContextInvertL = 35,
        BrCvt_TreeCountD = 38,
        BrCvt_ContextRunMaxD = 39,
        BrCvt_ContextPrefixD = 40,
        BrCvt_ContextValuesD = 41,
        BrCvt_ContextRepeatD = 42,
        BrCvt_ContextInvertD = 43,
        BrCvt_GaspVectorL = 44,
        BrCvt_GaspVectorI = 45,
        BrCvt_GaspVectorD = 46,
        BrCvt_DataInsertCopy = 47,
        BrCvt_Literal = 56,
      };
      /** @brief Treety machine states. */
      enum brcvt_tstate {
        BrCvt_TComplex = 0,
        BrCvt_TSimpleCount = 1,
        BrCvt_TSimpleAlpha = 2,
        BrCvt_TDone = 3,
        BrCvt_TSimpleFour = 4,
        BrCvt_TSymbols = 5,
        BrCvt_TRepeatStop = 14,
        BrCvt_TRepeat = 16,
        BrCvt_TZeroes = 17,
        BrCvt_TNineteen = 19,
      };
      enum brcvt_const : unsigned int {
        brcvt_MetaHeaderLen = 6,
        brcvt_CLenExtent = sizeof(brcvt_clen)/sizeof(brcvt_clen[0]),
        brcvt_Margin = 16,
        brcvt_BlockCountBits = 5,
        brcvt_NoSkip = std::numeric_limits<short>::max(),
        brcvt_RepeatBit = 128,
        brcvt_ZeroBit = 64,
      };

      struct brcvt_token {
        uint32 first;
        unsigned short second;
        unsigned char state;
      };
    }

    //BEGIN brcvt / static
    void brcvt_next_block(brcvt_state& state) noexcept {
      if (state.meta_index < state.metadata.size())
        state.state = BrCvt_MetaStart;
      else if (state.emptymeta)
        state.state = BrCvt_MetaStart;
      else if (!(state.h_end&1u))
        state.state = BrCvt_Nibbles;
      else
        state.state = BrCvt_LastCheck;
    }

    bool brcvt_can_add_input(brcvt_state const& ps) noexcept {
      if (ps.h_end&1u)
        return 0;
      else if (ps.state == BrCvt_Nibbles)
        return ps.bit_length == 0;
      else return (ps.state >= BrCvt_MetaStart)
        && (ps.state <= BrCvt_MetaText);
    }

    gasp_vector& brcvt_active_forest(brcvt_state& state) noexcept {
      switch (state.state) {
      case BrCvt_GaspVectorI: return state.insert_forest;
      case BrCvt_GaspVectorD: return state.distance_forest;
      case BrCvt_GaspVectorL:
      default: return state.literals_forest;
      }
    }
    unsigned short& brcvt_active_skip(brcvt_state& state) noexcept {
      switch (state.state) {
      case BrCvt_GaspVectorI: return state.insert_skip;
      case BrCvt_GaspVectorD: return state.distance_skip;
      case BrCvt_GaspVectorL:
      default: return state.literal_skip;
      }
    }


    brcvt_token brcvt_next_token
      (brcvt_state::forward_box& fwd, context_span const& guesses,
        unsigned char const* data, std::size_t size)
    {
      brcvt_token out = {};
      if (fwd.ostate == 0) {
        fwd.stop = static_cast<uint32>(guesses.count > 1
          ? guesses.offsets[1] : guesses.total_bytes);
        fwd.ostate = BrCvt_DataInsertCopy;
      }
      if (fwd.i >= size)
        return brcvt_token{};
      switch (fwd.ostate) {
      case BrCvt_DataInsertCopy:
        {
          /* acquire all inserts */
          uint32 total = 0;
          std::size_t next_i;
          uint32 literals = 0;
          uint32 first_literals = 0;
          out.state = BrCvt_DataInsertCopy;
          /* compose insert length */
          for (next_i = fwd.i; next_i < size; ++next_i) {
            unsigned short next_span = 0;
            unsigned char const ch = data[next_i];
            if (literals) {
              literals -= 1;
              continue;
            } else if (ch & 128u)
              break;
            next_span = (ch & 63u);
            if (ch & 64u) {
              assert(next_i < size-1u);
              next_i += 1;
              next_span = (next_span << 8) + data[next_i] + 64u;
            }
            if (next_span == 0 && first_literals == 0) {
              fwd.i = next_i+1;
              if (fwd.i >= size) {
                fwd.ostate = BrCvt_Done;
                return brcvt_token{};
              }
              continue;
            }
            if (!first_literals)
              first_literals = next_span;
            literals = next_span;
            assert(literals <= 16777216-total);
            total += literals;
          }
          out.first = total;
          if (next_i >= size)
            break;
          /* parse copy length */{
            unsigned char const ch = data[next_i];
            unsigned short next_span = 0;
            assert(ch & 128u);
            next_span = (ch & 63u);
            if (ch & 64u) {
              assert(next_i < size-1u);
              next_i += 1;
              next_span = (next_span << 8) + data[next_i] + 64u;
            }
            out.second = next_span;
          }
          fwd.i += (1 + ((data[fwd.i]&64u)!=0));
          fwd.ostate = BrCvt_Literal;
        } break;
      default:
        out.state = BrCvt_BadToken;
        break;
      }
      return out;
    }

    void brcvt_reset_compress(brcvt_state& state, api_error& ae) noexcept {
      inscopy_codesort(state.blockcounts, ae);
      state.state = BrCvt_BlockTypesL;
      state.bit_length = 0;
      state.bits = 0;
      state.count = 0;
      state.blocktypeL_skip = brcvt_NoSkip;
      state.blockcountL_skip = brcvt_NoSkip;
      state.blocktypeI_skip = brcvt_NoSkip;
      state.blockcountI_skip = brcvt_NoSkip;
      state.blocktypeD_skip = brcvt_NoSkip;
      state.blockcountD_skip = brcvt_NoSkip;
      state.literal_skip = brcvt_NoSkip;
      state.insert_skip = brcvt_NoSkip;
      state.distance_skip = brcvt_NoSkip;
    }

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
        case BrCvt_WBits:
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
              state.state = BrCvt_LastCheck;
              state.bit_length = 0;
              break;
            }
          }
          if (state.bit_length >= 7)
            ae = api_error::Sanitize;
          break;
        case BrCvt_LastCheck:
          if (state.bit_length == 0) {
            state.h_end = (x!=0);
            state.bit_length = x?4:3;
            state.count = 0;
            state.bits = 0;
          }
          if (state.count == 1 && state.h_end) {
            if (x) {
              state.state = BrCvt_Done;
              ae = api_error::EndOfFile;
            }
          } else if (state.count >= state.bit_length-2) {
            state.bits |= (x<<(state.count-2));
          }
          if (state.count < state.bit_length)
            state.count += 1;
          if (state.count >= state.bit_length) {
            if (state.bits == 3) {
              state.state = BrCvt_MetaStart;
              state.bit_length = 0;
            } else {
              state.bit_length = (state.bits+4)*4;
              state.state = BrCvt_InputLength;
              state.backward = 0;
              state.count = 0;
            }
          }
          break;
        case BrCvt_MetaStart:
          if (state.bit_length == 0) {
            if (x != 0)
            ae = api_error::Sanitize;
            else {
              state.count = 4;
              state.bit_length = brcvt_MetaHeaderLen;
              state.bits = 0;
            }
          } else if (state.count < brcvt_MetaHeaderLen) {
            state.bits |= (x << (state.count-4));
            state.count += 1;
          }
          if (state.count >= brcvt_MetaHeaderLen) {
            state.bit_length = state.bits*8;
            state.state = (state.bits
              ? BrCvt_MetaLength : BrCvt_MetaText);
            state.count = 0;
            state.backward = 0;
          }
          break;
        case BrCvt_MetaLength:
          if (state.count < state.bit_length) {
            state.backward |= ((static_cast<uint32>(x)&1u)<<state.count);
            state.count += 1;
          }
          if (state.count >= state.bit_length) {
            if (!(state.backward>>(state.count-8)))
              ae = api_error::Sanitize;
            state.count = 0;
            state.state = BrCvt_MetaText;
          }
          break;
        case BrCvt_MetaText:
          if (x)
            ae = api_error::Sanitize;
          else if (state.backward == 0 && i == 7) {
            state.state = (state.h_end
              ? BrCvt_Done : BrCvt_LastCheck);
            if (state.h_end)
              ae = api_error::EndOfFile;
          }
          break;
        case BrCvt_Done: /* end of stream */
          ae = api_error::EndOfFile;
          break;
        case BrCvt_InputLength:
          if (state.count < state.bit_length) {
            state.backward |= (static_cast<uint32>(x)<<state.count);
            state.count += 1;
          }
          if (state.count >= state.bit_length) {
            state.bit_length = 0;
            state.count = 0;
            if (state.bit_length > 16 && (state.backward>>(state.bit_length-4))==0)
              ae = api_error::Sanitize;
            state.backward += 1;
            if (state.h_end)
              brcvt_reset_compress(state, ae);
            else
              state.state = BrCvt_CompressCheck;
          } break;
        case BrCvt_CompressCheck:
          if (x) {
            state.state = BrCvt_Uncompress;
          } else {
            brcvt_reset_compress(state, ae);
          } break;
        case BrCvt_Uncompress:
          if (x)
            ae = api_error::Sanitize;
          break;
        case BrCvt_BlockTypesL:
          if (state.bit_length == 0) {
            state.count = 1;
            state.bits = x;
            state.bit_length = (x ? 4 : 1);
          } else if (state.count < state.bit_length) {
            state.bits |= ((x&1u)<<(state.count++));
            if (state.count == 4 && (state.bits&14u))
              state.bit_length += (state.bits>>1);
          }
          if (state.count >= state.bit_length) {
            brcvt_reset19(state.treety);
            state.blocktypeL_index = 0;
            if (state.count == 1) {
              state.treety.count = 1;
              fixlist_preset(state.literal_blocktype, prefix_preset::BrotliSimple1);
              state.state += 4;
              state.blocktypeL_skip = 0;
              state.blocktypeL_max = 0;
              state.blocktypeL_remaining = std::numeric_limits<uint32>::max();
              state.bit_length = 0;
            } else {
              unsigned const alphasize = (state.bits>>4)+(1u<<(state.count-4))+1u;
              state.treety.count = static_cast<unsigned short>(alphasize);
              state.alphabits = util_bitwidth(alphasize+1u); //BITWIDTH(NBLTYPESx + 2)
              state.state += 1;
              state.blocktypeL_max = static_cast<unsigned char>(alphasize-1u);
            }
            try {
              state.literals_map = context_map(state.treety.count, 64);
            } catch (std::bad_alloc const& ) {
              ae = api_error::Memory;
              break;
            }
          } break;
        case BrCvt_BlockTypesLAlpha:
          {
            api_error const res = brcvt_inflow19(state.treety, state.literal_blocktype, x,
              state.alphabits);
            if (res == api_error::EndOfFile) {
              state.blocktypeL_skip = brcvt_resolve_skip(state.literal_blocktype);
              brcvt_reset19(state.treety);
              state.state += 1;
            } else if (res != api_error::Success)
              ae = res;
          } break;
        case BrCvt_BlockCountLAlpha:
          {
            api_error const res = brcvt_inflow19(state.treety, state.literal_blockcount, x, 5);
            if (res == api_error::EndOfFile) {
              state.blockcountL_skip = brcvt_resolve_skip(state.literal_blockcount);
              state.state += 1;
              state.bit_length = 0;
              state.bits = 0;
              state.count = 0;
              state.extra_length = 0;
              state.blocktypeL_index = 0;
              state.blocktypeL_remaining = 0;
              fixlist_codesort(state.literal_blockcount, ae);
              if (state.blockcountL_skip != brcvt_NoSkip)
                state.blocktypeL_remaining = brcvt_config_count(state, state.blockcountL_skip, state.state + 1);
            } else if (res != api_error::Success)
              ae = res;
          } break;
        case BrCvt_BlockStartL:
          if (state.extra_length == 0) {
            size_t code_index = ~0;
            state.bits = (state.bits<<1) | x;
            state.bit_length += 1;
            code_index = fixlist_codebsearch(state.literal_blockcount, state.bit_length, state.bits);
            if (code_index >= 26) {
              if (state.bit_length >= 15)
                ae = api_error::Sanitize;
              break;
            }
            prefix_line const& line = state.literal_blockcount[code_index];
            state.blocktypeL_remaining = brcvt_config_count(state, line.value, state.state + 1);
          } else if (state.bit_length < state.extra_length) {
            state.bits |= (x<< state.bit_length++);
            if (state.bit_length >= state.extra_length) {
              state.blocktypeL_remaining += state.bits;
              state.bits = 0;
              state.bit_length = 0;
              state.state += 1;
            }
          } else ae = api_error::Sanitize;
          break;
        case BrCvt_BlockTypesI:
          if (state.bit_length == 0) {
            state.count = 1;
            state.bits = x;
            state.bit_length = (x ? 4 : 1);
          } else if (state.count < state.bit_length) {
            state.bits |= ((x&1u)<<(state.count++));
            if (state.count == 4 && (state.bits&14u))
              state.bit_length += (state.bits>>1);
          }
          if (state.count >= state.bit_length) {
            brcvt_reset19(state.treety);
            state.blocktypeI_index = 0;
            if (state.count == 1) {
              state.treety.count = 1;
              fixlist_preset(state.insert_blocktype, prefix_preset::BrotliSimple1);
              state.state += 4;
              state.blocktypeI_skip = 0;
              state.blocktypeI_max = 0;
              state.blocktypeI_remaining = std::numeric_limits<uint32>::max();
              state.bit_length = 0;
            } else {
              unsigned const alphasize = (state.bits>>4)+(1u<<(state.count-4))+1u;
              state.treety.count = static_cast<unsigned short>(alphasize);
              state.alphabits = util_bitwidth(alphasize+1u); //BITWIDTH(NBLTYPESx + 2)
              state.state += 1;
              state.blocktypeI_max = static_cast<unsigned char>(alphasize-1u);
            }
            try {
              state.insert_forest = gasp_vector(state.treety.count);
            } catch (std::bad_alloc const&) {
              ae = api_error::Memory;
            }
          } break;
        case BrCvt_BlockTypesIAlpha:
          {
            api_error const res = brcvt_inflow19(state.treety, state.insert_blocktype, x,
              state.alphabits);
            if (res == api_error::EndOfFile) {
              state.blocktypeI_skip = brcvt_resolve_skip(state.insert_blocktype);
              brcvt_reset19(state.treety);
              state.state += 1;
            } else if (res != api_error::Success)
              ae = res;
          } break;
        case BrCvt_BlockCountIAlpha:
          {
            api_error const res = brcvt_inflow19(state.treety, state.insert_blockcount, x, 5);
            if (res == api_error::EndOfFile) {
              state.blockcountI_skip = brcvt_resolve_skip(state.insert_blockcount);
              state.state += 1;
              state.bit_length = 0;
              state.bits = 0;
              state.count = 0;
              state.extra_length = 0;
              state.blocktypeI_index = 0;
              state.blocktypeI_remaining = 0;
              fixlist_codesort(state.insert_blockcount, ae);
              if (state.blockcountI_skip != brcvt_NoSkip)
                state.blocktypeI_remaining = brcvt_config_count(state, state.blockcountI_skip, state.state + 1);
            } else if (res != api_error::Success)
              ae = res;
          } break;
        case BrCvt_BlockStartI:
          if (state.extra_length == 0) {
            size_t code_index = ~0;
            state.bits = (state.bits<<1) | x;
            state.bit_length += 1;
            code_index = fixlist_codebsearch(state.insert_blockcount, state.bit_length, state.bits);
            if (code_index >= 26) {
              if (state.bit_length >= 15)
                ae = api_error::Sanitize;
              break;
            }
            prefix_line const& line = state.insert_blockcount[code_index];
            state.blocktypeI_remaining = brcvt_config_count(state, line.value, state.state + 1);
          } else if (state.bit_length < state.extra_length) {
            state.bits |= (x<< state.bit_length++);
            if (state.bit_length >= state.extra_length) {
              state.blocktypeI_remaining += state.bits;
              state.bits = 0;
              state.bit_length = 0;
              state.state += 1;
            }
          } else ae = api_error::Sanitize;
          break;
        case BrCvt_BlockTypesD:
          if (state.bit_length == 0) {
            state.count = 1;
            state.bits = x;
            state.bit_length = (x ? 4 : 1);
          } else if (state.count < state.bit_length) {
            state.bits |= ((x&1u)<<(state.count++));
            if (state.count == 4 && (state.bits&14u))
              state.bit_length += (state.bits>>1);
          }
          if (state.count >= state.bit_length) {
            brcvt_reset19(state.treety);
            state.blocktypeD_index = 0;
            if (state.count == 1) {
              state.treety.count = 1;
              fixlist_preset(state.distance_blocktype, prefix_preset::BrotliSimple1);
              state.state += 4;
              state.blocktypeD_skip = 0;
              state.blocktypeD_max = 0;
              state.blocktypeD_remaining = std::numeric_limits<uint32>::max();
              state.bit_length = 0;
            } else {
              unsigned const alphasize = (state.bits>>4)+(1u<<(state.count-4))+1u;
              state.treety.count = static_cast<unsigned short>(alphasize);
              state.alphabits = util_bitwidth(alphasize+1u); //BITWIDTH(NBLTYPESx + 2)
              state.state += 1;
              state.blocktypeD_max = static_cast<unsigned char>(alphasize-1u);
            }
            try {
              state.distance_map = context_map(state.treety.count, 4);
            } catch (std::bad_alloc const& ) {
              ae = api_error::Memory;
              break;
            }
          } break;
        case BrCvt_BlockTypesDAlpha:
          {
            api_error const res = brcvt_inflow19(state.treety, state.distance_blocktype, x,
              state.alphabits);
            if (res == api_error::EndOfFile) {
              state.blocktypeD_skip = brcvt_resolve_skip(state.distance_blocktype);
              brcvt_reset19(state.treety);
              state.state += 1;
            } else if (res != api_error::Success)
              ae = res;
          } break;
        case BrCvt_BlockCountDAlpha:
          {
            api_error const res = brcvt_inflow19(state.treety, state.distance_blockcount, x, 5);
            if (res == api_error::EndOfFile) {
              state.blockcountD_skip = brcvt_resolve_skip(state.distance_blockcount);
              state.state += 1;
              state.bit_length = 0;
              state.bits = 0;
              state.count = 0;
              state.extra_length = 0;
              state.blocktypeD_index = 0;
              state.blocktypeD_remaining = 0;
              fixlist_codesort(state.distance_blockcount, ae);
              if (state.blockcountD_skip != brcvt_NoSkip)
                state.blocktypeD_remaining = brcvt_config_count(state, state.blockcountD_skip, state.state + 1);
            } else if (res != api_error::Success)
              ae = res;
          } break;
        case BrCvt_BlockStartD:
          if (state.extra_length == 0) {
            size_t code_index = ~0;
            state.bits = (state.bits<<1) | x;
            state.bit_length += 1;
            code_index = fixlist_codebsearch(state.distance_blockcount, state.bit_length, state.bits);
            if (code_index >= 26) {
              if (state.bit_length >= 15)
                ae = api_error::Sanitize;
              break;
            }
            prefix_line const& line = state.distance_blockcount[code_index];
            state.blocktypeD_remaining = brcvt_config_count(state, line.value, state.state + 1);
          } else if (state.bit_length < state.extra_length) {
            state.bits |= (x<< state.bit_length++);
            if (state.bit_length >= state.extra_length) {
              state.blocktypeD_remaining += state.bits;
              state.bits = 0;
              state.bit_length = 0;
              state.state += 1;
            }
          } else ae = api_error::Sanitize;
          break;
        case BrCvt_NPostfix:
          if (state.bit_length == 0)
            state.bits = 0;
          if (state.bit_length < 6) {
            state.bits |= (x<<state.bit_length);
            state.bit_length += 1;
          }
          if (state.bit_length >= 6) {
            unsigned const postfix = state.bits&3;
            unsigned const direct = (state.bits>>2)<<postfix;
            try {
              distance_ring tryring(true,direct,postfix);
              state.ring = tryring;
              state.try_ring = std::move(tryring);
            } catch (api_exception const& ae_ring) {
              ae = ae_ring.to_error();
              break;
            }
            state.state = BrCvt_ContextTypesL;
            state.bit_length = 0;
            state.bits = 0;
            state.count = static_cast<uint32>(state.literals_map.block_types());
            state.index = 0;
          } break;
        case BrCvt_ContextTypesL:
          if (state.bit_length < 2) {
            state.bits |= (x<<state.bit_length);
            state.bit_length += 1;
          }
          if (state.bit_length >= 2) {
            state.literals_map.set_mode(state.index,
              static_cast<context_map_mode>(state.bits), ae);
            state.index += 1;
            state.bits = 0;
            state.bit_length = 0;
          }
          if (state.index >= state.count) {
            state.state = BrCvt_TreeCountL;
            state.bit_length = 0;
          } break;
        case BrCvt_TreeCountL:
        case BrCvt_TreeCountD:
          if (state.bit_length == 0) {
            state.count = 1;
            state.bits = x;
            state.bit_length = (x ? 4 : 1);
          } else if (state.count < state.bit_length) {
            state.bits |= ((x&1u)<<(state.count++));
            if (state.count == 4 && (state.bits&14u))
              state.bit_length += (state.bits>>1);
          }
          if (state.count >= state.bit_length) {
            bool const literal = (state.state == BrCvt_TreeCountL);
            gasp_vector& forest_ref =
              (literal ? state.literals_forest : state.distance_forest);
            unsigned const alphasize = (state.count == 1) ? 1u
              : ((state.bits>>4)+(1u<<(state.count-4))+1u);
            if (alphasize > 256 || alphasize == 0) {
              ae = api_error::Sanitize;
              break;
            }
            try {
              forest_ref = gasp_vector(alphasize);
            } catch (std::bad_alloc const& ) {
              ae = api_error::Memory;
              break;
            }
            if (alphasize == 1) {
              context_map& map = (literal ? state.literals_map : state.distance_map);
              std::memset(&map(0,0), 0, map.block_types() * map.contexts());
              state.state = (literal ? BrCvt_TreeCountD : BrCvt_GaspVectorL);
              state.bit_length = 0;
              state.index = 0;
            } else {
              state.state += 1;
              state.bit_length = 0;
              state.rlemax = 0;
            }
          } break;
        case BrCvt_ContextRunMaxL:
        case BrCvt_ContextRunMaxD:
          if (state.bit_length == 0) {
            state.count = 1;
            state.bits = x;
            state.bit_length = (x ? 5 : 1);
          } else if (state.count < state.bit_length) {
            state.bits |= ((x&1u)<<(state.count++));
          }
          if (state.count >= state.bit_length) {
            gasp_vector const& forest = (state.state == BrCvt_ContextRunMaxL)
              ? state.literals_forest : state.distance_forest;
            size_t const ntrees = forest.size();
            state.rlemax = (state.bits ? (state.bits>>1)+1u : 0u);
            state.bit_length = 0;
            state.state += 1;
            brcvt_reset19(state.treety);
            state.treety.count = static_cast<unsigned short>(state.rlemax + ntrees);
            state.alphabits = static_cast<unsigned char>(state.rlemax + ntrees);
          } break;
        case BrCvt_ContextPrefixL:
        case BrCvt_ContextPrefixD:
          {
            api_error const res = brcvt_inflow19(state.treety, state.context_tree, x,
              state.alphabits);
            if (res == api_error::EndOfFile) {
              context_map& map = (state.state == BrCvt_ContextPrefixL)
                ? state.literals_map : state.distance_map;
              state.context_skip = brcvt_resolve_skip(state.context_tree);
              brcvt_reset19(state.treety);
              state.bit_length = 0;
              state.bits = 0;
              state.index = 0;
              state.count = map.block_types()*map.contexts();
              if (state.context_skip == brcvt_NoSkip) {
                state.state += 1;
              } else if (state.context_skip == 0 || state.context_skip > state.rlemax) {
                unsigned char const fill = (state.context_skip ?
                  state.context_skip - state.rlemax : 0);
                std::memset(map.data(), fill, state.count*sizeof(char));
                state.state += 3;
              } else {
                state.extra_length = state.context_skip;
                state.state += 2;
              }
            } else if (res != api_error::Success)
              ae = res;
          } break;
        case BrCvt_ContextValuesL:
        case BrCvt_ContextValuesD:
          if (state.bit_length < 16) {
            context_map& map = (state.state == BrCvt_ContextValuesL)
              ? state.literals_map : state.distance_map;
            state.bits = (state.bits<<1)|x;
            state.bit_length += 1;
            std::size_t const line_index = fixlist_codebsearch(state.context_tree, state.bit_length, state.bits);
            if (line_index >= state.context_tree.size())
              break;
            state.bits = 0;
            state.bit_length = 0;
            prefix_line const& line = state.context_tree[line_index];
            if (line.value == 0 || line.value > state.rlemax) {
              /* single value */
              auto const value = static_cast<unsigned char>(line.value ? line.value-state.rlemax : 0);
              map.data()[state.index] = value;
              state.index += 1;
              if (state.index >= state.count)
                state.state += 2;
            } else {
              state.extra_length = line.value;
              state.state += 1;
            }
          }
          if (state.bit_length >= 16)
            ae = api_error::Sanitize;
          break;
        case BrCvt_ContextRepeatL:
        case BrCvt_ContextRepeatD:
          if (state.bit_length < state.extra_length) {
            state.bits |= (x<<state.bit_length);
            state.bit_length += 1;
          }
          if (state.bit_length >= state.extra_length) {
            context_map& map = (state.state == BrCvt_ContextRepeatL)
              ? state.literals_map : state.distance_map;
            std::size_t const total = state.bits | (1ul<<state.extra_length);
            if (total > state.count - state.index) {
              ae = api_error::Sanitize;
              break;
            }
            std::memset(map.data()+state.index, 0, total*sizeof(char));
            state.index += total;
            state.bits = 0;
            state.bit_length = 0;
            if (state.index >= state.count)
              state.state += 1; /* IMTF bit */
            else if (state.context_skip == brcvt_NoSkip)
              state.state -= 1; /* next */
            else/* stay put and */break;
          } break;
        case BrCvt_ContextInvertL:
        case BrCvt_ContextInvertD:
          {
            bool const literals = (state.state == BrCvt_ContextInvertL);
            if (x) {
              context_map& map = (literals ? state.literals_map : state.distance_map);
              ctxtmap_revert_movetofront(map);
            }
            state.state = (literals ? BrCvt_TreeCountD : BrCvt_GaspVectorL);
            state.bit_length = 0;
            state.bits = 0;
            state.index = 0;
          } break;
        case BrCvt_GaspVectorL:
        case BrCvt_GaspVectorI:
        case BrCvt_GaspVectorD:
          if (state.bit_length == 0) {
            state.bit_length = 1;
            brcvt_reset19(state.treety);
            if (state.state == BrCvt_GaspVectorD) {
              state.alphabits = (16 + state.ring.get_direct()
                + (48 << state.ring.get_postfix()));
            } else state.alphabits = ((state.state == BrCvt_GaspVectorL) ? 256 : 704);
          }
          if (state.bit_length > 0) {
            brcvt_state::treety_box& treety = state.treety;
            gasp_vector& forest = brcvt_active_forest(state);
            prefix_list& tree = forest[state.index];
            api_error const res = brcvt_inflow19(treety, tree, x, state.alphabits);
            if (res == api_error::EndOfFile) {
              if (state.index == 0)
                brcvt_active_skip(state) = brcvt_resolve_skip(tree);
              state.bit_length = 0;
              state.index += 1;
              if (state.index >= forest.size()) {
                state.state += 1;
                state.index = 0;
                if (state.state != BrCvt_DataInsertCopy || state.insert_skip == brcvt_NoSkip)
                  break;
                /* TODO handle an insert skip */
              }
            } else if (res != api_error::Success)
              ae = res;
          } break;
        case BrCvt_DataInsertCopy:
          /* TODO this state */
          break;
        case 5000019: /* generate code trees */
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
        case 5000020: /* alpha bringback */
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
        case 5000016: /* copy previous code length */
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
        case 5000017: /* copy zero length */
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
        case 5000018: /* copy zero length + 11 */
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

    unsigned short brcvt_resolve_skip(prefix_list const& prefixes) noexcept {
      constexpr unsigned long Mark = std::numeric_limits<unsigned long>::max();
      unsigned long last_nonzero = Mark;
      if (prefixes.size() == 1) {
        return static_cast<unsigned short>(prefixes[0].value);
      } else for (prefix_line const& line : prefixes) {
        if (line.len == 0)
          continue;
        else if (last_nonzero != Mark)
          return brcvt_NoSkip;
        else last_nonzero = line.value;
      }
      return last_nonzero == Mark ? brcvt_NoSkip : static_cast<unsigned short>(last_nonzero);
    }

    uint32 brcvt_config_count
      (brcvt_state& state, unsigned long value, unsigned char next_state)
    {
      insert_copy_row const& row = state.blockcounts[static_cast<std::size_t>(value)];
      state.extra_length = row.insert_bits;
      state.bits = 0;
      state.bit_length = 0;
      if (!state.extra_length)
        state.state = next_state;
      return row.insert_first;
    }

    api_error brcvt_inflow19(brcvt_state::treety_box& treety,
      prefix_list& prefixes, unsigned x, unsigned alphabits)
    {
      switch (treety.state) {
      case BrCvt_TComplex:
        treety.bits |= (x<<(treety.bit_length++));
        if (treety.bit_length == 2) {
          if (treety.bits == 1) {
            treety.state = BrCvt_TSimpleCount;
            treety.bits = 0;
            treety.bit_length = 0;
          } else {
            size_t i;
            try {
              treety.nineteen = prefix_list(brcvt_CLenExtent);
              prefixes = prefix_list(treety.count);
            } catch (std::bad_alloc const&) {
              return api_error::Memory;
            }
            for (i = 0; i < brcvt_CLenExtent; ++i) {
              prefix_line& line = treety.nineteen[i];
              line.value = brcvt_clen[i];
              line.code = 0;
              line.len = 0;
            }
            treety.index = treety.bits;
            treety.state = BrCvt_TNineteen;
            treety.bits = 0;
            treety.bit_length = 0;
            treety.nonzero = 0;
            treety.len_check = 0;
            treety.last_len = 255;
          }
        } break;
      case BrCvt_TSimpleCount:
        treety.bits |= (x<<(treety.bit_length++));
        if (treety.bit_length == 2) {
          unsigned short const new_count = treety.bits+1;
          try {
            treety.nineteen = prefix_list(new_count);
          } catch (std::bad_alloc const&) {
            return api_error::Memory;
          }
          treety.count = new_count;
          treety.index = 0;
          treety.bit_length = 0;
          treety.bits = 0;
          treety.state = BrCvt_TSimpleAlpha;
        } break;
      case BrCvt_TSimpleAlpha:
        treety.bits |= (x<<(treety.bit_length++));
        if (treety.bit_length == alphabits) {
          treety.nineteen[treety.index].value = treety.bits;
          treety.index += 1;
          treety.bits = 0;
          treety.bit_length = 0;
          if (treety.index >= treety.count) {
            if (treety.count == 4) {
              treety.state = BrCvt_TSimpleFour;
              break;
            }
            treety.state = BrCvt_TDone;
            api_error ae;
            fixlist_preset(prefixes, static_cast<prefix_preset>(treety.count), ae);
            return ae == api_error::Success
              ? brcvt_transfer19(prefixes, treety.nineteen) : api_error::Memory;
          }
        } break;
      case BrCvt_TSimpleFour:
        {
          treety.state = BrCvt_TDone;
          api_error ae = api_error::Success;
          fixlist_preset(prefixes, static_cast<prefix_preset>(4+x), ae);
          return ae == api_error::Success
            ? brcvt_transfer19(prefixes, treety.nineteen) : api_error::Memory;
        } break;
      case BrCvt_TNineteen:
        treety.bits = (treety.bits<<1)|x;
        treety.bit_length += 1;
        if (treety.bit_length > 4)
          return api_error::Sanitize;
        else {
          unsigned short len = 255;
          switch (treety.bit_length) {
          case 2:
            if (treety.bits < 3u) {
              len = (treety.bits>0)?2+treety.bits:0;
            } break;
          case 3:
            if (treety.bits == 6u) {
              len = 2;
            } break;
          case 4:
            len = (treety.bits-14u)*4u + 1u;
            break;
          }
          if (len > 5)
            return api_error::Success;
          treety.bits = 0;
          treety.bit_length = 0;
          if (len > 0) {
            treety.len_check += (32 >> len);
            treety.nonzero += 1;
            if (treety.len_check > 32)
              return api_error::Sanitize;
          }
          if (treety.nonzero == 1)
            treety.singular = brcvt_clen[treety.index];
          treety.nineteen[treety.index++].len = len;
          if (treety.index >= brcvt_CLenExtent || treety.len_check >= 32) {
            if (treety.nonzero > 1 && treety.len_check != 32)
              return api_error::Sanitize;
            treety.state = BrCvt_TSymbols;
            treety.len_check = 0;
            treety.last_len = 8;
            treety.last_nonzero = 8;
            treety.bits = 0;
            treety.index = 0;
            treety.bit_length = 0;
            treety.last_repeat = 0;
            if (treety.nonzero == 1) {
              unsigned stop;
              for (stop = 0; stop < 704 && treety.state == BrCvt_TSymbols; ++stop) {
                api_error const res = brcvt_post19(treety, prefixes, treety.singular);
                if (res != api_error::Success)
                  return res;
              }
            } else {
              api_error res;
              fixlist_valuesort(treety.nineteen, res);
              fixlist_gen_codes(treety.nineteen, res);
              if (res != api_error::Success)
                return api_error::Sanitize;
              fixlist_codesort(treety.nineteen);
            }
          }
        } break;
      case BrCvt_TRepeat:
      case BrCvt_TZeroes:
        treety.bits |= (x<<(treety.bit_length++));
        if (treety.bit_length < treety.state-BrCvt_TRepeatStop)
          break;
        else {
          api_error const res = brcvt_post19(treety, prefixes, treety.bits);
          if (res != api_error::Success)
            return res;
        }
        if (treety.nonzero != 1)
          break;
      case BrCvt_TSymbols:
        if (treety.nonzero == 1) {
          unsigned stop;
          for (stop = 0; stop < 704 && treety.state == BrCvt_TSymbols; ++stop) {
            api_error const res = brcvt_post19(treety, prefixes, treety.singular);
            if (res != api_error::Success)
              return res;
          }
        } else {
          std::size_t code_index;
          treety.bits = (treety.bits<<1)|x;
          treety.bit_length += 1;
          code_index = fixlist_codebsearch(treety.nineteen, treety.bit_length, treety.bits);
          if (code_index < treety.nineteen.size()) {
            api_error const res = brcvt_post19(treety, prefixes,
              treety.nineteen[code_index].value);
            if (res != api_error::Success)
              return res;
          } else if (treety.bit_length > 5)
            return api_error::Sanitize;
        } break;
      default:
        return api_error::Sanitize;
      }
      return api_error::Success;
    }
    api_error brcvt_transfer19(prefix_list& prefixes, prefix_list const& nineteen) {
        std::size_t const n = nineteen.size();
        for (std::size_t i = 0; i < n; ++i) {
          prefixes[i].value = nineteen[i].value;
        }
        api_error ae = api_error::Success;
        fixlist_valuesort(prefixes, ae);
        if (ae != api_error::Success)
          return ae;
        api_error ae2 = api_error::Success;
        fixlist_gen_codes(prefixes, ae2);
        if (ae2 != api_error::Success)
          return ae2;
        return api_error::EndOfFile;
    }

    void brcvt_reset19(brcvt_state::treety_box& treety) {
      prefix_list nineteen = std::move(treety.nineteen);
      block_string compress = std::move(treety.sequence_list);
      treety = {};
      treety.sequence_list = std::move(compress);
      treety.sequence_list.clear();
      treety.nineteen = std::move(nineteen);
    }

    api_error brcvt_post19(brcvt_state::treety_box& treety,
      prefix_list& prefixes, unsigned short value)
    {
      if (treety.state == BrCvt_TRepeat) {
        unsigned short const repeat_total =
          ((treety.last_repeat > 0) ? 4*(treety.last_repeat-2) : 0) + (value+3);
        unsigned short const repeat_gap = repeat_total - treety.last_repeat;
        if (repeat_gap > treety.count - treety.index)
          return api_error::Sanitize;
        treety.last_repeat = repeat_total;
        for (unsigned i = 0; i < repeat_gap; ++i) {
          unsigned short const push = (32768>>treety.last_nonzero);
          if (push > 32768-treety.len_check)
            return api_error::Sanitize;
          prefixes[treety.index].value = treety.index;
          prefixes[treety.index].len = treety.last_nonzero;
          treety.len_check += push;
          treety.index += 1;
        }
        treety.last_len = treety.last_nonzero;
      } else if (treety.state == BrCvt_TZeroes) {
        unsigned short const repeat_total =
          ((treety.last_repeat > 0) ? 8*(treety.last_repeat-2) : 0) + (value+3);
        unsigned short const repeat_gap = repeat_total - treety.last_repeat;
        if (repeat_gap > treety.count - treety.index)
          return api_error::Sanitize;
        treety.last_repeat = repeat_total;
        for (unsigned i = 0; i < repeat_gap; ++i) {
          prefixes[treety.index].value = treety.index;
          prefixes[treety.index].len = 0;
          treety.index += 1;
        }
        treety.last_len = 0;
      } else if (treety.state == BrCvt_TSymbols) {
        if (value < 16) {
          prefixes[treety.index].value = treety.index;
          prefixes[treety.index].len = value;
          treety.index += 1;
          treety.last_repeat = 0;
          treety.last_len = value;
          treety.bits = 0;
          treety.bit_length = 0;
          if (value) {
            unsigned short const push = (32768>>value);
            if (push > 32768-treety.len_check)
              return api_error::Sanitize;
            treety.last_nonzero = static_cast<unsigned char>(value);
            treety.len_check += push;
          }
        } else if (value == 16) {
          if (treety.last_len == 0)
            treety.last_repeat = 0;
          treety.state = BrCvt_TRepeat;
          treety.bits = 0;
          treety.bit_length = 0;
        } else if (value == 17) {
          if (treety.last_len != 0)
            treety.last_repeat = 0;
          treety.state = BrCvt_TZeroes;
          treety.bits = 0;
          treety.bit_length = 0;
        } else return api_error::Sanitize;
      } else return api_error::Sanitize;
      if (treety.index >= treety.count || treety.len_check >= 32768) {
        api_error ae = api_error::Success;
        treety.state = BrCvt_TDone;
        fixlist_valuesort(prefixes, ae);
        if (ae != api_error::Success)
          return api_error::Sanitize;
        fixlist_gen_codes(prefixes, ae);
        if (ae != api_error::Success)
          return api_error::Sanitize;
        fixlist_codesort(prefixes, ae);
        return ae == api_error::Success ? api_error::EndOfFile : api_error::Sanitize;
      }
      return api_error::Success;
    }

    api_error brcvt_outflow19(brcvt_state::treety_box& treety,
      prefix_list& prefixes, unsigned& x, unsigned alphabits)
    {
      switch (treety.state) {
      case BrCvt_TComplex:
        if (treety.bit_length == 0) {
          prefix_preset const preset = fixlist_match_preset(prefixes);
          if (preset != prefix_preset::BrotliComplex) {
            treety.bits = 1;
            treety.count = static_cast<unsigned short>(preset);
          } else {
            constexpr uint32 lines = sizeof(brcvt_clen);
            int res = 0;
            prefix_histogram histogram;
            if (util_move_make(histogram, lines) != api_error::Success)
              return api_error::Memory;
            std::fill(histogram.begin(), histogram.end(), 0);
            uint32 nonzero = 0;
            if (brcvt_make_sequence(treety, prefixes) != api_error::Success)
              return api_error::Memory;
            for (uint32 i = 0; i < treety.sequence_list.size(); ++i) {
              unsigned char const code = treety.sequence_list[i];
              assert(code < histogram.size());
              histogram[code] += 1;
              if (code >= 16)
                i += 1;
            }
            uint32 shortcut = 0;
            for (uint32 shortcut = 0; shortcut < 3; ++shortcut) {
              if (histogram[brcvt_clen[shortcut]])
                break;
            }
            treety.bits = (shortcut - (shortcut==1));
            if (util_move_make(treety.nineteen, lines) != api_error::Success)
              return api_error::Memory;
            for (uint32 i = 0; i < lines; ++i)
              treety.nineteen[i].value = i;
            api_error ae;
            fixlist_gen_lengths(treety.nineteen, histogram, 5, ae);
            if (ae != api_error::Success)
              return api_error::Memory;
            fixlist_gen_codes(treety.nineteen, ae);
            assert(ae == api_error::Success);
            treety.count = 0;
            treety.nonzero = 0;
            for (uint32 i = 0; i < lines; ++i) {
              unsigned char clen = brcvt_clen[i];
              if (histogram[clen] == 0)
                continue;
              else if (!treety.nonzero)
                treety.nonzero = clen;
              treety.count = i+1;
              nonzero += 1;
            }
            if (nonzero < 2)
              treety.count = static_cast<unsigned short>(lines);
            else nonzero = 0;
          }
        }
        x = (treety.bits>>treety.bit_length++)&1u;
        if (treety.bit_length >= 2) {
          treety.bit_length = 0;
          if (treety.bits == 1) {
            treety.state = BrCvt_TSimpleCount;
            treety.index = 0;
          } else {
            treety.state = BrCvt_TNineteen;
            treety.index = treety.bits;
          }
        } break;
      case BrCvt_TSimpleCount:
        if (treety.bit_length == 0) {
          treety.bits = treety.count - 1 - (treety.count==static_cast<unsigned>(prefix_preset::BrotliSimple4B));
        }
        x = (treety.bits>>treety.bit_length++)&1u;
        if (treety.bit_length >= 2) {
          treety.state = BrCvt_TSimpleAlpha;
          treety.bit_length = 0;
        } break;
      case BrCvt_TSimpleAlpha:
        if (treety.bit_length == 0) {
          if (treety.index >= prefixes.size())
            return api_error::Sanitize;
          treety.bits = static_cast<unsigned short>(prefixes[treety.index].value);
        }
        x = (treety.bits>>treety.bit_length++)&1u;
        if (treety.bit_length >= alphabits) {
          unsigned const effective = treety.count - (treety.count==static_cast<unsigned>(prefix_preset::BrotliSimple4B));
          treety.index += 1;
          treety.bit_length = 0;
          if (treety.index < effective)
            break;
          treety.state = BrCvt_TDone;
          if (treety.count < static_cast<unsigned>(prefix_preset::BrotliSimple4A))
            return api_error::EndOfFile;
          treety.state = BrCvt_TSimpleFour;
        } break;
      case BrCvt_TSimpleFour:
        x = (treety.count==static_cast<unsigned>(prefix_preset::BrotliSimple4B));
        treety.state = BrCvt_TDone;
        return api_error::EndOfFile;
      case BrCvt_TNineteen:
        if (treety.bit_length == 0) {
          unsigned short const len = treety.nineteen[treety.index].len;
          switch (len) {
          case 0: treety.bit_length = 2; treety.bits = 0u; break;
          case 1: treety.bit_length = 4; treety.bits = 14u; break;
          case 2: treety.bit_length = 3; treety.bits = 6u; break;
          case 3: treety.bit_length = 2; treety.bits = 1u; break;
          case 4: treety.bit_length = 2; treety.bits = 2u; break;
          case 5: treety.bit_length = 2; treety.bits = 15u; break;
          default: return api_error::Sanitize;
          }
        }
        if (treety.bit_length > 0)
          x = (treety.bits>>--treety.bit_length)&1u;
        if (treety.bit_length == 0) {
          treety.index += 1;
          if (treety.index < treety.count)
            break;
          assert(treety.bit_length == 0);
          if (treety.nonzero >= 16) {
            treety.state = treety.nonzero;
            treety.index = 1;
            assert(treety.sequence_list.size() >= 2);
          } else if (treety.nonzero > 0) {
            treety.state = BrCvt_TDone;
            return api_error::EndOfFile;
          } else {
            treety.index = 0;
            treety.state = BrCvt_TSymbols;
          }
        } break;
      case BrCvt_TRepeat:
      case BrCvt_TZeroes:
        if (treety.bit_length == 0) {
          treety.bits = treety.sequence_list[treety.index];
          treety.count = treety.state - 14;
        }
        x = (treety.bits>>treety.bit_length++)&1u;
        if (treety.bit_length >= treety.count) {
          treety.index += 1;
          if (treety.index >= treety.sequence_list.size()) {
            treety.state = BrCvt_TDone;
            return api_error::EndOfFile;
          } else if (treety.nonzero) {
            assert(treety.sequence_list[treety.index] == treety.nonzero);
            treety.index += 1;
          } else treety.state = BrCvt_TSymbols;
        } break;
      case BrCvt_TSymbols:
        if (treety.bit_length == 0) {
          unsigned char const code = treety.sequence_list[treety.index];
          prefix_line const& line = treety.nineteen[code];
          treety.bits = line.code;
          treety.bit_length = line.len;
        }
        if (treety.bit_length > 0)
          x = (treety.bits>>--treety.bit_length)&1u;
        if (treety.bit_length == 0) {
          unsigned char const code = treety.sequence_list[treety.index];
          treety.index += 1;
          if (treety.index >= treety.sequence_list.size()) {
            if (code >= 16)
              return api_error::Sanitize;
            treety.state = BrCvt_TDone;
            return api_error::EndOfFile;
          } else if (code >= 16)
            treety.state = code;
          // otherwise stay put
        } break;
      case BrCvt_TDone:
        return api_error::EndOfFile;
      default:
        return api_error::Sanitize;
      }
      return api_error::Success;
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

    api_error brcvt_make_sequence
      (brcvt_state::treety_box& state, prefix_list const& literals) noexcept
    {
      unsigned int len = ~0u, len_count = 0u;
      size_t const lit_sz = literals.size();
      size_t i;
      auto const* end = literals.begin();
      state.sequence_list.clear();
      for (prefix_line const& line : literals) {
        if (line.len > 0)
          end = (&line)+1;
      }
      for (auto const* line = literals.begin(); line < end; ++line) {
        unsigned int const n = line->len;
        if (len != n) {
          api_error const ae = brcvt_post_sequence
            (state.sequence_list, len, len_count);
          if (ae != api_error::Success)
            return ae;
          len = n;
          len_count = 1u;
        } else len_count += 1u;
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
      api_error ae = api_error::Success;
      unsigned int i;
      if (count == 0u)
        return api_error::Success;
      else if (count < 4u) {
        for (unsigned i = 0; i < count && ae == api_error::Success; ++i)
          s.push_back(static_cast<unsigned char>(len), ae);
      } else if (len == 0u) {
        unsigned const recount = count-3;
        int width = static_cast<int>(util_bitwidth(recount));
        for (i = width-width%3; i >= 0 && ae == api_error::Success; i -= 3) {
          unsigned int const x = (recount>>i)&7u;
          ae = brcvt_post_two(s, 17, static_cast<unsigned char>(x));
        }
      } else {
        s.push_back(static_cast<unsigned char>(len), ae);
        unsigned const recount = count-3;
        unsigned const width = util_bitwidth(recount);
        for (int i = static_cast<int>(width&~1u); i >= 0 && ae == api_error::Success; i -= 2) {
          unsigned int const x = (recount>>i)&3u;
          ae = brcvt_post_two(s, 16, static_cast<unsigned char>(x));
        }
      }
      return ae;
    }

    api_error brcvt_check_compress(brcvt_state& state) {
      int guess_nonzero = 0;
      size_t accum = 0;
      /* calculate the guesses */
      prefix_histogram ctxt_histogram(4);
      std::fill(ctxt_histogram.begin(), ctxt_histogram.end(), 0);
      state.guesses = {};
      ctxtspan_subdivide(state.guesses,
        state.buffer.input_data().data(), state.buffer.input_size(),
        brcvt_Margin);
      if (state.literal_blocktype.size() != 4)
        state.literal_blocktype = prefix_list(4);
      state.guess_offset = (state.guesses.count > 0)
        ? static_cast<unsigned char>(state.guesses.modes[0]) : 0;
      for (unsigned ctxt_i = 0; ctxt_i < state.guesses.count; ++ctxt_i) {
        context_map_mode const mode = state.guesses.modes[ctxt_i];
        assert(mode < context_map_mode::ModeMax);
        ctxt_histogram[(static_cast<unsigned>(mode)+4u-state.guess_offset)%4u] += 1;
      }
      for (unsigned ctxt_i = 0; ctxt_i < 4u; ++ctxt_i) {
        prefix_line& line = state.literal_blocktype[ctxt_i];
        line.value = ctxt_i;
        if (ctxt_histogram[ctxt_i] > 0)
          guess_nonzero += 1;
      }
      fixlist_gen_lengths(state.literal_blocktype, ctxt_histogram, 3);
      fixlist_gen_codes(state.literal_blocktype);
      prefix_preset blocktype_tree = fixlist_match_preset(state.literal_blocktype);
      if (blocktype_tree == prefix_preset::BrotliComplex)
        return api_error::Sanitize;
      state.blocktype_simple = static_cast<unsigned char>(blocktype_tree);
      accum += 4;
      std::size_t const btypes = state.literal_blocktype.size();
      accum += (blocktype_tree >= prefix_preset::BrotliSimple3 ? 3 : 2) * btypes;
      accum += (blocktype_tree >= prefix_preset::BrotliSimple4A);
      if (btypes != state.literals_map.block_types()) {
        try {
          state.literals_map = context_map(btypes, 64);
        } catch (std::bad_alloc const&) {
          return api_error::Memory;
        }
      }
      for (std::size_t btype_j = 0; btype_j < btypes; ++btype_j) {
        for (unsigned ctxt_i = 0; ctxt_i < 64; ++ctxt_i)
          state.literals_map(btype_j, ctxt_i) = static_cast<unsigned char>(btype_j);
      }
      state.context_encode.clear();
      try {
        /* prepare the fixed-size forests */
        if (state.insert_forest.size() != 1)
          state.insert_forest = gasp_vector(1);
        if (state.distance_forest.size() != 1)
          state.distance_forest = gasp_vector(1);
        /* prepare the variable-size forest */
        if (state.literals_forest.size() != btypes)
          state.literals_forest = gasp_vector(btypes);
      } catch (std::bad_alloc const& ) {
        return api_error::Memory;
      }
      /* fill the histograms */{
        std::size_t const size = state.buffer.str().size();
        unsigned char const* const data = state.buffer.str().data();
        std::size_t stop = (state.guesses.count > 1
            ? state.guesses.offsets[1] : state.guesses.total_bytes);
        std::array<uint32, CtxtSpan_Size+1> literal_lengths;
        uint32 literal_counter = 0;
        brcvt_state::forward_box try_fwd = {0};
        std::fill(state.ins_histogram.begin(), state.ins_histogram.end(), 0);
        std::fill(state.dist_histogram.begin(), state.dist_histogram.end(), 0);
        for (prefix_histogram& gram : state.lit_histogram)
          std::fill(gram.begin(), gram.end(), 0);
        for (std::size_t i = 0; i < size; ++i) {
          brcvt_token next = brcvt_next_token(try_fwd, state.guesses, data, size);
          if (try_fwd.i <= i)
            return api_error::Sanitize;
          i = try_fwd.i-1;
          switch (next.state) {
          case BrCvt_DataInsertCopy:
            /* TODO use the inscopy instance to update the histogram */
            break;
          default:
            return api_error::Sanitize;
          }
        }
      }
      /* TODO the rest */
      return api_error::Success;
    }

    api_error brcvt_encode_map(block_string& buffer, std::size_t zeroes,
      unsigned char map_datum, unsigned& rlemax_ptr) noexcept
    {
      unsigned char code[3] = {0};
      int len = 0;
      if (zeroes > 0) {
        code[len] = util_bitwidth(static_cast<unsigned>(zeroes))-1u;
        len += 1;
        if (zeroes > 1) {
          if (code[0] > rlemax_ptr)
            rlemax_ptr = code[0];
          code[0] |= brcvt_ZeroBit;
          code[len] = (unsigned char)(((1u<<code[0])-1u)&zeroes) | brcvt_RepeatBit;
          len += 1;
        }
      }
      if (map_datum) {
        code[len] = map_datum;
        len += 1;
      }
      api_error ae = api_error::Success;
      for (int i = 0; i < len && ae == api_error::Success; ++i)
        buffer.push_back(code[i], ae);
      return ae;
    }

    api_error brcvt_out_bits
      ( brcvt_state& state,
        unsigned char const* from, unsigned char const* from_end,
        unsigned char const*& p, unsigned char& y)
    {
      unsigned int i;
      api_error ae = api_error::Success;
      // Restore in-progress byte.
      y = state.write_scratch;
      state.write_scratch = 0;
      // Iterate through remaining bits.
      for (i = state.bit_index; i < 8u && ae == api_error::Success; ++i) {
        unsigned int x = 0u;
        if (brcvt_can_add_input(state)) {
          uint32 const input_space =
              state.buffer.capacity() - state.buffer.input_size();
          std::size_t const from_count = from_end - p;
          std::size_t const min_count = (input_space < from_count)
            ? static_cast<std::size_t>(input_space) : from_count;
          state.buffer.write(p, min_count, ae);
          if (ae != api_error::Success)
            break;
          p += min_count;
        }
        switch (state.state) {
        case BrCvt_WBits: /* WBITS */
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
            brcvt_next_block(state);
            state.bit_length = 0;
          }
          break;
        case BrCvt_MetaStart:
          if (state.bit_length == 0u) {
            bool const actual_meta =
              state.meta_index < state.metadata.size();
            size_t const sz = actual_meta
              ? state.metadata[state.meta_index].size()
              : 0;
            state.metatext = actual_meta
              ? state.metadata[state.meta_index].data()
              : nullptr;
            state.emptymeta = false;
            state.bits = 6;
            state.count = 0;
            state.bit_length = brcvt_MetaHeaderLen;
            state.backward = static_cast<uint32>(sz);
            if (sz > 65536)
              state.bits |= 48;
            else if (sz > 256)
              state.bits |= 32;
            else if (sz > 0)
              state.bits |= 16;
          }
          if (state.count < brcvt_MetaHeaderLen) {
            x = (state.bits>>state.count)&1u;
            state.count += 1u;
          }
          if (state.count >= brcvt_MetaHeaderLen) {
            if (state.backward)
              state.state = BrCvt_MetaLength;
            else if (i == 7)
              brcvt_next_block(state);
            else
              state.state = BrCvt_MetaText;
            state.bit_length = 0;
          }
          break;
        case BrCvt_MetaLength:
          if (state.bit_length == 0u) {
            state.bit_length = (state.bits>>5)*8;
            state.backward -= 1;
            state.count = 0;
          }
          if (state.count < state.bit_length) {
            x = (state.backward>>state.count)&1;
            state.count += 1;
          }
          if (state.count >= state.bit_length) {
            state.state = BrCvt_MetaText;
            state.backward += 1;
            state.count = 0;
          }
          break;
        case BrCvt_MetaText:
          x = 0;
          if (i == 7 && !state.backward)
              brcvt_next_block(state);
          break;
        case BrCvt_LastCheck:
          if (state.bit_length == 0u) {
            state.bit_length = 2;
            state.count = 0;
          }
          if (state.count < state.bit_length) {
            x = 1;
            state.count += 1;
          } else x = 0;
          if (state.count >= state.bit_length && i==7) {
            state.state = BrCvt_Done;
            state.bit_length = 0;
          }
          break;
        case BrCvt_Nibbles:
          if (state.bit_length == 0u) {
            size_t const input_len = state.buffer.input_size();
            if (input_len < state.buffer.capacity()
            &&  !(state.h_end&2u))
            {
              ae = api_error::Partial;
              break;
            }
            state.bit_length = 3;
            state.backward = static_cast<uint32>(input_len-1u);
            if (input_len > 1048576)
              state.bits = 4;
            else if (input_len > 65536)
              state.bits = 2;
            else if (input_len > 0)
              state.bits = 0;
            else if (state.h_end&2u) { /* convert to end block */
              state.bits = 3;
              state.h_end |= 1u;
              state.state = BrCvt_LastCheck;
              state.bit_length = 2;
            }else { /* convert to metadata block*/
              state.bits = 6;
              state.state = BrCvt_MetaStart;
              state.bit_length = brcvt_MetaHeaderLen;
              state.backward = 0;
            }
            state.count = 0;
          }
          if (state.count < state.bit_length) {
            x = (state.bits>>state.count)&1u;
            state.count += 1;
          }
          if (state.count >= state.bit_length) {
            state.state = BrCvt_InputLength; /* TODO */
            state.bit_length = (((state.bits>>1)&3u)|4u)<<2;
            state.count = 0;
          }
          break;
        case BrCvt_InputLength:
          if (state.count < state.bit_length) {
            x = (state.backward>>state.count)&1u;
            state.count += 1;
          }
          if (state.count >= state.bit_length) {
            state.state = BrCvt_CompressCheck; /* TODO */
            state.bit_length = 0;
            state.count = 0;
          }
          break;
        case BrCvt_CompressCheck:
          if (state.bit_length == 0) {
            bool const want_compress = (brcvt_check_compress(state)==api_error::Success);
            if (!want_compress) {
              x = 1;
              state.buffer.clear_output();
              state.buffer.noconv_block(ae);
              if (ae != api_error::Success)
                break;
              state.state = BrCvt_Uncompress;
              state.backward = state.buffer.str().size();
              state.count = 0;
              assert(state.backward);
              break;
            }
            state.bit_length = 1;
            x = 0;
            state.state = BrCvt_BlockTypesL;
          } break;
        case BrCvt_BlockTypesL:
          if (state.bit_length == 0) {
            state.count = 0;
            switch (state.literal_blocktype.size()) {
            case 0:
            case 1:
              state.bit_length = 1;
              state.bits = 0;
              break;
            case 2:
              state.bit_length = 4;
              state.bits = 1;
              break;
            case 3:
            case 4:
              state.bit_length = 5;
              state.bits = 3 | ((state.literal_blocktype.size()-3)<<4);
              break;
            default:
              ae = api_error::Sanitize;
              break;
            }
          }
          if (state.count < state.bit_length) {
            x = (state.bits>>state.count)&1u;
            state.count += 1;
          }
          if (state.count >= state.bit_length) {
            state.bit_length = 0;
            if (state.literal_blocktype.size() > 1)
              state.state += 1;
            else
              state.state += 4;
          } break;
        case BrCvt_BlockTypesLAlpha:
          {
            api_error const res = brcvt_outflow19(state.treety, state.literal_blocktype, x,
              state.alphabits);
            if (res == api_error::EndOfFile) {
              state.bit_length = 0;
              brcvt_reset19(state.treety);
              state.state += 1;
            } else if (res != api_error::Success)
              ae = res;
          } break;
        case BrCvt_BlockCountLAlpha:
          if (state.bit_length == 0) {
            state.bit_length += 1;
            prefix_histogram histogram;
            ae = util_move_make(histogram, 26);
            if (ae != api_error::Success)
              break;
            std::fill(histogram.begin(), histogram.end(), 0);
            block_string const& data = state.buffer.str();
            size_t const output_len = data.size();
            size_t i;
            size_t guess_i = 0;
            size_t total = 0;
            size_t literals = 0;
            size_t coverage = 0;
            // Reparse the text.
            for (std::size_t i = 0; i < output_len; ++i) {
              int const copy = (data[i]&128u);
              unsigned len = data[i]&63u;
              if (data[i] & 64u && i+1 < output_len) {
                i += 1;
                len = (len<<8) | data[i];
              }
              if (copy && i+i < output_len) {
                unsigned extend = 0;
                i += 1;
                if (data[i] < 128)
                  extend = 3;
                else if (data[i] < 192)
                  extend = 2;
                else extend = 4;
                if (extend >= output_len - i)
                  break;
                i += (extend-1);
              } else if (len > output_len - i) {
                ae = api_error::Sanitize;
                break;
              } else i += len;
              for (; guess_i < state.guesses.count; ++guess_i) {
                size_t const limit = (guess_i >= state.guesses.count-1)
                  ? state.guesses.total_bytes : state.guesses.offsets[guess_i+1];
                size_t const remaining = total - limit;
                if (len < remaining) {
                  literals += len;
                  break;
                }
                len -= remaining;
                literals += remaining;
                state.guess_lengths[guess_i] = literals;
                literals = 0;
              }
              total += len;
            }
            if (ae != api_error::Success)
              break;
            assert(total == output_len);
            // Populate histogram.
            inscopy_lengthsort(state.blockcounts);
            for (std::size_t i = 0; i < state.guesses.count; ++i) {
              size_t const v = inscopy_encode(state.blockcounts, state.guess_lengths[i], 0, 0);
              if (v >= 26) {
                ae = api_error::Sanitize;
                break;
              }
              histogram[v] += 1;
            }
            if (ae != api_error::Success)
              break;
            ae = util_move_make(state.literal_blockcount, 26);
            if (ae != api_error::Success)
              break;
            for (unsigned i = 0; i < 26; ++i) {
              state.literal_blockcount[i].value = i;
              coverage += (histogram[i] != 0);
            }
            fixlist_gen_lengths(state.literal_blockcount, histogram, 15, ae);
            if (ae != api_error::Success)
              break;
            fixlist_gen_codes(state.literal_blockcount, ae);
            if (ae != api_error::Success)
              break;
          }
          /* render tree to output */
          {
            api_error const res = brcvt_outflow19(state.treety, state.literal_blockcount, x, 5);
            if (res == api_error::EndOfFile) {
              state.bit_length = 0;
              brcvt_reset19(state.treety);
              state.state += 1;
              fixlist_valuesort(state.literal_blockcount, ae);
            } else if (res != api_error::Success)
              ae = res;
          } break;
        case BrCvt_BlockStartL:
          if (state.bit_length == 0) {
            std::size_t const code_index =
              inscopy_encode(state.blockcounts, state.guess_lengths[0], 0,0);
            if (code_index >= state.blockcounts.size()) {
              ae = api_error::Sanitize;
              break;
            }
            insert_copy_row const& row = state.blockcounts[code_index];
            std::size_t const value_index =
              fixlist_valuebsearch(state.literal_blockcount, row.code);
            if (value_index >= state.literal_blockcount.size()) {
              ae = api_error::Sanitize;
              break;
            }
            prefix_line const& line = state.literal_blockcount[value_index];
            state.count = (state.guess_lengths[0] - row.insert_first);
            state.extra_length = row.insert_bits;
            state.bits = line.code;
            state.bit_cap = line.len;
          }
          if (state.bit_length < state.bit_cap) {
            x = (state.bits >> (state.bit_cap - state.bit_length - 1u))&1u;
          } else {
            x = (state.count >> (state.bit_length - state.bit_cap)) & 1u;
          }
          state.bit_length += 1;
          if (state.bit_length >= state.bit_cap + state.extra_length) {
            state.state += 1;
            state.bit_length = 0;
          } break;
        case BrCvt_BlockTypesI:
          // hardcode single insert-and-copy block type
          x = 0;
          state.state = BrCvt_BlockTypesD;
          state.bit_length = 0;
          break;
        case BrCvt_BlockTypesD:
          // hardcode single distance block type
          x = 0;
          state.state = BrCvt_NPostfix;
          state.bit_length = 0;
          break;
        case BrCvt_NPostfix:
          if (state.bit_length == 0) {
            unsigned const postfix = state.ring.get_postfix();
            unsigned const direct = state.ring.get_direct();
            if (postfix > 3 || (direct & ((1u<<postfix)-1u))) {
              ae = api_error::Sanitize;
              break;
            }
            state.bits = postfix|(direct>>postfix);
          }
          if (state.bit_length < 6) {
            x = (state.bits>>state.bit_length)&1u;
            state.bit_length += 1;
          }
          if (state.bit_length >= 6) {
            state.state = BrCvt_ContextTypesL;
            state.bit_length = 0;
            state.count = 0;
          } break;
        case BrCvt_ContextTypesL:
          if (state.bit_length == 0) {
            size_t const contexts = state.literal_blocktype.size();
            for (size_t i = 0; i < contexts; ++i) {
              prefix_line const& line = state.literal_blocktype[i];
              state.bits |= ((line.value&3u)<<state.bit_length);
              state.bit_length += 2;
            }
          }
          if (state.count < state.bit_length) {
            x = (state.bits>>state.count)&1u;
            state.count += 1;
          }
          if (state.count >= state.bit_length) {
            state.state = BrCvt_TreeCountL;
            state.bit_length = 0;
            state.count = 0;
          } break;
        case BrCvt_TreeCountL:
          if (state.bit_length == 0) {
            size_t const btypes = state.literal_blocktype.size();
            state.count = 0;
            switch (btypes) {
            case 1:
              state.bits = 0;
              state.bit_length = 1;
              break;
            case 2:
              state.bits = 1;
              state.bit_length = 4;
              break;
            default:
              state.bits = 3|(((btypes&1u)^1u)<<4);
              state.bit_length = 5;
              break;
            }
          }
          if (state.count < state.bit_length) {
            x = (state.bits>>state.count)&1u;
            state.count += 1;
          }
          if (state.count >= state.bit_length) {
            state.state = (state.count == 1) ? BrCvt_TreeCountD : BrCvt_ContextRunMaxL;
            state.bit_length = 0;
          } break;
        case BrCvt_ContextRunMaxL:
          if (state.bit_length == 0) {
            context_map& map = state.literals_map;
            size_t const total = map.block_types() * map.contexts();
            size_t zeroes = 0;
            unsigned int rlemax = 0;
            ctxtmap_apply_movetofront(map);
            unsigned char const* const map_data = map.data();
            state.context_encode.clear();
            for (size_t j = 0; j < total && ae == api_error::Success; ++j) {
              if (map_data[j] || zeroes >= 63) {
                ae = brcvt_encode_map(state.context_encode, zeroes, map_data[j], rlemax);
                zeroes = (map_data[j]==0);
              } else zeroes += 1;
            }
            if (zeroes > 0 && ae == api_error::Success)
              ae = brcvt_encode_map(state.context_encode, zeroes, 0, rlemax);
#ifndef NDEBUG
            ctxtmap_revert_movetofront(map);
#endif //NDEBUG
            if (ae != api_error::Success)
              break;
            state.rlemax = static_cast<unsigned char>(rlemax);
            state.count = 0;
            if (state.rlemax == 0) {
              state.bits = 0;
              state.bit_length = 1;
            } else {
              state.bits = 1u | ((rlemax-1)<<1u);
              state.bit_length = 5;
            }
          }
          if (state.count < state.bit_length) {
            x = (state.bits>>state.count)&1u;
            state.count += 1;
          }
          if (state.count >= state.bit_length) {
            std::size_t const btypes = state.literal_blocktype.size();
            constexpr unsigned HistogramSize = 10;
            prefix_histogram histogram(HistogramSize);
            unsigned int const rlemax = state.rlemax;
            unsigned char const alphabits = static_cast<unsigned char>(rlemax+btypes);
            /* calculate prefix tree */
            try {
              state.context_tree = prefix_list(alphabits);
            } catch (std::bad_alloc const&) {
              ae = api_error::Memory;
              break;
            }
            for (std::size_t j = 0; j < state.context_encode.size(); ++j) {
              unsigned char const ch = state.context_encode[j];
              if (ch < brcvt_ZeroBit) {
                if (ch == 0)
                  histogram[0] += 1;
                else {
                  assert(ch+rlemax < HistogramSize);
                  histogram[ch+rlemax] += 1;
                }
              } else if (!(ch & brcvt_RepeatBit)) {
                histogram[ch&(brcvt_ZeroBit-1)] += 1;
              } else continue;
            }
            fixlist_gen_lengths(state.context_tree, histogram, 8, ae);
            if (ae != api_error::Success)
              break;
            fixlist_gen_codes(state.context_tree, ae);
            if (ae != api_error::Success)
              break;
            brcvt_reset19(state.treety);
            state.alphabits = alphabits;
            state.state += 1;
          } break;
        case BrCvt_ContextPrefixL:
          {
            api_error const res = brcvt_outflow19(state.treety, state.context_tree, x, state.alphabits);
            if (res == api_error::EndOfFile) {
              state.bit_length = 0;
              state.index = 0;
              state.count = static_cast<uint32>(state.context_encode.size());
              brcvt_reset19(state.treety);
              state.state += 1;
              fixlist_valuesort(state.context_tree, ae);
            } else if (res != api_error::Success)
              ae = res;
          } break;
        case BrCvt_ContextValuesL:
          if (state.bit_length == 0) {
            unsigned char const code = state.context_encode[state.index];
            unsigned int const extra = (code&brcvt_ZeroBit)
              ? code & (brcvt_ZeroBit-1u) : 0;
            unsigned int const value = extra ? extra : (code?code+state.rlemax:0);
            std::size_t const line_index = fixlist_valuebsearch(state.context_tree, value);
            if (line_index >= state.context_tree.size()) {
              ae = api_error::Sanitize;
              break;
            }
            prefix_line const& line = state.context_tree[line_index];
            state.bits = line.code;
            state.bit_length = static_cast<unsigned char>(line.len);
            state.extra_length = extra;
          }
          if (state.bit_length > 0)
            x = (state.bits>>(--state.bit_length))&1u;
          if (state.bit_length == 0) {
            state.index += 1;
            state.bits = 0;
            if (state.index >= state.count)
              state.state += 2;
            else if (state.extra_length > 0)
              state.state += 1;
          } break;
        case BrCvt_ContextRepeatL:
          if (state.bit_length == 0) {
            assert(state.index < state.context_encode.size());
            state.bits = state.context_encode[state.index];
          }
          if (state.bit_length < state.extra_length) {
            x = (state.bits>>state.bit_length)&1u;
            state.bit_length += 1;
          }
          if (state.bit_length >= state.extra_length) {
            state.index += 1;
            state.bits = 0;
            state.bit_length = 0;
            if (state.index >= state.count)
              state.state += 1;
            else
              state.state -= 1;
          } break;
        case BrCvt_ContextInvertL:
          x = 1;
          state.state = BrCvt_TreeCountD;
          break;
        case BrCvt_TreeCountD: // hard-code single distance tree
          x = 0;
          state.state = BrCvt_GaspVectorL;
          break;
        case BrCvt_GaspVectorL:
          /* TODO this state */
          break;
        case BrCvt_ContextRunMaxD:
        case BrCvt_ContextPrefixD:
        case BrCvt_ContextValuesD:
        case BrCvt_ContextRepeatD:
        case BrCvt_ContextInvertD:
          ae = api_error::Sanitize;
          break;
        case BrCvt_Uncompress:
          x = 0;
          break;
        case BrCvt_Done: /* end of stream */
          x = 0;
          ae = api_error::EndOfFile;
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
    //END   brcvt / static

    //BEGIN brcvt_state / rule-of-zero
    brcvt_state::brcvt_state(uint32 block_size, uint32 n, size_t chain_length)
      : buffer(std::min<uint32>(block_size,16777200u), n, chain_length, false),
        literals(288u), distances(32u), sequence(19u),
        wbits(15u), values(286u),
        ring(false,4,0), try_ring(false,4,0),
        lit_histogram{{256u}, {256u}, {256u}, {256u}}, dist_histogram(68u), ins_histogram(704u),
        bits(0u), bit_length(0u), state(0u), bit_index(0u),
        backward(0u), count(0u),
        wbits_select(0u), emptymeta(false), write_scratch(0), checksum(0u),
        bit_cap(0u), meta_index(0), metatext(nullptr), max_len_meta(1024),
        treety{}, guesses{},
        blocktypeL_index(0), blocktypeL_max(0),
        blocktypeI_index(0), blocktypeI_max(0),
        blocktypeD_index(0), blocktypeD_max(0),
        rlemax(0),
        guess_lengths{},
        blocktypeL_remaining(0),blocktypeI_remaining(0),
        blocktypeL_skip(brcvt_NoSkip), blockcountL_skip(brcvt_NoSkip),
        blocktypeI_skip(brcvt_NoSkip), blockcountI_skip(brcvt_NoSkip),
        blocktypeD_skip(brcvt_NoSkip), blockcountD_skip(brcvt_NoSkip),
        literal_skip(brcvt_NoSkip), insert_skip(brcvt_NoSkip), distance_skip(brcvt_NoSkip),
        context_skip(brcvt_NoSkip)
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
      inscopy_preset(blockcounts, insert_copy_preset::BrotliBlock);
      inscopy_codesort(blockcounts);
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
        case BrCvt_WBits: /* WBITS */
        case BrCvt_LastCheck:
        case BrCvt_MetaStart:
        case BrCvt_MetaLength:
        case BrCvt_CompressCheck:
        case BrCvt_InputLength:
        case BrCvt_BlockTypesL:
        case BrCvt_BlockTypesLAlpha:
        case BrCvt_BlockCountLAlpha:
        case BrCvt_BlockStartL:
        case BrCvt_BlockTypesI:
        case BrCvt_BlockTypesIAlpha:
        case BrCvt_BlockCountIAlpha:
        case BrCvt_BlockStartI:
        case BrCvt_BlockTypesD:
        case BrCvt_BlockTypesDAlpha:
        case BrCvt_BlockCountDAlpha:
        case BrCvt_BlockStartD:
        case BrCvt_NPostfix:
        case BrCvt_ContextTypesL:
        case BrCvt_TreeCountL:
        case BrCvt_ContextRunMaxL:
        case BrCvt_ContextPrefixL:
        case BrCvt_ContextValuesL:
        case BrCvt_ContextRepeatL:
        case BrCvt_ContextInvertL:
        case BrCvt_TreeCountD:
        case BrCvt_ContextRunMaxD:
        case BrCvt_ContextPrefixD:
        case BrCvt_ContextValuesD:
        case BrCvt_ContextRepeatD:
        case BrCvt_ContextInvertD:
        case BrCvt_GaspVectorL:
        case BrCvt_GaspVectorI:
        case BrCvt_GaspVectorD:
        case BrCvt_DataInsertCopy:
          ae = brcvt_in_bits(state, (*p), to, to_end, to_out);
          break;
        case BrCvt_MetaText:
          if (state.count == 0) {
            /* allocate */
            size_t const use_backward = (state.backward >= state.max_len_meta)
              ? state.max_len_meta : state.backward;
            state.meta_index = state.metadata.size();
            state.metadata.emplace(use_backward, ae);
            if (ae != api_error::Success)
              break;
            state.metatext = state.metadata[state.meta_index].data();
            state.index = state.metadata[state.meta_index].size();
          }
          if (state.count < state.backward) {
            if (state.count < state.index)
              state.metatext[state.count] = (*p);
            state.count += 1;
          }
          if (state.count >= state.backward) {
            state.metatext = nullptr;
            state.state = (state.h_end
              ? BrCvt_Done : BrCvt_LastCheck);
            if (state.h_end)
              ae = api_error::EndOfFile;
          }
          break;
        case BrCvt_Uncompress:
          if (state.count < state.backward) {
            if (!state.buffer.bypass(p, 1))
              ae = api_error::Memory;
            (*to_out) = (*p);
            to_out += 1;
            state.count += 1;
          }
          if (state.count >= state.backward) {
            state.metatext = nullptr;
            state.state = (state.h_end
              ? BrCvt_Done : BrCvt_LastCheck);
            if (state.h_end)
              ae = api_error::EndOfFile;
          } break;
        case 7:
          ae = api_error::EndOfFile;
          break;
        case 10: /* backward */
        case 11: /* distance bits */
        case 12: /* output from backward */
        case 13: /* hcounts */
        case 14: /* code lengths code lengths */
        case 15: /* literals and distances */
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
        case BrCvt_WBits: /* WBITS */
        case BrCvt_MetaStart:
        case BrCvt_MetaLength:
        case BrCvt_LastCheck:
        case BrCvt_Nibbles:
        case BrCvt_InputLength:
        case BrCvt_CompressCheck:
        case BrCvt_BlockTypesL:
        case BrCvt_BlockTypesLAlpha:
        case BrCvt_BlockCountLAlpha:
        case BrCvt_BlockStartL:
        case BrCvt_BlockTypesI:
        case BrCvt_BlockTypesIAlpha:
        case BrCvt_BlockCountIAlpha:
        case BrCvt_BlockStartI:
        case BrCvt_BlockTypesD:
        case BrCvt_BlockTypesDAlpha:
        case BrCvt_BlockCountDAlpha:
        case BrCvt_BlockStartD:
        case BrCvt_NPostfix:
        case BrCvt_ContextTypesL:
        case BrCvt_TreeCountL:
        case BrCvt_ContextRunMaxL:
        case BrCvt_ContextPrefixL:
        case BrCvt_ContextValuesL:
        case BrCvt_ContextRepeatL:
        case BrCvt_ContextInvertL:
        case BrCvt_TreeCountD:
        case BrCvt_ContextRunMaxD:
        case BrCvt_ContextPrefixD:
        case BrCvt_ContextValuesD:
        case BrCvt_ContextRepeatD:
        case BrCvt_ContextInvertD:
        case BrCvt_GaspVectorL:
        case BrCvt_GaspVectorI:
        case BrCvt_GaspVectorD:
        case BrCvt_DataInsertCopy:
          ae = brcvt_out_bits(state, from, from_end, p, *to_out);
          break;
        case BrCvt_MetaText:
          assert(state.metatext);
          if (state.count < state.backward)
            *to_out = state.metatext[state.count++];
          ae = api_error::Success;
          if (state.count >= state.backward) {
            state.metatext = NULL;
            brcvt_next_block(state);
            state.bit_length = 0;
          }
          break;
        case BrCvt_Uncompress:
          if (state.count < state.backward)
            *to_out = state.buffer.str()[state.count++];
          ae = api_error::Success;
          if (state.count >= state.backward) {
            state.metatext = nullptr;
            state.buffer.clear_input();
            brcvt_next_block(state);
            state.bit_length = 0;
          } break;
        case BrCvt_Done:
          ae = api_error::EndOfFile;
          break;
        case 13: /* hcounts */
        case 14: /* code lengths code lengths */
        case 15: /* literals and distances */
          ae = brcvt_out_bits(state, from, from_end, p, *to_out);
          break;
        default:
          ae = api_error::Sanitize;
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

    api_error brcvt_flush(brcvt_state& state,
        unsigned char* to, unsigned char* to_end,
        unsigned char*& to_next)
    {
      unsigned char const tmp[1] = {0u};
      unsigned char const* tmp_next = &tmp[0];
      /* set the flush flag: */state.emptymeta = true;
      return brcvt_out(state, &tmp[0], &tmp[0], tmp_next, to, to_end, to_next);
    }
    //END   brcvt / namespace local
  };
};
