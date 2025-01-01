/**
 * @file text-complex-plus/access/fixlist.hpp
 * @brief Prefix code list
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_FixList_H_
#define hg_TextComplexAccessP_FixList_H_

#include "api.hpp"
#include "util.hpp"

namespace text_complex {
  namespace access {
    /**
     * @defgroup fixlist Prefix code list
     *   (access/fixlist.hpp)
     * @{
     */
    //BEGIN prefix preset identifiers
    /**
     * @brief Identifiers for prefix code list presets.
     */
    enum struct prefix_preset : unsigned int {
      /**
       * @brief Alphabet for Brotli complex prefix codes.
       */
      BrotliComplex = 0,
      /**
       * @brief Code lengths for Brotli simple prefix list `(NSYM=1)`.
       * @note Sort the values before applying the prefix codes.
       *   Use @link fixlist_gen_codes @endlink if necessary.
       */
      BrotliSimple1 = 1,
      /**
       * @brief Code lengths for Brotli simple prefix list `(NSYM=2)`.
       * @note Sort the values before applying the prefix codes.
       *   Use @link fixlist_gen_codes @endlink if necessary.
       */
      BrotliSimple2 = 2,
      /**
       * @brief Code lengths for Brotli simple prefix list `(NSYM=3)`.
       * @note Sort the values before applying the prefix codes.
       *   Use @link fixlist_gen_codes @endlink if necessary.
       */
      BrotliSimple3 = 3,
      /**
       * @brief Code lengths for Brotli simple prefix list
       *   `(NSYM=4, tree-select bit 0)`.
       * @note Sort the values before applying the prefix codes.
       *   Use @link fixlist_gen_codes @endlink if necessary.
       */
      BrotliSimple4A = 4,
      /**
       * @brief Code lengths for Brotli simple prefix list
       *   `(NSYM=4, tree-select bit 1)`.
       * @note Sort the values before applying the prefix codes.
       *   Use @link fixlist_gen_codes @endlink if necessary.
       */
      BrotliSimple4B = 5,
      /**
       * @brief Code lengths for Brotli window bits.
       */
      BrotliWBits = 6,
      /**
       * @brief Code lengths for Brotli literal block types
       *   `NBLTYPESL`.
       * @note Also used for `NBLTYPESI`, `NBLTYPESD`, `NTREESL`, and `NTREESD`.
       */
      BrotliBlockType = 7,
    };
    //END   prefix preset identifiers

    //BEGIN prefix code line
    /**
     * @brief A single line from a prefix code list.
     */
    struct prefix_line {
      /** @brief Prefix bits. */
      unsigned short int code;
      /** @brief Length of prefix in bits. */
      unsigned short int len;
      /** @brief Alphabet code for a prefix. */
      unsigned long int value;
    };
    //END   prefix code line

    //BEGIN prefix list
    /**
     * @brief Prefix code list.
     */
    class TCMPLX_AP_API prefix_list final {
    private:
      struct prefix_line* p;
      size_t n;

    public: /** @name container-compat *//** @{ */
      /** @brief Data type used for array sizes. */
      typedef size_t size_type;
      /** @brief Data type stored in the list. */
      typedef struct prefix_line value_type;
      /** @brief Data type used for mutable iteration. */
      typedef struct prefix_line* iterator;
      /** @brief Data type used for const iteration. */
      typedef struct prefix_line const* const_iterator;
      /** @} */

    public: /** @name rule-of-six *//** @{ */
      /**
       * @brief Constructor.
       * @param n number of prefix code lines
       * @throw `std::bad_alloc` if something breaks
       */
      prefix_list(size_t n = 0u);
      /**
       * @brief Destructor.
       */
      ~prefix_list(void);
      /**
       * @brief Copy constructor.
       */
      prefix_list(prefix_list const& );
      /**
       * @brief Copy assignment operator.
       * @return this prefix list
       */
      prefix_list& operator=(prefix_list const& );
      /**
       * @brief Move constructor.
       */
      prefix_list(prefix_list&& ) noexcept;
      /**
       * @brief Move assignment operator.
       * @return this prefix list
       */
      prefix_list& operator=(prefix_list&& ) noexcept;
      /** @} */

    public: /** allocation *//** @{ */
      /**
       * @brief Scalar memory allocator.
       * @param sz size in `char`s of `prefix_list` to allocate
       * @return a pointer to memory on success
       * @throw `std::bad_alloc` on allocation error
       */
      static void* operator new(std::size_t sz);
      /**
       * @brief Array memory allocator.
       * @param sz size in `char`s of `prefix_list[]` to allocate
       * @return a pointer to memory on success
       * @throw std::bad_alloc on allocation error
       */
      static void* operator new[](std::size_t sz);
      /**
       * @brief Scalar memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `prefix_list` to free
       */
      static void operator delete(void* p, std::size_t sz) noexcept;
      /**
       * @brief Array memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `prefix_list[]` to free
       */
      static void operator delete[](void* p, std::size_t sz) noexcept;
      /** @} */

    public: /** @name range-based *//** @{ */
      /**
       * @brief `begin` method for range-based `for`.
       * @return a pointer to the first line, or `nullptr` for empty tables
       */
      prefix_line* begin(void) noexcept;
      /**
       * @brief `begin` method for range-based `for`.
       * @return a pointer to the first line, or `nullptr` for empty tables
       */
      prefix_line const* begin(void) const noexcept;
      /**
       * @brief `end` method for range-based `for`.
       * @return a pointer to one-past the last line,
       *   or `nullptr` for empty tables
       */
      prefix_line* end(void) noexcept;
      /**
       * @brief `end` method for range-based `for`.
       * @return a pointer to one-past the last line,
       *   or `nullptr` for empty tables
       */
      prefix_line const* end(void) const noexcept;
      /** @} */

    public: /** @name array-compat *//** @{ */
      /**
       * @brief Query the size of the list.
       * @return the number of lines in this list
       */
      size_t size(void) const noexcept;
      /**
       * @brief Array index operator.
       * @param i array index
       * @return a reference to the line at the given index
       */
      prefix_line& operator[](size_t i) noexcept;
      /**
       * @brief Array index operator.
       * @param i array index
       * @return a reference to the line at the given index
       */
      prefix_line const& operator[](size_t i) const noexcept;
      /**
       * @brief Write to a prefix list.
       * @param i an array index
       * @return a reference to an offset line
       * @throw std::out_of_range on bad index
       */
      prefix_line& at(size_t i);
      /**
       * @brief Read from a prefix list.
       * @param i an array index
       * @return a pointer to a prefix line on success, NULL otherwise
       * @throw std::out_of_range on bad index
       */
      prefix_line const& at(size_t i) const;
      /** @} */

    private: /** @name rule-of-six *//** @{ */
      void duplicate(prefix_list const& );
      void transfer(prefix_list&& ) noexcept;
      void transfer(prefix_list const& ) = delete;
      void resize(size_t n);
      /** @} */
    };
    //END   prefix list

    //BEGIN prefix histogram
    /**
     * @brief Prefix code histogram.
     */
    class TCMPLX_AP_API prefix_histogram final {
    private:
      uint32* p;
      size_t n;

    public: /** @name container-compat *//** @{ */
      /** @brief Data type used for array sizes. */
      typedef size_t size_type;
      /** @brief Data type stored in the histogram. */
      typedef uint32 value_type;
      /** @brief Data type used for mutable iteration. */
      typedef uint32* iterator;
      /** @brief Data type used for const iteration. */
      typedef uint32 const* const_iterator;
      /** @} */

    public: /** @name rule-of-six *//** @{ */
      /**
       * @brief Constructor.
       * @param n number of prefix code frequencies
       * @throw `std::bad_alloc` if something breaks
       */
      prefix_histogram(size_t n = 0u);
      /**
       * @brief Destructor.
       */
      ~prefix_histogram(void);
      /**
       * @brief Copy constructor.
       */
      prefix_histogram(prefix_histogram const& );
      /**
       * @brief Copy assignment operator.
       * @return this prefix histogram
       */
      prefix_histogram& operator=(prefix_histogram const& );
      /**
       * @brief Move constructor.
       */
      prefix_histogram(prefix_histogram&& ) noexcept;
      /**
       * @brief Move assignment operator.
       * @return this prefix histogram
       */
      prefix_histogram& operator=(prefix_histogram&& ) noexcept;
      /** @} */

    public: /** allocation *//** @{ */
      /**
       * @brief Scalar memory allocator.
       * @param sz size in `char`s of `prefix_histogram` to allocate
       * @return a pointer to memory on success
       * @throw `std::bad_alloc` on allocation error
       */
      static void* operator new(std::size_t sz);
      /**
       * @brief Array memory allocator.
       * @param sz size in `char`s of `prefix_histogram[]` to allocate
       * @return a pointer to memory on success
       * @throw std::bad_alloc on allocation error
       */
      static void* operator new[](std::size_t sz);
      /**
       * @brief Scalar memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `prefix_histogram` to free
       */
      static void operator delete(void* p, std::size_t sz) noexcept;
      /**
       * @brief Array memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `prefix_histogram[]` to free
       */
      static void operator delete[](void* p, std::size_t sz) noexcept;
      /** @} */

    public: /** @name range-based *//** @{ */
      /**
       * @brief `begin` method for range-based `for`.
       * @return a pointer to the first frequency,
       *   or `nullptr` for empty histograms
       */
      uint32* begin(void) noexcept;
      /**
       * @brief `begin` method for range-based `for`.
       * @return a pointer to the first frequency,
       *   or `nullptr` for empty histograms
       */
      uint32 const* begin(void) const noexcept;
      /**
       * @brief `end` method for range-based `for`.
       * @return a pointer to one-past the last frequency,
       *   or `nullptr` for empty histograms
       */
      uint32* end(void) noexcept;
      /**
       * @brief `end` method for range-based `for`.
       * @return a pointer to one-past the last frequency,
       *   or `nullptr` for empty histograms
       */
      uint32 const* end(void) const noexcept;
      /** @} */

    public: /** @name array-compat *//** @{ */
      /**
       * @brief Query the size of the histogram.
       * @return the number of frequencies in this histogram
       */
      size_t size(void) const noexcept;
      /**
       * @brief Array index operator.
       * @param i array index
       * @return a reference to the frequency at the given index
       */
      uint32& operator[](size_t i) noexcept;
      /**
       * @brief Array index operator.
       * @param i array index
       * @return a reference to the frequency at the given index
       */
      uint32 const& operator[](size_t i) const noexcept;
      /**
       * @brief Write to a prefix histogram.
       * @param i an array index
       * @return a reference to an offset frequency
       * @throw std::out_of_range on bad index
       */
      uint32& at(size_t i);
      /**
       * @brief Read from a prefix histogram.
       * @param i an array index
       * @return a pointer to a prefix frequency on success, NULL otherwise
       * @throw std::out_of_range on bad index
       */
      uint32 const& at(size_t i) const;
      /** @} */

    private: /** @name rule-of-six *//** @{ */
      void duplicate(prefix_histogram const& );
      void transfer(prefix_histogram&& ) noexcept;
      void transfer(prefix_histogram const& ) = delete;
      void resize(size_t n);
      /** @} */
    };
    //END   prefix histogram

    //BEGIN prefix list / exports
    template
    class TCMPLX_AP_API util_unique_ptr<prefix_list>;
    //END   prefix list / exports

    //BEGIN prefix list / allocation (namespace local)
    /**
     * @brief Non-throwing prefix list allocator.
     * @param n number of prefix code lines
     * @return a prefix list on success, `nullptr` otherwise
     */
    TCMPLX_AP_API
    prefix_list* fixlist_new(size_t n = 0u) noexcept;

    /**
     * @brief Non-throwing prefix list allocator.
     * @param n number of prefix code lines
     * @return a prefix list on success, `nullptr` otherwise
     */
    TCMPLX_AP_API
    util_unique_ptr<prefix_list> fixlist_unique(size_t n = 0u) noexcept;

    /**
     * @brief Destroy a prefix list.
     * @param x (nullable) the prefix list to destroy
     */
    TCMPLX_AP_API
    void fixlist_destroy(prefix_list* x) noexcept;
    //END   prefix list / allocation (namespace local)

    //BEGIN prefix histogram / exports
    template
    class TCMPLX_AP_API util_unique_ptr<prefix_histogram>;
    //END   prefix histogram / exports

    //BEGIN prefix histogram / allocation (namespace local)
    /**
     * @brief Non-throwing prefix histogram allocator.
     * @param n number of prefix code lines
     * @return a prefix histogram on success, `nullptr` otherwise
     */
    TCMPLX_AP_API
    prefix_histogram* fixlist_histogram_new(size_t n = 0u) noexcept;

    /**
     * @brief Non-throwing prefix histogram allocator.
     * @param n number of prefix code lines
     * @return a prefix histogram on success, `nullptr` otherwise
     */
    TCMPLX_AP_API
    util_unique_ptr<prefix_histogram> fixlist_histogram_unique
        (size_t n = 0u) noexcept;

    /**
     * @brief Destroy a prefix histogram.
     * @param x (nullable) the prefix histogram to destroy
     */
    TCMPLX_AP_API
    void fixlist_histogram_destroy(prefix_histogram* x) noexcept;
    //END   prefix histogram / allocation (namespace local)

    //BEGIN prefix list / namespace local
    /**
     * @brief Generate prefix codes given a prefix list.
     *
     * This function fills in the bit strings of each prefix code,
     *   using the corresponding @link prefix_line#len @endlink values.
     * @param dst list to populate with codes
     * @param[out] ae @em error-code api_error::Success on success,
     *   nonzero otherwise
     */
    TCMPLX_AP_API
    void fixlist_gen_codes(prefix_list& dst, api_error& ae) noexcept;

