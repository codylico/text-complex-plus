/**
 * @file tcmplx-access-plus/fixlist.hpp
 * @brief Prefix code list
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_FixList_H_
#define hg_TextComplexAccessP_FixList_H_

#include "api.hpp"
#include "util.hpp"

namespace text_complex {
  namespace access {
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
      BrotliSimple4B = 5
    };
    //END   prefix preset identifiers

    //BEGIN prefix code line
    /**
     * @brief A single line from a prefix code list.
     */
    struct prefix_line {
      unsigned short int code;
      unsigned short int len;
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

    public /* container-compat */:
      typedef size_t size_type;
      typedef struct prefix_line value_type;
      typedef struct prefix_line* iterator;
      typedef struct prefix_line const* const_iterator;

    public /*rule-of-six*/:
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

    public /* allocation */:
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

    public /* range-based */:
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

    public /* array-compat */:
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
       * @param x the list to write
       * @param i an array index
       * @return a reference to an offset line
       * @throw std::out_of_range on bad index
       */
      prefix_line& at(size_t i);
      /**
       * @brief Read from a prefix list.
       * @param x the list to read
       * @param i an array index
       * @return a pointer to an offset line on success, NULL otherwise
       * @throw std::out_of_range on bad index
       */
      prefix_line const& at(size_t i) const;

    private /* rule-of-six */:
      void duplicate(prefix_list const& );
      void transfer(prefix_list&& ) noexcept;
      void transfer(prefix_list const& ) = delete;
      void resize(size_t n);
    };
    //END   prefix list

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

    //BEGIN prefix list / namespace local
    /**
     * @brief Generate prefix codes given a prefix list.
     * @param dst list to populate with codes
     * @param[out] ae @em error-code api_error::Success on success,
     *   nonzero otherwise
     */
    TCMPLX_AP_API
    void fixlist_gen_codes(prefix_list& dst, api_error& ae) noexcept;

#if  (!(defined TextComplexAccessP_NO_EXCEPT))
    /**
     * @brief Generate prefix codes given a prefix list.
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
    //END   prefix list / namespace local
  };
};

#include "fixlist.txx"

#endif //hg_TextComplexAccessP_FixList_H_
