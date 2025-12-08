/**
 * @file text-complex-plus/access/brcvt.hpp
 * @brief Brotli conversion state
 */
#ifndef hg_TextComplexAccessP_BrCvt_H_
#define hg_TextComplexAccessP_BrCvt_H_

#include "api.hpp"
#include "util.hpp"
#include "blockbuf.hpp"
#include "fixlist.hpp"
#include "inscopy.hpp"
#include "ringdist.hpp"
#include "brmeta.hpp"
#include "ctxtspan.hpp"
#include "gaspvec.hpp"

namespace text_complex {
  namespace access {
    /**
     * @defgroup brcvt zlib conversion state
     *   (access/brcvt.hpp)
     * @{
     */
    //BEGIN brcvt state
    /**
     * @brief zlib conversion state.
     */
    class TCMPLX_AP_API brcvt_state final {
    public:
      /** @brief Prefix code tree marshal. */
      struct treety_box {
        /** @brief Number of symbols in the tree. */
        unsigned short count;
        /** @brief Index of current symbol. */
        unsigned short index;
        /** @brief Partial bit sequence. */
        unsigned short bits;
        /** @brief Machine state. */
        unsigned char state;
        /** @brief Number of bits captured or remaining. */
        unsigned char bit_length;
        /** @brief Code length check. */
        unsigned short len_check;
        /** @brief Count of nonzero code lengths. */
        unsigned char nonzero;
        /** @brief Most recently used code length. */
        unsigned char last_len;
        /** @brief The only code length usable. */
        unsigned char singular;
        /** @brief Previous nonzero code length. */
        unsigned char last_nonzero;
        /** @brief Last repeat count. */
        unsigned short last_repeat;
        /** @brief Internal prefix code. */
        prefix_list nineteen;
        /** @brief Tree description sequence. */
        block_string sequence_list;
      };
      /**
       * @brief Demand-driven output bit negotiator.
       * @note This structure allows meta-blocks to emit zero bytes.
       */
      struct forward_box {
        std::size_t i;
        uint32 literal_i;
        uint32 literal_total;
        /**
         * @note Used as temporary storage of distance code during
         *   inflow of Distance and DataDistanceExtra.
         */
        uint32 pos;
        /**
         * @note Used as temporary storage of copy length during
         *   inflow of Literal and LiteralRestart.
         */
        uint32 stop;
        uint32 accum;
        unsigned short command_span;
        unsigned char ostate;
        /**
         * @note Used as temporary storage of zero distance flag.
         */
        unsigned char ctxt_i;
        unsigned char bstore[38];
        unsigned char literal_ctxt[2];
      };
      /**
       * @brief Block type history tracker.
       */
      struct block_box {
        unsigned char current;
        unsigned char previous;
      };
      /**
       * @brief ...
       * @note Using a buffer with a slide ring extent greater than 32768
       *   can cause output sanity check to fail.
       */
      block_buffer buffer;
      /** @brief Block type prefix code for literals. */
      prefix_list literal_blocktype;
      /** @brief Block count prefix code for literals. */
      prefix_list literal_blockcount;
      /** @brief Block type prefix code for insert-and-copy. */
      prefix_list insert_blocktype;
      /** @brief Block count prefix code for insert-and-copy. */
      prefix_list insert_blockcount;
      /** @brief Block type prefix code for distances. */
      prefix_list distance_blocktype;
      /** @brief Block count prefix code for distances. */
      prefix_list distance_blockcount;
      /** @brief Context transcode prefixes. */
      prefix_list context_tree;
      /** @brief ... */
      prefix_list literals;
      /** @brief ... */
      prefix_list distances;
      /** @brief ... */
      prefix_list sequence;
      /** @brief Fixed window size Huffman code table. */
      prefix_list wbits;
      /** @brief ... */
      insert_copy_table values;
      /** @brief Block count insert-copy table */
      insert_copy_table blockcounts;
      /** @brief ... */
      distance_ring ring;
      /** @brief Check for large blocks. */
      distance_ring try_ring;
      /** @brief Context map for dancing through the literals' Huffman forest. */
      context_map literals_map;
      /** @brief The literals' Huffman forest. */
      gasp_vector literals_forest;
      /** @brief Context map for dancing through the distances' Huffman forest. */
      context_map distance_map;
      /** @brief The distances' Huffman forest. */
      gasp_vector distance_forest;
      /** @brief The Huffman forest for insert-and-copy. */
      gasp_vector insert_forest;
      /** @brief Histogram used for generating literal trees. */
      prefix_histogram lit_histogram[4];
      /** @brief Histogram used for generating distance trees. */
      prefix_histogram dist_histogram;
      /** @brief Histogram used for generating insert-and-copy trees. */
      prefix_histogram ins_histogram;
      /** @brief ... */
      uint32 bits;
      /** @brief Read count for bits used after a Huffman code. */
      unsigned short int extra_length;
      /**
       * @brief End indicator.
       * @note AND 2 = requested; AND 1 = granted.
       */
      unsigned char h_end;
      /** @brief ... */
      unsigned char bit_length;
      /** @brief ... */
      unsigned char state;
      /** @brief Bit position in the stream. */
      unsigned char bit_index;
      /** @brief Backward distance value. */
      uint32 backward;
      /** @brief Byte count for the current meta block. */
      uint32 metablock_pos;
      /** @brief Byte count for the active state. */
      uint32 count;
      /** @brief Byte index for the active state. */
      uint32 index;
      /** @brief Which value to use for WBITS. */
      unsigned char wbits_select;
      /** @brief Whether to insert an empty metadata block. */
      bool emptymeta;
      /** @brief Size of alphabet used by the treety. */
      unsigned char alphabits;
      /** @brief Scratch space for last byte generated. */
      unsigned char write_scratch;
      /** @brief Checksum value. */
      [[deprecated]]
      unsigned short checksum;
      /** @brief Output internal bit count. */
      uint32 bit_cap;
      /** @brief Nonzero metadata block storage. */
      brotli_meta metadata;
      /** @brief Number of metadata posted so far. */
      std::size_t meta_index;
      /** @brief Text of current metadata to post. */
      unsigned char* metatext;
      /** @brief Maximum metadatum length to store aside. */
      std::size_t max_len_meta;
      /** @brief Prefix code tree marshal. */
      treety_box treety;
      /** @brief Context span guess for outflow. */
      context_span guesses;
      /** @brief Context mode offset for outflow. */
      unsigned char guess_offset;
      /** @brief Current literal block type. */
      block_box blocktypeL_index;
      /** @brief Maximum literal block type. */
      unsigned char blocktypeL_max;
      /** @brief Current insert-and-copy block type. */
      block_box blocktypeI_index;
      /** @brief Maximum insert-and-copy block type. */
      unsigned char blocktypeI_max;
      /** @brief Current distance block type. */
      block_box blocktypeD_index;
      /** @brief Maximum distance block type. */
      unsigned char blocktypeD_max;
      /** @brief Field for context map transcoding. */
      unsigned char rlemax;
      /** @brief Field for context map encoding. */
      block_string context_encode;
      /** @brief Context span effective lengths for outflow. */
      uint32 guess_lengths[CtxtSpan_Size];
      /** @brief Remaining items under the current literal blocktype. */
      uint32 blocktypeL_remaining;
      /** @brief Remaining items under the current insert-and-copy blocktype. */
      uint32 blocktypeI_remaining;
      /** @brief Remaining items under the current distance blocktype. */
      uint32 blocktypeD_remaining;
      /** @brief Literal type skip code. */
      unsigned short blocktypeL_skip;
      /** @brief Literal count skip code. */
      unsigned short blockcountL_skip;
      /** @brief Insert-and-copy type skip code. */
      unsigned short blocktypeI_skip;
      /** @brief Insert-and-copy count skip code. */
      unsigned short blockcountI_skip;
      /** @brief Distance type skip code. */
      unsigned short blocktypeD_skip;
      /** @brief Distance count skip code. */
      unsigned short blockcountD_skip;
      /** @brief Skip code for literals. */
      unsigned short literal_skip;
      /** @brief Skip code for insert-and-copy. */
      unsigned short insert_skip;
      /** @brief Skip code for distances. */
      unsigned short distance_skip;
      /** @brief Context map prefix tree skip code. */
      unsigned short context_skip;
      /** @brief Token forwarding. */
      forward_box fwd;
      /** @brief Map from mode to outflow context index. */
      unsigned char ctxt_mode_map[4];
      /**
       * @brief Built-in tree type for block type outflow.
       * @todo Test for removal.
       */
      [[deprecated("Unused field; ignore.")]]
      unsigned char blocktype_simple;

