/**
 * @file text-complex-plus/access/blockbuf.hpp
 * @brief DEFLATE block buffer
 */
#ifndef hg_TextComplexAccessP_BlockBuf_H_
#define hg_TextComplexAccessP_BlockBuf_H_

#include "api.hpp"
#include "util.hpp"

namespace text_complex {
  namespace access {
    class hash_chain;

    /**
     * @defgroup blockbuf DEFLATE block buffer
     *   (access/blockbuf.hpp)
     * @{
     */
    //BEGIN block string
    /**
     * @brief Byte storage used by the block buffer.
     */
    class TCMPLX_AP_API block_string final {
    private:
      unsigned char* p;
      uint32 sz;
      uint32 cap;

    public: /** @name string-compat *//** @{ */
      /** @brief Bytes stored in the block string. */
      typedef unsigned char value_type;
      /** @brief Numeric type represents capacity and size of the string. */
      typedef uint32 size_type;
      /** @} */

    public: /** @name rule-of-six *//** @{ */
      /**
       * @brief Constructor.
       */
      block_string(void) noexcept;
      /**
       * @brief Destructor.
       */
      ~block_string(void);
      /**
       * @brief Copy constructor.
       * @param s the string to copy
       * @throw std::bad_alloc on failure
       */
      block_string(block_string const& s);
      /**
       * @brief Copy assignment operator.
       * @param s the string to copy
       * @throw std::bad_alloc on failure
       */
      block_string& operator=(block_string const& s);
      /**
       * @brief Move constructor.
       * @param s the string to move
       */
      block_string(block_string &&s) noexcept;
      /**
       * @brief Move assignment operator.
       * @param s the string to move
       */
      block_string& operator=(block_string &&s) noexcept;
      /** @} */

    public: /** @name allocation *//** @{ */
      /**
       * @brief Scalar memory allocator.
       * @param sz size in `char`s of `block_string` to allocate
       * @return a pointer to memory on success
       * @throw std::bad_alloc on allocation error
       */
      static void* operator new(std::size_t sz);
      /**
       * @brief Array memory allocator.
       * @param sz size in `char`s of `block_string[]` to allocate
       * @return a pointer to memory on success
       * @throw std::bad_alloc on allocation error
       */
      static void* operator new[](std::size_t sz);
      /**
       * @brief Scalar memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `block_string` to free
       */
      static void operator delete(void* p, std::size_t sz) noexcept;
      /**
       * @brief Array memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `block_string[]` to free
       */
      static void operator delete[](void* p, std::size_t sz) noexcept;
      /** @} */

    public: /** @name string-compat *//** @{ */
      /**
       * @brief Reserve some capacity.
       * @param cap amount of capacity to reserve
       * @throw api_exception on failure
       */
      void reserve(uint32 cap);
      /**
       * @brief Reserve some capacity.
       * @param cap amount of capacity to reserve
       * @param[out] ae @em error-code api_error::Success on success,
       *   other value on failure
       */
      void reserve(uint32 cap, api_error &ae) noexcept;
      /**
       * @brief Check the capacity.
       * @return the capacity
       */
      uint32 capacity(void) const noexcept;
      /**
       * @brief Check the size.
       * @return the number of valid bytes in storage
       */
      uint32 size(void) const noexcept;
      /**
       * @brief Check the storage.
       * @return a pointer to the first byte of storage
       */
      unsigned char const* data(void) const noexcept;
      /**
       * @brief Access the storage.
       * @return a pointer to the first byte of storage
       */
      unsigned char* data(void) noexcept;
      /**
       * @brief Resize the storage to zero.
       */
      void clear(void) noexcept;
      /**
       * @brief Resize the storage.
       * @param sz desired size
       * @param ch fill byte
       * @throw api_exception on failure
       */
      void resize(uint32 sz, unsigned char ch);
      /**
       * @brief Resize the storage.
       * @param sz desired size
       * @param ch fill byte
       * @param[out] ae @em error-code api_error::Success on success,
       *   other value on failure
       */
      void resize(uint32 sz, unsigned char ch, api_error& ae) noexcept;
      /**
       * @brief Add a byte to the end of the storage.
       * @param ch byte to add
       * @throw api_exception on failure
       */
      void push_back(unsigned char ch);
      /**
       * @brief Add a byte to the end of the storage.
       * @param ch byte to add
       * @param[out] ae @em error-code api_error::Success on success,
       *   other value on failure
       */
      void push_back(unsigned char ch, api_error &ae) noexcept;
      /**
       * @brief String byte indexing.
       * @param i index
       * @return a reference to the byte at index `i`
       */
      unsigned char& operator[](uint32 i) noexcept;
      /**
       * @brief String byte indexing.
       * @param i index
       * @return a reference to the byte at index `i`
       */
      unsigned char const& operator[](uint32 i) const noexcept;
      /** @} */

