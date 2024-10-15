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
      /**
       * @brief ...
       * @note Using a buffer with a slide ring extent greater than 32768
       *   can cause output sanity check to fail.
       */
      block_buffer buffer;
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
      /** @brief ... */
      distance_ring ring;
      /** @brief Check for large blocks. */
      distance_ring try_ring;
      /** @brief Check for large blocks. */
      prefix_histogram lit_histogram;
      /** @brief Check for large blocks. */
      prefix_histogram dist_histogram;
      /** @brief Check for large blocks. */
      prefix_histogram seq_histogram;
      /** @brief Tree description sequence. */
      block_string sequence_list;
      /** @brief ... */
      unsigned short int bits;
      /** @brief Read count for bits used after a Huffman code. */
      unsigned short int extra_length;
      /** @brief End indicator. */
      unsigned char h_end;
      /** @brief ... */
      unsigned char bit_length;
      /** @brief ... */
      unsigned char state;
      /** @brief Bit position in the stream. */
      unsigned char bit_index;
      /** @brief Backward distance value. */
      uint32 backward;
      /** @brief Byte count for the active state. */
      uint32 count;
      /** @brief Byte index for the active state. */
      uint32 index;
      /** @brief Which value to use for WBITS. */
      unsigned char wbits_select;
      /** @brief Whether to insert an empty metadata block. */
      bool emptymeta;
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


    public: /** @name rule-of-zero*//** @{ */
      /**
       * @brief Constructor.
       * @param block_size amount of input data to process at once
       * @param n maximum sliding window size
       * @param chain_length run-time parameter limiting hash chain length
       * @throw std::bad_alloc if something breaks
       */
      brcvt_state
        ( uint32 block_size = 4096u, uint32 n = 32768u,
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
        ( uint32 block_size = 4096u, uint32 n = 32768u,
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
        ( uint32 block_size = 4096u, uint32 n = 32768u,
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
     * @return api_error::Success on success, nonzero otherwise
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
