/*
 * \file text-complex-plus/access/bdict.hpp
 * \brief Built-in dictionary
 * \author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_BDict_H_
#define hg_TextComplexAccessP_BDict_H_

#include "api.hpp"

namespace text_complex {
  namespace access {
    //BEGIN built-in dictionary word
    struct TCMPLX_AP_API bdict_word final {
    private:
      unsigned char p[38];
      unsigned short len;

    public: /** @name container-compat *//** @{ */
      typedef size_t size_type;
      typedef unsigned char value_type;
      typedef unsigned char* iterator;
      typedef unsigned char const* const_iterator;
      /** @} */

    public: /** @name rule-of-zero *//** @{ */
      /**
       * @brief Zero-initialize a word.
       */
      bdict_word(void) noexcept;

      /**
       * @brief Initialize with a word.
       * @param s bytes to copy
       * @param len length of bytes to copy; should not exceed 37.
       * @throw `api_exception` if len is too long
       */
      bdict_word(unsigned char const* s, size_t len);
      /** @} */

    public: /** @name allocation *//** @{ */
      /**
       * @brief Scalar memory allocator.
       * @param sz size in `char`s of `bdict_word` to allocate
       * @return a pointer to memory on success
       * @throw `std::bad_alloc` on allocation error
       */
      static void* operator new(std::size_t sz);
      /**
       * @brief Array memory allocator.
       * @param sz size in `char`s of `bdict_word[]` to allocate
       * @return a pointer to memory on success
       * @throw `std::bad_alloc` on allocation error
       */
      static void* operator new[](std::size_t sz);
      /**
       * @brief Scalar memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `bdict_word` to free
       */
      static void operator delete(void* p, std::size_t sz) noexcept;
      /**
       * @brief Array memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `bdict_word[]` to free
       */
      static void operator delete[](void* p, std::size_t sz) noexcept;
      /** @} */

    public: /** @name range-based *//** @{ */
      /**
       * @brief `begin` method for range-based `for`.
       * @return a pointer to the first byte
       */
      unsigned char* begin(void) noexcept;
      /**
       * @brief `begin` method for range-based `for`.
       * @return a pointer to the first byte
       */
      unsigned char const* begin(void) const noexcept;
      /**
       * @brief `end` method for range-based `for`.
       * @return a pointer to one-past the last byte
       */
      unsigned char* end(void) noexcept;
      /**
       * @brief `end` method for range-based `for`.
       * @return a pointer to one-past the last byte
       */
      unsigned char const* end(void) const noexcept;
      /** @} */

    public:
      /**
       * @brief Query the size of the word.
       * @return the number of bytes in this word
       */
      size_t size(void) const noexcept;
      /**
       * @brief Array index operator.
       * @param i array index
       * @return a reference to the byte at the given index
       */
      unsigned char& operator[](size_t i) noexcept;
      /**
       * @brief Array index operator.
       * @param i array index
       * @return a reference to the byte at the given index
       */
      unsigned char const& operator[](size_t i) const noexcept;
      /**
       * @brief Write to a word.
       * @param x the table to write
       * @param i an array index
       * @return a reference to a byte
       * @throw @link api_exception @endlink on bad index
       */
      unsigned char& at(size_t i);
      /**
       * @brief Read from a word.
       * @param x the word to read
       * @param i an array index
       * @return a pointer to a word on success, NULL otherwise
       * @throw @link api_exception @endlink on bad index
       */
      unsigned char const& at(size_t i) const;

      /**
       * @brief Resize the byte container.
       * @param sz desired size
       * @param[out] ae error reception variable
       */
      void resize(size_t sz, api_error& ae) noexcept;
      /**
       * @brief Resize the byte container.
       * @param sz desired size
       * @throw `api_exception` if the size is too big (i.e. `> 37`).
       */
      void resize(size_t sz);

      /**
       * @brief Determine maximum size for a byte container.
       * @return 37.
       */
      size_t max_size(void) const noexcept;

      /**
       * @brief Compare two words.
       * @param other the other word
       * @return true if the words are same, false otherwise
       */
      bool operator==(bdict_word const& other) const noexcept;
    };
    //END   built-in dictionary word

    //BEGIN built-in dictionary / namespace local
    /**
     * @brief Acquire the number of words for a given word length.
     * @param j a word length in bytes
     * @return a word count for the given length, or zero if unsupported
     */
    TCMPLX_AP_API
    unsigned int bdict_word_count(unsigned int j) noexcept;

    /**
     * @brief Acquire a word.
     * @param j a word length in bytes
     * @param i word array index
     * @return a word on success, a zero-length word otherwise
     */
    TCMPLX_AP_API
    struct bdict_word bdict_get_word(unsigned int j, unsigned int i) noexcept;

    /**
     * @brief Transform a word.
     * @param[in,out] buf a buffer holding the word
     * @param k transform selector (in range `[0,121)`)
     * @param[out] ae error reception variable
     */
    TCMPLX_AP_API
    void bdict_transform
      (struct bdict_word& buf, unsigned int k, api_error& ae) noexcept;

    /**
     * @brief Transform a word.
     * @param[in,out] buf a buffer holding the word
     * @param k transform selector (in range `[0,121)`)
     * @throw @link api_exception @endlink on bad transform selector
     *   or overlong word
     */
    TCMPLX_AP_API
    void bdict_transform(struct bdict_word& buf, unsigned int k);
    //END   built-in dictionary / namespace local
  };
};

#include "bdict.txx"

#endif //hg_TextComplexAccessP_BDict_H_