#if  (!(defined TextComplexAccessP_NO_EXCEPT))
    /**
     * @brief Generate prefix codes given a prefix list.
     *
     * This function fills in the bit strings of each prefix code,
     *   using the corresponding @link prefix_line#len @endlink values.
     * @param dst list to populate with codes
     * @throw api_exception on code length error
     */
    TCMPLX_AP_API
    void fixlist_gen_codes(prefix_list& dst);
#endif //TextComplexAccessP_NO_EXCEPT

    /**
     * @brief Assign a prefix list with a preset code-value list.
     * @param dst list to populate with lengths
     * @param i preset identifier
     * @param[out] ae @em error-code api_error::Success on success,
     *   nonzero otherwise
     */
    TCMPLX_AP_API
    void fixlist_preset
      (prefix_list& dst, prefix_preset i, api_error& ae) noexcept;

#if  (!(defined TextComplexAccessP_NO_EXCEPT))
    /**
     * @brief Assign a prefix list with a preset code-value list.
     * @param dst list to populate with lengths
     * @param i preset identifier
     * @throw api_exception on storage or code length error
     */
    TCMPLX_AP_API
    void fixlist_preset(prefix_list& dst, prefix_preset i);
#endif //TextComplexAccessP_NO_EXCEPT

    /**
     * @brief Generate prefix code lengths given a prefix list and
     *   histogram of code frequencies.
     * @param dst list to populate with lengths
     * @param table frequency histogram, flat array as long as number
     *   of prefixes in the list
     * @param max_bits maximum output length
     * @param[out] ae @em error-code api_error::Success on success,
     *   nonzero otherwise
     */
    TCMPLX_AP_API
    void fixlist_gen_lengths
      ( prefix_list& dst, prefix_histogram const& table,
        unsigned int max_bits, api_error& ae) noexcept;