    public: /** @name rule-of-zero*//** @{ */
      /**
       * @brief Constructor.
       * @param block_size amount of input data to process at once
       * @param n maximum sliding window size
       * @param chain_length run-time parameter limiting hash chain length
       * @throw std::bad_alloc if something breaks
       */
      brcvt_state
        ( uint32 block_size = 4096u, uint32 n = 16777200u,
          size_t chain_length = 256u);
      /** @} */

    public: /** @name allocation *//** @{ */
      /**
       * @brief Scalar memory allocator.
       * @param sz size in `char`s of `brcvt_state` to allocate
       * @return a pointer to memory on success
       * @throw std::bad_alloc on allocation error
       */
      static void* operator new(std::size_t sz);
      /**
       * @brief Array memory allocator.
       * @param sz size in `char`s of `brcvt_state[]` to allocate
       * @return a pointer to memory on success
       * @throw std::bad_alloc on allocation error
       */
      static void* operator new[](std::size_t sz);
      /**
       * @brief Scalar memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `brcvt_state` to free
       */
      static void operator delete(void* p, std::size_t sz) noexcept;
      /**
       * @brief Array memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `brcvt_state[]` to free
       */
      static void operator delete[](void* p, std::size_t sz) noexcept;
      /** @} */

    public:
      /**
       * @ingroup methods
       * @return a reference to the metadata array
       */
      brotli_meta& get_metadata() noexcept;
      /**
       * @ingroup methods
       * @return a reference to the metadata array
       */
      brotli_meta const& get_metadata() const noexcept;
    };
    //END   brcvt state