    private: /** @name rule-of-six *//** @{ */
      void duplicate(block_string const&);
      void transfer(block_string&&) noexcept;
      /** @} */
    };
    //END   block string

    //BEGIN block buffer
    /**
     * @brief DEFLATE block buffer.
     */
    class TCMPLX_AP_API block_buffer final {
    private:
      hash_chain* chain;
      block_string input;
      block_string output;
      bool use_bdict;
      uint32 input_block_size;

    public: /** @name rule-of-six*//** @{ */
      /**
       * @brief Constructor.
       * @param block_size amount of input data to process at once
       * @param n maximum sliding window size
       * @param chain_length run-time parameter limiting hash chain length
       * @param use_bdict whether to use the built-in dictionary
       * @throw std::bad_alloc if something breaks
       */
      block_buffer(uint32 block_size, uint32 n, size_t chain_length,
          bool use_bdict);
      /**
       * @brief Destructor.
       */
      ~block_buffer(void);
      /**
       * @brief Copy constructor.
       */
      block_buffer(block_buffer const& ) = delete;
      /**
       * @brief Copy assignment operator.
       * @return this block buffer
       */
      block_buffer& operator=(block_buffer const& ) = delete;
      /**
       * @brief Move constructor.
       */
      block_buffer(block_buffer&& ) noexcept;
      /**
       * @brief Move assignment operator.
       * @return this block buffer
       */
      block_buffer& operator=(block_buffer&& ) noexcept;
      /** @} */

    public: /** @name allocation *//** @{ */
      /**
       * @brief Scalar memory allocator.
       * @param sz size in `char`s of `block_buffer` to allocate
       * @return a pointer to memory on success
       * @throw std::bad_alloc on allocation error
       */
      static void* operator new(std::size_t sz);
      /**
       * @brief Array memory allocator.
       * @param sz size in `char`s of `block_buffer[]` to allocate
       * @return a pointer to memory on success
       * @throw std::bad_alloc on allocation error
       */
      static void* operator new[](std::size_t sz);
      /**
       * @brief Scalar memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `block_buffer` to free
       */
      static void operator delete(void* p, std::size_t sz) noexcept;
      /**
       * @brief Array memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `block_buffer[]` to free
       */
      static void operator delete[](void* p, std::size_t sz) noexcept;
      /** @} */

    public: /** @name methods *//** @{ */
      /**
       * @brief Check how many input bytes await processing.
       * @return a byte count
       */
      uint32 input_size(void) const noexcept;
      /**
       * @brief Check how many input bytes can be processed at once.
       * @return a byte count
       */
      uint32 capacity(void) const noexcept;
      /** @} */