#if  (!(defined TextComplexAccessP_NO_EXCEPT))
    /**
     * @brief Generate prefix code lengths given a prefix list and
     *   histogram of code frequencies.
     * @param dst list to populate with lengths
     * @param table frequency histogram, flat array as long as number
     *   of prefixes in the list
     * @param max_bits maximum output length
     * @throw api_exception on storage or code length error
     */
    TCMPLX_AP_API
    void fixlist_gen_lengths
      ( prefix_list& dst, prefix_histogram const& table,
        unsigned int max_bits);
#endif //TextComplexAccessP_NO_EXCEPT

    /**
     * @brief Sort a prefix list by Huffman code.
     * @param dst list to sort
     * @param[out] ae @em error-code api_error::Success on success,
     *   nonzero otherwise
     * @note Useful for decoding from a compressed stream.
     */
    TCMPLX_AP_API
    void fixlist_codesort
      (prefix_list& dst, api_error& ae) noexcept;

#if  (!(defined TextComplexAccessP_NO_EXCEPT))
    /**
     * @brief Sort a prefix list by Huffman code.
     * @param dst list to sort
     * @throw api_exception on allocation problem
     * @note Useful for decoding from a compressed stream.
     */
    TCMPLX_AP_API
    void fixlist_codesort(prefix_list& dst);
