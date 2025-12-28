/**
 * @file text-complex-plus/access/zcvt.hpp
 * @brief zlib conversion state
 */
#ifndef hg_TextComplexAccessP_ZCvt_H_
#define hg_TextComplexAccessP_ZCvt_H_

#include "api.hpp"
#include "util.hpp"
#include "blockbuf.hpp"
#include "fixlist.hpp"
#include "inscopy.hpp"
#include "ringdist.hpp"

namespace text_complex {
  namespace access {
    /**
     * @defgroup zcvt zlib conversion state
     *   (access/zcvt.hpp)
     * @{
     */
    //BEGIN zcvt state
    /**
     * @brief zlib conversion state.
     */
    class TCMPLX_AP_API zcvt_state final {
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
      /** @brief Checksum value. */
      uint32 checksum;
      /** @brief Output internal bit count. */
      uint32 bit_cap;
      /** @brief Scratch space for last byte generated. */
      unsigned char write_scratch;

    public: /** @name rule-of-zero*//** @{ */
      /**
       * @brief Constructor.
       * @param block_size amount of input data to process at once
       * @param n maximum sliding window size
       * @param chain_length run-time parameter limiting hash chain length
       * @throw std::bad_alloc if something breaks
       */
      zcvt_state
        ( uint32 block_size = 4096u, uint32 n = 32768u,
          size_t chain_length = 256u);
      /** @} */

    public: /** @name allocation *//** @{ */
      /**
       * @brief Scalar memory allocator.
       * @param sz size in `char`s of `zcvt_state` to allocate
       * @return a pointer to memory on success
       * @throw std::bad_alloc on allocation error
       */
      static void* operator new(std::size_t sz);
      /**
       * @brief Array memory allocator.
       * @param sz size in `char`s of `zcvt_state[]` to allocate
       * @return a pointer to memory on success
       * @throw std::bad_alloc on allocation error
       */
      static void* operator new[](std::size_t sz);
      /**
       * @brief Scalar memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `zcvt_state` to free
       */
      static void operator delete(void* p, std::size_t sz) noexcept;
      /**
       * @brief Array memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `zcvt_state[]` to free
       */
      static void operator delete[](void* p, std::size_t sz) noexcept;
      /** @} */

    public: /** @name methods *//** @{ */
      /** @} */
    };
    //END   zcvt state

    //BEGIN zcvt state / exports
    template
    class TCMPLX_AP_API util_unique_ptr<zcvt_state>;
    //END   zcvt state / exports

    //BEGIN zcvt state / allocation (namespace local)
    /**
     * @brief Non-throwing zcvt state allocator.
     * @param block_size amount of input data to process at once
     * @param n maximum sliding window size
     * @param chain_length run-time parameter limiting hash chain length
     * @return a zcvt state on success, `nullptr` otherwise
     */
    TCMPLX_AP_API
    zcvt_state* zcvt_new
        ( uint32 block_size = 4096u, uint32 n = 32768u,
          size_t chain_length = 256u) noexcept;

    /**
     * @brief Non-throwing zcvt state allocator.
     * @param block_size amount of input data to process at once
     * @param n maximum sliding window size
     * @param chain_length run-time parameter limiting hash chain length
     * @return a zcvt state on success, `nullptr` otherwise
     */
    TCMPLX_AP_API
    util_unique_ptr<zcvt_state> zcvt_unique
        ( uint32 block_size = 4096u, uint32 n = 32768u,
          size_t chain_length = 256u) noexcept;

    /**
     * @brief Destroy a zcvt state.
     * @param x (nullable) the zcvt state to destroy
     */
    TCMPLX_AP_API
    void zcvt_destroy(zcvt_state* x) noexcept;
    //END   zcvt state / allocation (namespace local)

    //BEGIN zcvt state / namespace local
    /**
     * @brief Convert a zlib stream to a byte stream.
     * @param state the zlib conversion state to use
     * @param from source buffer
     * @param from_end pointer to end of source buffer
     * @param[out] from_next location of next byte to process
     * @param to destination buffer
     * @param to_end pointer to end of destination buffer
     * @param[out] to_next location of next output byte
     * @return api_error::Success on success, nonzero otherwise
     * @note The conversion state referred to by `state` is updated based
     *   on the conversion result, whether succesful or failed.
     *
     * @note This function returns api_error::ZDictionary if the input
     *   stream expects a ZLIB dictionary. If the caller expects to
     *   supply a dictionary, use @link zcvt_bypass @endlink
     *   to add the dictionary corresponding to the state's `checksum` field.
     * @see @link zcvt_state#checksum @endlink
     */
    TCMPLX_AP_API
    api_error zcvt_in(zcvt_state& state,
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
    size_t zcvt_bypass
      ( zcvt_state& state, unsigned char const* buf, size_t sz,
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
    size_t zcvt_bypass
      (zcvt_state& state, unsigned char const* buf, size_t sz);

    /**
     * @brief Convert a byte stream to a zlib stream.
     * @param state the zlib conversion state to use
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
    api_error zcvt_out(zcvt_state& state,
        unsigned char const* from, unsigned char const* from_end,
        unsigned char const*& from_next,
        unsigned char* to, unsigned char* to_end,
        unsigned char*& to_next);

    /**
     * @brief Convert a byte stream to a zlib stream.
     * @param state the zlib conversion state to use
     * @param to destination buffer
     * @param to_end pointer to end of destination buffer
     * @param[out] to_next location of next output byte,
     *   api_error::EndOfFile at end of stream, other nonzero otherwise
     * @note The conversion state referred to by `state` is updated based
     *   on the conversion result, whether succesful or failed.
     *
     * @note Any bytes remaining in the conversion state will
     *   be processed before outputting the delimiter (Adler32 checksum).
     */
    TCMPLX_AP_API
    api_error zcvt_unshift(zcvt_state& state,
        unsigned char* to, unsigned char* to_end,
        unsigned char*& to_next);
    //END   zcvt state / namespace local
    /** @} */
  };
};

#include "zcvt.txx"

#endif //hg_TextComplexAccessP_ZCvt_H_