    public: /** @name ostringstream-compat *//** @{ */
      /**
       * @brief Process the current block of input bytes.
       * @return `*this`
       * @throw api_exception on failure
       * @note The input buffer is cleared on success.
       */
      block_buffer& flush(void);
      /**
       * @brief Process the current block of input bytes.
       * @param[out] ae @em error-code api_error::Success on success,
       *   other value on failure
       * @return `*this`
       * @note The input buffer is cleared on success.
       */
      block_buffer& flush(api_error& ae) noexcept;
      /**
       * @brief Add some input bytes.
       * @param s data to add to the input
       * @param count size of the added input
       * @return `*this`
       * @throw api_exception on failure
       * @note Either the full block is added, or none at all.
       */
      block_buffer& write
        (unsigned char const* s, size_t count);
      /**
       * @brief Add some input bytes.
       * @param s data to add to the input
       * @param count size of the added input
       * @param[out] ae @em error-code api_error::Success on success,
       *   other value on failure
       * @return `*this`
       * @note Either the full block is added, or none at all.
       */
      block_buffer& write
        (unsigned char const* s, size_t count, api_error& ae) noexcept;

      /**
       * @brief Look at the output.
       * @return the block string for output
       * @note Format: @verbatim
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
        Note that both the 14-bit and 30-bit distances start with
        zero (0) representing the most recent previous byte. Both Brotli
        and DEFLATE instead use one (1) to represent the most recent
        previous byte.
       @endverbatim
       *
       * @note Examples (values in hexadecimal): @verbatim
       (03)(41)(62)(63)         -> "Abc"
       (83)(80)(01)             -> copy 3 bytes, distance 1
       (01)(54)(83)(80)(00)     -> "TTTT"
       (01)(54)(51)(83)(80)(01) -> "TQTTT"
       (C0)(05)(90)(02)         -> copy 69 bytes, distance 4098
       (C0)(06)(C0)(00)(00)(03) -> copy 70 bytes, distance 16387
       (84)(05)(00)(02)         -> "life the " (bdict length 4 filter 5 word 2)
       @endverbatim
       */
      block_string const& str(void) const noexcept;
      /** @} */
    public: /** @name methods *//** @{ */
      /**
       * @brief Clear the output buffer.
       */
      void clear_output(void) noexcept;
      /**
       * @brief Add some bytes to the slide ring,
       *   bypassing the input buffer.
       * @param s data to add to the slide ring
       * @param count size of the added data
       * @return the number of bytes successfully added
       * @throw api_exception on failure
       */
      size_t bypass(unsigned char const* s, size_t count);
      /**
       * @brief Add some bytes to the slide ring,
       *   bypassing the input buffer.
       * @param s data to add to the slide ring
       * @param count size of the added data
       * @param[out] ae @em error-code api_error::Success on success,
       *   other value on failure
       * @return the number of bytes successfully added
       */
      size_t bypass
        (unsigned char const* s, size_t count, api_error& ae) noexcept;
      /** @} */

    private: /** @name rule-of-six *//** @{ */
      void transfer(block_buffer&& ) noexcept;
      void transfer(block_buffer const& ) = delete;
      /** @} */
    };
    //END   block buffer

    //BEGIN block buffer / exports
    template
    class TCMPLX_AP_API util_unique_ptr<block_buffer>;
    //END   block buffer / exports

    //BEGIN block buffer / allocation (namespace local)
    /**
     * @brief Non-throwing block buffer allocator.
     * @return a block buffer on success, `nullptr` otherwise
     */
    TCMPLX_AP_API
    block_buffer* blockbuf_new
      (uint32 block_size, uint32 n, size_t chain_length, bool use_bdict)
      noexcept;

    /**
     * @brief Non-throwing block buffer allocator.
     * @return a block buffer on success, `nullptr` otherwise
     */
    TCMPLX_AP_API
    util_unique_ptr<block_buffer> blockbuf_unique
      (uint32 block_size, uint32 n, size_t chain_length, bool use_bdict)
      noexcept;

    /**
     * @brief Destroy a block buffer.
     * @param x (nullable) the block buffer to destroy
     */
    TCMPLX_AP_API
    void blockbuf_destroy(block_buffer* x) noexcept;
    //END   block buffer / allocation (namespace local)
    /** @} */
  };
};

#include "blockbuf.txx"

#endif //hg_TextComplexAccessP_BlockBuf_H_