#endif //TextComplexAccessP_NO_EXCEPT


    /**
     * @brief Binary search a prefix list by Huffman code.
     * @param dst list sorted by Huffman code
     * @param n length of bit string
     * @param bits bit string
     * @return index if found, `std::numeric_limits<size_t>::%max()` otherwise
     * @note Useful for decoding from a compressed stream.
     */
    TCMPLX_AP_API
    size_t fixlist_codebsearch
      (prefix_list const& dst, unsigned int n, unsigned int bits) noexcept;

    /**
     * @brief Sort a prefix list by alphabet value.
     * @param dst list to sort
     * @param[out] ae @em error-value api_error::Success on success,
     *   nonzero otherwise
     * @note Useful for encoding to a compressed stream.
     */
    TCMPLX_AP_API
    void fixlist_valuesort
      (prefix_list& dst, api_error& ae) noexcept;

#if  (!(defined TextComplexAccessP_NO_EXCEPT))
    /**
     * @brief Sort a prefix list by alphabet value.
     * @param dst list to sort
     * @throw api_exception on allocation problem
     * @note Useful for encoding to a compressed stream.
     */
    TCMPLX_AP_API
    void fixlist_valuesort(prefix_list& dst);
#endif //TextComplexAccessP_NO_EXCEPT

    /**
     * @brief Binary search a prefix list by alphabet value.
     * @param dst list sorted by alphabet value
     * @param value alphabet value
     * @return index if found, `std::numeric_limits<size_t>::%max()` otherwise
     * @note Useful for encoding to a compressed stream.
     */
    TCMPLX_AP_API
    size_t fixlist_valuebsearch
      (prefix_list const& dst, unsigned long int value) noexcept;
    //END   prefix list / namespace local
    /** @} */
  };
};

#include "fixlist.txx"

#endif //hg_TextComplexAccessP_FixList_H_