    //BEGIN brcvt state / exports
    template
    class TCMPLX_AP_API util_unique_ptr<brcvt_state>;
    //END   brcvt state / exports

    //BEGIN brcvt state / allocation (namespace local)
    /**
     * @brief Non-throwing brcvt state allocator.
     * @param block_size amount of input data to process at once
     * @param n maximum sliding window size
     * @param chain_length run-time parameter limiting hash chain length
     * @return a brcvt state on success, `nullptr` otherwise
     */
    TCMPLX_AP_API
    brcvt_state* brcvt_new
        ( uint32 block_size = 4096u, uint32 n = 16777200u,
          size_t chain_length = 256u) noexcept;

    /**
     * @brief Non-throwing brcvt state allocator.
     * @param block_size amount of input data to process at once
     * @param n maximum sliding window size
     * @param chain_length run-time parameter limiting hash chain length
     * @return a brcvt state on success, `nullptr` otherwise
     */
    TCMPLX_AP_API
    util_unique_ptr<brcvt_state> brcvt_unique
        ( uint32 block_size = 4096u, uint32 n = 16777200u,
          size_t chain_length = 256u) noexcept;

    /**
     * @brief Destroy a brcvt state.
     * @param x (nullable) the brcvt state to destroy
     */
    TCMPLX_AP_API
    void brcvt_destroy(brcvt_state* x) noexcept;
    //END   brcvt state / allocation (namespace local)

