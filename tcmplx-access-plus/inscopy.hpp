/**
 * @file tcmplx-access-plus/inscopy.hpp
 * @brief table for insert and copy lengths
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_InsCopy_H_
#define hg_TextComplexAccessP_InsCopy_H_

#include "api.hpp"
#include "util.hpp"

namespace text_complex {
  namespace access {
    //BEGIN insert copy table
    /**
     * @brief Type of insert copy table to generate.
     */
    enum struct insert_copy_preset : int {
      /**
       * @brief Select the DEFLATE 286-code literal-length alphabet.
       */
      Deflate = 0,
      /**
       * @brief Select the Brotli 704-code insert-and-copy-length alphabet.
       */
      BrotliIC = 1,
      /**
       * @brief Select the Brotli 26-code block count alphabet.
       */
      BrotliBlock = 2
    };

    /**
     * @brief Type of insert copy table row.
     */
    enum struct insert_copy_type : unsigned char {
      /**
       * @brief DEFLATE literal value.
       */
      Literal = 0,
      /**
       * @brief DEFLATE block stop code.
       */
      Stop = 1,
      /**
       * @brief DEFLATE copy length.
       */
      Copy = 2,
      /**
       * @brief Brotli insert-copy length code.
       */
      InsertCopy = 3,
      /**
       * @brief Brotli block count code.
       */
      BlockCount = 4,
      /**
       * @brief DEFLATE copy length, minus one valid value in the extra bits.
       * @note DEFLATE code 284 is in this category, accepting one less than
       *   the proper five-bit extra range (i.e. 227-258) according to RFC1951.
       * @note This type compares as equal to
       *   @link insert_copy_type::Copy @endlink for length-based sorting.
       */
      CopyMinus1 = 130
    };

    /**
     * @brief Single row of an insert copy table.
     */
    struct insert_copy_row final {
      /**
       * @brief What type of value this row indicates.
       */
      insert_copy_type type;
      /**
       * @brief Whether this row indicates a reused (thus omitted)
       *   back distance.
       */
      bool zero_distance_tf;
      /**
       * @brief Number of extra bits for the (Brotli) insert code or
       *   the (DEFLATE) length code.
       */
      unsigned char insert_bits;
      /**
       * @brief Number of extra bits for the copy code. Zero for DEFLATE.
       */
      unsigned char copy_bits;
      /**
       * @brief First value for insert length.
       */
      unsigned short insert_first;
      /**
       * @brief First value for copy length.
       */
      unsigned short copy_first;
      /**
       * @brief Alphabet value.
       */
      unsigned short int code;
    };

    /**
     * @brief table for insert and copy lengths.
     */
    class TCMPLX_AP_API insert_copy_table final {
    private:
      struct insert_copy_row* p;
      size_t n;

    public /* container-compat */:
      typedef size_t size_type;
      typedef struct insert_copy_row value_type;
      typedef struct insert_copy_row* iterator;
      typedef struct insert_copy_row const* const_iterator;

    public /*rule-of-six*/:
      /**
       * @brief Constructor.
       * @param n row count
       * @throw `std::bad_alloc` if something breaks, or
       *   @link api_exception @endlink for an invalid table type
       */
      insert_copy_table(size_t n = 0u);
      /**
       * @brief Destructor.
       */
      ~insert_copy_table(void);
      /**
       * @brief Copy constructor.
       */
      insert_copy_table(insert_copy_table const& );
      /**
       * @brief Copy assignment operator.
       * @return this insert copy table
       */
      insert_copy_table& operator=(insert_copy_table const& );
      /**
       * @brief Move constructor.
       */
      insert_copy_table(insert_copy_table&& ) noexcept;
      /**
       * @brief Move assignment operator.
       * @return this insert copy table
       */
      insert_copy_table& operator=(insert_copy_table&& ) noexcept;

    public /* allocation */:
      /**
       * @brief Scalar memory allocator.
       * @param sz size in `char`s of `insert_copy_table` to allocate
       * @return a pointer to memory on success
       * @throw `std::bad_alloc` on allocation error
       */
      static void* operator new(std::size_t sz);
      /**
       * @brief Array memory allocator.
       * @param sz size in `char`s of `insert_copy_table[]` to allocate
       * @return a pointer to memory on success
       * @throw std::bad_alloc on allocation error
       */
      static void* operator new[](std::size_t sz);
      /**
       * @brief Scalar memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `insert_copy_table` to free
       */
      static void operator delete(void* p, std::size_t sz) noexcept;
      /**
       * @brief Array memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `insert_copy_table[]` to free
       */
      static void operator delete[](void* p, std::size_t sz) noexcept;

    public /* range-based */:
      /**
       * @brief `begin` method for range-based `for`.
       * @return a pointer to the first row, or `nullptr` for empty tables
       */
      insert_copy_row const* begin(void) const noexcept;
      /**
       * @brief `end` method for range-based `for`.
       * @return a pointer to one-past the last row,
       *   or `nullptr` for empty tables
       */
      insert_copy_row const* end(void) const noexcept;
      /**
       * @brief `begin` method for range-based `for`.
       * @return a pointer to the first row, or `nullptr` for empty tables
       */
      insert_copy_row* begin(void) noexcept;
      /**
       * @brief `end` method for range-based `for`.
       * @return a pointer to one-past the last row,
       *   or `nullptr` for empty tables
       */
      insert_copy_row* end(void) noexcept;

    public /* array-compat */:
      /**
       * @brief Query the length of the table.
       * @return the number of rows in this table
       */
      size_t size(void) const noexcept;
      /**
       * @brief Array index operator.
       * @param i array index
       * @return a reference to the row at the given index
       */
      insert_copy_row const& operator[](size_t i) const noexcept;
      /**
       * @brief Read from a insert copy table.
       * @param x the list to read
       * @param i an array index
       * @return a pointer to an insert copy row on success, NULL otherwise
       * @throw std::out_of_range on bad index
       */
      insert_copy_row const& at(size_t i) const;
      /**
       * @brief Array index operator.
       * @param i array index
       * @return a reference to the row at the given index
       */
      insert_copy_row& operator[](size_t i) noexcept;
      /**
       * @brief Read from a insert copy table.
       * @param x the list to read
       * @param i an array index
       * @return a pointer to an insert copy row on success, NULL otherwise
       * @throw std::out_of_range on bad index
       */
      insert_copy_row& at(size_t i);

    private /* rule-of-six */:
      void duplicate(insert_copy_table const& );
      void transfer(insert_copy_table&& ) noexcept;
      void transfer(insert_copy_table const& ) = delete;
      void resize(size_t n);
    };
    //END   insert copy table

    //BEGIN insert copy table / exports
    template
    class TCMPLX_AP_API util_unique_ptr<insert_copy_table>;
    //END   insert copy table / exports

    //BEGIN insert copy table / allocation (namespace local)
    /**
     * @brief Non-throwing insert copy table allocator.
     * @param t table preset type
     * @return a insert copy table on success, `nullptr` otherwise
     */
    TCMPLX_AP_API
    insert_copy_table* inscopy_new(size_t n = 0u) noexcept;

    /**
     * @brief Non-throwing insert copy table allocator.
     * @param t table preset type
     * @return a insert copy table on success, `nullptr` otherwise
     */
    TCMPLX_AP_API
    util_unique_ptr<insert_copy_table> inscopy_unique(size_t n = 0u) noexcept;

    /**
     * @brief Destroy a insert copy table.
     * @param x (nullable) the insert copy table to destroy
     */
    TCMPLX_AP_API
    void inscopy_destroy(insert_copy_table* x) noexcept;
    //END   insert copy table / allocation (namespace local)

    //BEGIN insert copy table / namespace local
    /**
     * @brief Assign an insert-copy table with a preset code-value list.
     * @param dst list to populate with lengths
     * @param i preset identifier
     * @param[out] ae @em error-code api_error::Success on success,
     *   nonzero otherwise
     */
    TCMPLX_AP_API
    void inscopy_preset
      (insert_copy_table& dst, insert_copy_preset i, api_error& ae) noexcept;

