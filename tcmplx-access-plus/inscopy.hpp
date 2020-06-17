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
    enum struct inscopy_preset : int {
      /**
       * @brief Select the DEFLATE 285-code literal-length alphabet.
       */
      Deflate = 0,
      /**
       * @brief Select the Brotli 704-code insert-and-copy-length alphabet.
       */
      Brotli = 1
    };

    /**
     * @brief Type of insert copy table row.
     */
    enum struct inscopy_type : unsigned char {
      /**
       * @brief DEFLATE literal value.
       */
      Literal = 0,
      /**
       * @brief DEFLATE block stop code.
       */
      Stop = 1,
      /**
       * @brief DEFLATE insert length.
       */
      Insert = 2,
      /**
       * @brief Brotli insert-copy length code.
       */
      InsertCopy = 3
    };

    /**
     * @brief Single row of an insert copy table.
     */
    struct inscopy_row final {
      /**
       * @brief What type of value this row indicates.
       */
      inscopy_type type;
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
      struct inscopy_row* p;
      size_t n;

    public /* container-compat */:
      typedef size_t size_type;
      typedef struct inscopy_row value_type;
      typedef struct inscopy_row* iterator;
      typedef struct inscopy_row const* const_iterator;

    public /*rule-of-six*/:
      /**
       * @brief Constructor.
       * @param t table preset type
       * @throw `std::bad_alloc` if something breaks, or
       *   @link api_exception @endlink for an invalid table type
       */
      insert_copy_table(inscopy_preset t);
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
      inscopy_row const* begin(void) const noexcept;
      /**
       * @brief `end` method for range-based `for`.
       * @return a pointer to one-past the last row,
       *   or `nullptr` for empty tables
       */
      inscopy_row const* end(void) const noexcept;
      /**
       * @brief `begin` method for range-based `for`.
       * @return a pointer to the first row, or `nullptr` for empty tables
       */
      inscopy_row* begin(void) noexcept;
      /**
       * @brief `end` method for range-based `for`.
       * @return a pointer to one-past the last row,
       *   or `nullptr` for empty tables
       */
      inscopy_row* end(void) noexcept;

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
      inscopy_row const& operator[](size_t i) const noexcept;
      /**
       * @brief Read from a insert copy table.
       * @param x the list to read
       * @param i an array index
       * @return a pointer to an insert copy row on success, NULL otherwise
       * @throw std::out_of_range on bad index
       */
      inscopy_row const& at(size_t i) const;
      /**
       * @brief Array index operator.
       * @param i array index
       * @return a reference to the row at the given index
       */
      inscopy_row& operator[](size_t i) noexcept;
      /**
       * @brief Read from a insert copy table.
       * @param x the list to read
       * @param i an array index
       * @return a pointer to an insert copy row on success, NULL otherwise
       * @throw std::out_of_range on bad index
       */
      inscopy_row& at(size_t i);

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
    insert_copy_table* inscopy_new(inscopy_preset t) noexcept;

    /**
     * @brief Non-throwing insert copy table allocator.
     * @param t table preset type
     * @return a insert copy table on success, `nullptr` otherwise
     */
    TCMPLX_AP_API
    util_unique_ptr<insert_copy_table> inscopy_unique
        (inscopy_preset t) noexcept;

    /**
     * @brief Destroy a insert copy table.
     * @param x (nullable) the insert copy table to destroy
     */
    TCMPLX_AP_API
    void inscopy_destroy(insert_copy_table* x) noexcept;
    //END   insert copy table / allocation (namespace local)
  };
};

#endif //hg_TextComplexAccessP_InsCopy_H_