    //BEGIN brcvt state / namespace local
    /**
     * @brief Convert a Brotli stream to a byte stream.
     * @param state the Brotli conversion state to use
     * @param from source buffer
     * @param from_end pointer to end of source buffer
     * @param[out] from_next location of next byte to process
     * @param to destination buffer
     * @param to_end pointer to end of destination buffer
     * @param[out] to_next location of next output byte
     * @return api_error::Success on success, nonzero otherwise
     * @note The conversion state referred to by `state` is updated based
     *   on the conversion result, whether succesful or failed.
     * @see @link brcvt_state#checksum @endlink
     */
    TCMPLX_AP_API
    api_error brcvt_in(brcvt_state& state,
        unsigned char const* from, unsigned char const* from_end,
        unsigned char const*& from_next,
        unsigned char* to, unsigned char* to_end,
        unsigned char*& to_next);

    /**
     * @brief Add dictionary data outside of the input stream.
     * @param state the conversion state to configure
     * @param buf buffer of bytes to add
     * @param sz size of the buffer in bytes
     * @param[out] ae @em error-code api_error::Success on success,
     *   other value on failure
     * @return count of bytes added
     */
    TCMPLX_AP_API
    size_t brcvt_bypass
      ( brcvt_state& state, unsigned char const* buf, size_t sz,
        api_error& ae) noexcept;

    /**
     * @brief Add dictionary data outside of the input stream.
     * @param state the conversion state to configure
     * @param buf buffer of bytes to add
     * @param sz size of the buffer in bytes
     * @return count of bytes added
     * @throw api_exception on failure
     */
    TCMPLX_AP_API
    size_t brcvt_bypass
      (brcvt_state& state, unsigned char const* buf, size_t sz);

    /**
     * @brief Convert a byte stream to a Brotli stream.
     * @param state the Brotli conversion state to use
     * @param from source buffer
     * @param from_end pointer to end of source buffer
     * @param[out] from_next location of next byte to process
     * @param to destination buffer
     * @param to_end pointer to end of destination buffer
     * @param[out] to_next location of next output byte
     * @return api_error::Success on success, nonzero otherwise
     * @note The conversion state referred to by `state` is updated based
     *   on the conversion result, whether succesful or failed.
     */
    TCMPLX_AP_API
    api_error brcvt_out(brcvt_state& state,
        unsigned char const* from, unsigned char const* from_end,
        unsigned char const*& from_next,
        unsigned char* to, unsigned char* to_end,
        unsigned char*& to_next);

    /**
     * @brief Convert a byte stream to a Brotli stream.
     * @param state the Brotli conversion state to use
     * @param to destination buffer
     * @param to_end pointer to end of destination buffer
     * @param[out] to_next location of next output byte
     * @return api_error::Success on success,
     *   api_error::EndOfFile at end of stream, other nonzero otherwise
     * @note The conversion state referred to by `state` is updated based
     *   on the conversion result, whether succesful or failed.
     *
     * @note Any bytes remaining in the conversion state will
     *   be processed before outputting the delimiter (stream terminator bits).
     */
    TCMPLX_AP_API
    api_error brcvt_unshift(brcvt_state& state,
        unsigned char* to, unsigned char* to_end,
        unsigned char*& to_next);

    /**
     * @brief Flush a Brotli stream to a byte using an empty metadata block.
     * @param state the Brotli conversion state to use
     * @param to destination buffer
     * @param to_end pointer to end of destination buffer
     * @param[out] to_next location of next output byte
     * @return api_error::Success on success, nonzero otherwise
     * @note The conversion state referred to by `state` is updated based
     *   on the conversion result, whether successful or failed.
     *
     * @note Any bytes remaining in the conversion state will
     *   be processed before outputting the metadata block.
     */
    TCMPLX_AP_API
    api_error brcvt_flush(brcvt_state& state,
        unsigned char* to, unsigned char* to_end,
        unsigned char*& to_next);
    //END   brcvt state / namespace local
    /** @} */
  };
};

#include "brcvt.txx"

#endif //hg_TextComplexAccessP_BrCvt_H_