#if  (!(defined TextComplexAccessP_NO_EXCEPT))
    /**
     * @brief Assign an insert-copy table with a preset code-value list.
     * @param dst list to populate with lengths
     * @param i preset identifier
     * @throw api_exception on storage or coding error
     */
    TCMPLX_AP_API
    void inscopy_preset(insert_copy_table& dst, insert_copy_preset i);
#endif //TextComplexAccessP_NO_EXCEPT

    /**
     * @brief Sort an insert-copy table by alphabet code.
     * @param ict list to sort
     * @param[out] ae @em error-code api_error::Success on success,
     *   nonzero otherwise
     * @note Useful for decoding from a compressed stream.
     */
    TCMPLX_AP_API
    void inscopy_codesort
      (insert_copy_table& ict, api_error& ae) noexcept;

#if  (!(defined TextComplexAccessP_NO_EXCEPT))
    /**
     * @brief Assign an insert-copy table with a preset code-value list.
     * @param ict list to sort
     * @param i preset identifier
     * @throw api_exception on allocation problem
     * @note Useful for decoding from a compressed stream.
     */
    TCMPLX_AP_API
    void inscopy_codesort(insert_copy_table& ict);
#endif //TextComplexAccessP_NO_EXCEPT

    /**
     * @brief Sort an insert-copy table by insert length (primary)
     *   and copy length (secondary).
     * @param ict list to sort
     * @param[out] ae @em error-code api_error::Success on success,
     *   nonzero otherwise
     * @note Useful for encoding to a compressed stream.
     */
    TCMPLX_AP_API
    void inscopy_lengthsort
      (insert_copy_table& ict, api_error& ae) noexcept;

#if  (!(defined TextComplexAccessP_NO_EXCEPT))
    /**
     * @brief Sort an insert-copy table by insert length (primary)
     *   and copy length (secondary).
     * @param ict list to sort
     * @throw api_exception on allocation problem
     */
    TCMPLX_AP_API
    void inscopy_lengthsort(insert_copy_table& ict);
#endif //TextComplexAccessP_NO_EXCEPT

    /**
     * @brief Encode a pair of insert and copy lengths using a table.
     * @param ict the table to read (sorted for length)
     * @param i insert length
     * @param c copy length, or zero if unused
     * @param z_tf whether to select the zero distance variation
     * @return an index of a matching table row if found, SIZE_MAX otherwise
     * @note The zero distance variation is only available for a small
     *   subset of preset insert-copy tables. If the zero distance variation
     *   is unavailable, try again with the default (nonzero distance)
     *   variation instead.
     */
    TCMPLX_AP_API
    size_t inscopy_encode
      ( insert_copy_table const& ict, unsigned long int i,
        unsigned long int c, bool z_tf=false) noexcept;
    //END   insert copy table / namespace local
  };
};

#include "inscopy.txx"

#endif //hg_TextComplexAccessP_InsCopy_H_
