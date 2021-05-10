/**
 * @file text-complex-plus/access/hashchain.hpp
 * @brief Duplicate lookup hash chain
 */
#ifndef hg_TextComplexAccessP_HashChain_H_
#define hg_TextComplexAccessP_HashChain_H_

#include "api.hpp"
#include "util.hpp"
#include "ringslide.hpp"

namespace text_complex {
  namespace access {
    /**
     * @defgroup hashchain Duplicate lookup hash chain
     *   (access/hashchain.hpp)
     * @{
     */
    //BEGIN hash chain
    /**
     * @brief Duplicate lookup hash chain.
     */
    class TCMPLX_AP_API hash_chain final {
    private:
      slide_ring sr;
      unsigned char last_count;
      unsigned char last_bytes[3];
      uint32 counter;
      uint32* chains;
      size_t* positions;
      size_t chain_length;

    public: /** @name string-compat *//** @{ */
      /**
       * @brief Find failure value.
       * @see @link #find @endlink
       */
      static uint32 const npos;
      /** @} */

    public: /** @name rule-of-six*//** @{ */
      /**
       * @brief Constructor.
       * @param n maximum sliding window size
       * @param chain_length run-time parameter limiting hash chain length
       * @throw std::bad_alloc if something breaks
       */
      hash_chain(uint32 n, size_t chain_length);
      /**
       * @brief Destructor.
       */
      ~hash_chain(void);
      /**
       * @brief Copy constructor.
       */
      hash_chain(hash_chain const& ) = delete;
      /**
       * @brief Copy assignment operator.
       * @return this hash chain
       */
      hash_chain& operator=(hash_chain const& ) = delete;
      /**
       * @brief Move constructor.
       */
      hash_chain(hash_chain&& ) noexcept;
      /**
       * @brief Move assignment operator.
       * @return this hash chain
       */
      hash_chain& operator=(hash_chain&& ) noexcept;
      /** @} */

    public: /** @name allocation *//** @{ */
      /**
       * @brief Scalar memory allocator.
       * @param sz size in `char`s of `hash_chain` to allocate
       * @return a pointer to memory on success
       * @throw std::bad_alloc on allocation error
       */
      static void* operator new(std::size_t sz);
      /**
       * @brief Array memory allocator.
       * @param sz size in `char`s of `hash_chain[]` to allocate
       * @return a pointer to memory on success
       * @throw std::bad_alloc on allocation error
       */
      static void* operator new[](std::size_t sz);
      /**
       * @brief Scalar memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `hash_chain` to free
       */
      static void operator delete(void* p, std::size_t sz) noexcept;
      /**
       * @brief Array memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `hash_chain[]` to free
       */
      static void operator delete[](void* p, std::size_t sz) noexcept;
      /** @} */

    public: /** @name container-compat *//** @{ */
      /**
       * @brief Add the most recent byte.
       * @param v the byte to add
       * @throw api_exception on allocation failure
       */
      void push_front(unsigned char v);
      /**
       * @brief Add the most recent byte.
       * @param v the byte to add
       * @param[out] ae @em error-code api_error::Success on success
       *   api_error::Memory otherwise
       */
      void push_front(unsigned char v, api_error& ae) noexcept;

      /**
       * @brief Query the number of bytes held by the sliding window.
       * @return the count of past bytes in the window
       */
      uint32 size(void) const noexcept;
      /**
       * @brief Query a past byte.
       * @param i number of bytes to go back; zero is most recent
       * @return a reference to the byte at the given index
       */
      unsigned char const& operator[](uint32 i) const noexcept;
      /**
       * @brief Read from a sliding window.
       * @param i number of bytes to go back; zero is most recent
       * @return a reference to the byte at the given index
       * @throw api_exception on bad index
       */
      unsigned char const& at(uint32 i) const;
      /** @} */

    public: /** @name methods *//** @{ */
      /**
       * @brief Query the window size of the slide ring.
       * @return a sliding window size
       */
      uint32 extent(void) const noexcept;
      /**
       * @brief Search for a byte sequence.
       * @param b three byte sequence for which to look
       * @param i number of bytes to go back; zero is most recent
       * @return a backward distance where to find the three-byte
       *   sequence, or #npos if not found
       */
      uint32 find(unsigned char const* b, uint32 pos = 0u) const noexcept;
      /** @} */

    private: /** @name rule-of-six *//** @{ */
      //void duplicate(hash_chain const& );
      void transfer(hash_chain&& ) noexcept;
      void transfer(hash_chain const& ) = delete;
      /** @} */
    };
    //END   hash chain

    //BEGIN hash chain / exports
    template
    class TCMPLX_AP_API util_unique_ptr<hash_chain>;
    //END   hash chain / exports

    //BEGIN hash chain / allocation (namespace local)
    /**
     * @brief Non-throwing hash chain allocator.
     * @param n maximum sliding window size
     * @param chain_length run-time parameter limiting hash chain length
     * @return a hash chain on success, `nullptr` otherwise
     */
    TCMPLX_AP_API
    hash_chain* hashchain_new(uint32 n, size_t chain_length) noexcept;

    /**
     * @brief Non-throwing hash chain allocator.
     * @param n maximum sliding window size
     * @param chain_length run-time parameter limiting hash chain length
     * @return a hash chain on success, `nullptr` otherwise
     */
    TCMPLX_AP_API
    util_unique_ptr<hash_chain> hashchain_unique
        (uint32 n, size_t chain_length) noexcept;

    /**
     * @brief Destroy a hash chain.
     * @param x (nullable) the hash chain to destroy
     */
    TCMPLX_AP_API
    void hashchain_destroy(hash_chain* x) noexcept;
    //END   hash chain / allocation (namespace local)
    /** @} */
  };
};

#include "hashchain.txx"

#endif //hg_TextComplexAccessP_HashChain_H_
