/**
 * @file text-complex-plus/access/brmeta.hpp
 * @brief Brotli metadata storage
 */
#ifndef hg_TextComplexAccessP_BrMeta_H_
#define hg_TextComplexAccessP_BrMeta_H_

#include "api.hpp"
#include "util.hpp"

namespace text_complex {
  namespace access {
    class block_string;

    /**
     * @brief Brotli metadata storage.
     */
    class TCMPLX_AP_API brotli_meta final {
    private:
      block_string* lines;
      std::size_t n;
      std::size_t cap;

    public:
      /**
       * @ingroup rule-of-six
       * @brief Constructor.
       * @param reserve initial capacity
       * @throw std::bad_alloc if something breaks
       */
      brotli_meta(std::size_t reserve = 0u);
      /**
       * @ingroup rule-of-six
       * @brief Destructor.
       */
      ~brotli_meta();
      /**
       * @ingroup rule-of-six
       * @brief Move constructor.
       */
      brotli_meta(brotli_meta&& other) noexcept;
      /**
       * @ingroup rule-of-six
       * @brief Move assignment operator.
       */
      brotli_meta& operator=(brotli_meta&& other) noexcept;
      /**
       * @ingroup rule-of-six
       * @brief Copy constructor.
       */
      brotli_meta(brotli_meta const& other);
      /**
       * @ingroup rule-of-six
       * @brief Copy assignment operator.
       */
      brotli_meta& operator=(brotli_meta const& other);

    public:
      /**
       * @ingroup allocation
       * @brief Scalar memory allocator.
       * @param sz size in `char`s of `brcvt_state` to allocate
       * @return a pointer to memory on success
       * @throw std::bad_alloc on allocation error
       */
      static void* operator new(std::size_t sz);
      /**
       * @ingroup allocation
       * @brief Array memory allocator.
       * @param sz size in `char`s of `brcvt_state[]` to allocate
       * @return a pointer to memory on success
       * @throw std::bad_alloc on allocation error
       */
      static void* operator new[](std::size_t sz);
      /**
       * @ingroup allocation
       * @brief Scalar memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `brcvt_state` to free
       */
      static void operator delete(void* p, std::size_t sz) noexcept;
      /**
       * @ingroup allocation
       * @brief Array memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `brcvt_state[]` to free
       */
      static void operator delete[](void* p, std::size_t sz) noexcept;

    public:
      /**
       * @ingroup vector-compat
       * @brief `begin` method for range-based `for`.
       * @return a pointer to the first line, or `nullptr` for empty tables
       */
      block_string* begin(void) noexcept;
      /**
       * @ingroup vector-compat
       * @brief `begin` method for range-based `for`.
       * @return a pointer to the first line, or `nullptr` for empty tables
       */
      block_string const* begin(void) const noexcept;
      /**
       * @ingroup vector-compat
       * @brief `end` method for range-based `for`.
       * @return a pointer to one-past the last line,
       *   or `nullptr` for empty tables
       */
      block_string* end(void) noexcept;
      /**
       * @ingroup vector-compat
       * @brief `end` method for range-based `for`.
       * @return a pointer to one-past the last line,
       *   or `nullptr` for empty tables
       */
      block_string const* end(void) const noexcept;
      /**
       * @ingroup vector-compat
       * @param n length of metadata line in bytes
       * @param[out] ae @em error-code api_error::Success on success,
       *   other value on failure
       */
      void emplace(std::size_t n, api_error& ae) noexcept;
      /**
       * @ingroup vector-compat
       * @param n length of metadata line in bytes
       * @throw api_exception on failure
       */
      void emplace(std::size_t n);
      /**
       * @ingroup vector-compat
       * @brief Check the size.
       * @return the number of lines in storage
       */
      std::size_t size(void) const noexcept;
      /**
       * @ingroup vector-compat
       * @brief Line indexing.
       * @param i index
       * @return a reference to the line at index `i`
       */
      block_string& operator[](std::size_t i) noexcept;
      /**
       * @ingroup vector-compat
       * @brief Line indexing.
       * @param i index
       * @return a reference to the line at index `i`
       */
      block_string const& operator[](std::size_t i) const noexcept;
      /**
       * @ingroup vector-compat
       * @brief Write to an offset table.
       * @param i an array index
       * @return a reference to an offset line
       * @throw std::out_of_range on bad index
       */
      block_string& at(std::size_t i);
      /**
       * @ingroup vector-compat
       * @brief Read from an offset table.
       * @param i an array index
       * @return a pointer to an offset line on success, NULL otherwise
       * @throw std::out_of_range on bad index
       */
      block_string const& at(std::size_t i) const;
    };

    template
    class TCMPLX_AP_API util_unique_ptr<brotli_meta>;

    /**
     * @brief Non-throwing metadata storage allocator.
     * @param reserve initial capacity
     * @return a storage on success, `nullptr` otherwise
     */
    TCMPLX_AP_API
    brotli_meta* brmeta_new(std::size_t reserve) noexcept;

    /**
     * @brief Non-throwing brcvt state allocator.
     * @param reserve initial capacity
     * @return a storage on success, `nullptr` otherwise
     */
    TCMPLX_AP_API
    util_unique_ptr<brotli_meta> brmeta_unique
        (std::size_t reserve) noexcept;

    /**
     * @brief Destroy a storage.
     * @param x (nullable) the storage to destroy
     */
    TCMPLX_AP_API
    void brmeta_destroy(brotli_meta* x) noexcept;
  };
};

#include "brmeta.txx"

#endif //hg_TextComplexAccessP_BrMeta_H_
