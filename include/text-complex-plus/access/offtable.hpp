/*
 * \file tcmplx-access-plus/offtable.hpp
 * \brief Access point for TrueType files
 * \author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_OffTable_H_
#define hg_TextComplexAccessP_OffTable_H_

#include "api.hpp"
#include "util.hpp"

namespace text_complex {
  namespace access {
    //BEGIN offset line
    /**
     * \brief Single row of the offset table.
     *
     * Each row describes a data table in a TrueType font file.
     */
    struct TCMPLX_AP_API offset_line final {
      /**
       * \brief Table name.
       */
      unsigned char tag[4];
      /**
       * \brief Table checksum.
       */
      uint32 checksum;
      /**
       * \brief Offset of table in bytes from beginning of file.
       */
      uint32 offset;
      /**
       * \brief Length of table in bytes.
       */
      uint32 length;
    };
    //END   offset line

    //BEGIN offset table
    class TCMPLX_AP_API offset_table final {
    private:
      struct offset_line* p;
      size_t n;

    public /* container-compat */:
      typedef size_t size_type;
      typedef struct offset_line value_type;
      typedef struct offset_line* iterator;
      typedef struct offset_line const* const_iterator;

    public /*rule-of-six*/:
      /**
       * \brief Constructor.
       * \param n desired number of lines in the table
       * \throw `std::bad_alloc` if `n` is too big
       */
      offset_table(size_t n = 0u);
      /**
       * \brief Destructor.
       */
      ~offset_table(void);
      /**
       * \brief Copy constructor.
       */
      offset_table(offset_table const& );
      /**
       * \brief Copy assignment operator.
       * \return this table
       */
      offset_table& operator=(offset_table const& );
      /**
       * \brief Move constructor.
       */
      offset_table(offset_table&& ) noexcept;
      /**
       * \brief Move assignment operator.
       * \return this table
       */
      offset_table& operator=(offset_table&& ) noexcept;

    public /* allocation */:
      /**
       * \brief Scalar memory allocator.
       * \param sz size in `char`s of `offset_table` to allocate
       * \return a pointer to memory on success
       * \throw std::bad_alloc on allocation error
       */
      static void* operator new(std::size_t sz);
      /**
       * \brief Array memory allocator.
       * \param sz size in `char`s of `offset_table[]` to allocate
       * \return a pointer to memory on success
       * \throw std::bad_alloc on allocation error
       */
      static void* operator new[](std::size_t sz);
      /**
       * \brief Scalar memory free callback.
       * \param p pointer to memory to free
       * \param sz size in `char`s of `offset_table` to free
       */
      static void operator delete(void* p, std::size_t sz) noexcept;
      /**
       * \brief Array memory free callback.
       * \param p pointer to memory to free
       * \param sz size in `char`s of `offset_table[]` to free
       */
      static void operator delete[](void* p, std::size_t sz) noexcept;

    public /* range-based */:
      /**
       * \brief `begin` method for range-based `for`.
       * \return a pointer to the first line, or `nullptr` for empty tables
       */
      offset_line* begin(void) noexcept;
      /**
       * \brief `begin` method for range-based `for`.
       * \return a pointer to the first line, or `nullptr` for empty tables
       */
      offset_line const* begin(void) const noexcept;
      /**
       * \brief `end` method for range-based `for`.
       * \return a pointer to one-past the last line,
       *   or `nullptr` for empty tables
       */
      offset_line* end(void) noexcept;
      /**
       * \brief `end` method for range-based `for`.
       * \return a pointer to one-past the last line,
       *   or `nullptr` for empty tables
       */
      offset_line const* end(void) const noexcept;

    public:
      /**
       * \brief Query the size of the table.
       * \return the number of lines in this table
       */
      size_t size(void) const noexcept;
      /**
       * \brief Array index operator.
       * \param i array index
       * \return a reference to the line at the given index
       */
      offset_line& operator[](size_t i) noexcept;
      /**
       * \brief Array index operator.
       * \param i array index
       * \return a reference to the line at the given index
       */
      offset_line const& operator[](size_t i) const noexcept;
      /**
       * \brief Write to an offset table.
       * \param x the table to write
       * \param i an array index
       * \return a reference to an offset line
       * \throw std::out_of_range on bad index
       */
      offset_line& at(size_t i);
      /**
       * \brief Read from an offset table.
       * \param x the table to read
       * \param i an array index
       * \return a pointer to an offset line on success, NULL otherwise
       * \throw std::out_of_range on bad index
       */
      offset_line const& at(size_t i) const;

    private /* rule-of-six */:
      void duplicate(offset_table const& );
      void transfer(offset_table&& ) noexcept;
      void transfer(offset_table const& ) = delete;
      void resize(size_t n);
    };
    //END   offset table

    //BEGIN offset table / exports
    template
    class TCMPLX_AP_API util_unique_ptr<offset_table>;
    //END   offset table / exports

    //BEGIN offset table / namespace local
    /**
     * \brief Non-throwing offset table allocator.
     * \param n desired offset table size
     * \return an offset table on success, nullptr otherwise
     */
    TCMPLX_AP_API
    offset_table* offtable_new(size_t n = 0u) noexcept;

    /**
     * \brief Non-throwing offset table allocator.
     * \param n desired offset table size
     * \return an offset table on success, nullptr otherwise
     */
    TCMPLX_AP_API
    util_unique_ptr<offset_table> offtable_unique(size_t n = 0u) noexcept;

    /**
     * \brief Destroy an offset table.
     * \param x (nullable) the table to destroy
     */
    TCMPLX_AP_API
    void offtable_destroy(offset_table* x) noexcept;
    //END   offset table / namespace local
  };
};


#endif //hg_TextComplexAccessP_OffTable_H_
