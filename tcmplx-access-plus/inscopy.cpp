/**
 * @file tcmplx-access-plus/inscopy.cpp
 * @brief table for insert and copy lengths
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#define TCMPLX_AP_WIN32_DLL_INTERNAL
#include "inscopy.hpp"
#include <new>
#include <stdexcept>
#include <cstring>
#include <limits>

namespace text_complex {
  namespace access {
    /**
     * @brief Fill an insert-copy table with the DEFLATE
     *   literal-length alphabet.
     * @param r array of 286 rows
     */
    static
    void inscopy_1951_fill(inscopy_row* r);
    /**
     * @brief Fill an insert-copy table with the Brotli
     *   length alphabet cross product.
     * @param r array of 704 rows
     */
    static
    void inscopy_7932_fill(inscopy_row* r);
    
    //BEGIN insert_copy_table / static
    void inscopy_1951_fill(inscopy_row* r) {
      size_t i;
      unsigned short first_insert = 3u;
      unsigned short bits = 0u;
      /* put literals */for (i = 0u; i < 256u; ++i) {
        r[i].code = static_cast<unsigned short>(i);
        r[i].type = inscopy_type::Literal;
        r[i].zero_distance_tf = 0;
        r[i].insert_bits = 0;
        r[i].copy_bits = 0;
        r[i].insert_first = 0;
        r[i].copy_first = 0;
      }
      /* put stop code */{ /* i=256; */
        r[i].code = static_cast<unsigned short>(i);
        r[i].type = inscopy_type::Stop;
        r[i].zero_distance_tf = 0;
        r[i].insert_bits = 0;
        r[i].copy_bits = 0;
        r[i].insert_first = 0;
        r[i].copy_first = 0;
        ++i;
      }
      /* put some zero-bit insert codes */for (; i < 261u; ++i) {
        r[i].code = static_cast<unsigned short>(i);
        r[i].type = inscopy_type::Insert;
        r[i].zero_distance_tf = 0;
        r[i].insert_bits = 0;
        r[i].copy_bits = 0;
        r[i].insert_first = first_insert;
        r[i].copy_first = 0;
        first_insert += 1u;
      }
      /* put most of the other insert codes */for (; i < 285u; ++i) {
        r[i].code = static_cast<unsigned short>(i);
        r[i].type = inscopy_type::Insert;
        r[i].zero_distance_tf = 0;
        r[i].insert_bits = bits;
        r[i].copy_bits = 0;
        r[i].insert_first = first_insert;
        r[i].copy_first = 0;
        first_insert += (1u<<bits);
        if ((i%4) == 0) {
          bits += 1u;
        }
      }
      /* put code 285 */{ /* i=285; */
        r[i].code = static_cast<unsigned short>(i);
        r[i].type = inscopy_type::Insert;
        r[i].zero_distance_tf = 0;
        r[i].insert_bits = 0;
        r[i].copy_bits = 0;
        r[i].insert_first = first_insert;
        r[i].copy_first = 0;
        /* ++i; */
      }
      return;
    }

    void inscopy_7932_fill(inscopy_row* r) {
      size_t i;
      struct tab {
        unsigned char bits;
        unsigned short first;
      };
      static constexpr struct {
        bool zero_dist_tf : 1;
        unsigned short insert_start : 5;
        unsigned short copy_start : 5;
      } lookup_matrix[11] = {
        {true,  0, 0}, {true,  0, 8},
        {false, 0, 0}, {false, 0, 8}, /*          */
        {false, 8, 0}, {false, 8, 8}, /*          */
        /*          */ /*          */ /*          */

        /*          */ /*          */ {false, 0,16},
        /*          */ /*          */ /*          */
        {false,16, 0}, /*          */ /*          */

        /*          */ /*          */ /*          */
        /*          */ /*          */ {false, 8,16},
        /*          */ {false,16, 8}, {false,16,16}
      };
      static constexpr struct tab insert_tabs[24] = {
        {0,  0}, {0,  1}, {0,   2}, {0,   3}, {0,   4}, {0,    5},
        {1,  6}, {1,  8}, {2,  10}, {2,  14}, {3,  18}, {3,   26},
        {4, 34}, {4, 50}, {5,  66}, {5,  98}, {6, 130}, {7,  194},
        {8,322}, {9,578},{10,1090},{12,2114},{14,6210},{24,22594}
      };
      static constexpr struct tab copy_tabs[24] = {
        {0,  2}, {0,  3}, {0,   4}, {0,   5}, {0,   6}, {0,    7},
        {0,  8}, {0,  9}, {1,  10}, {1,  12}, {2,  14}, {2,   18},
        {3, 22}, {3, 30}, {4,  38}, {4,  54}, {5,  70}, {5,  102},
        {6,134}, {7,198}, {8, 326}, {9, 582},{10,1094},{24, 2118}
      };
      for (i = 0u; i < 704u; ++i) {
        /* decouple insert code from copy code */
        size_t const j = i/64u;
        unsigned int const insert_code =
          lookup_matrix[j].insert_start + ((i>>3)&7);
        unsigned int const copy_code =
          lookup_matrix[j].copy_start + (i&7);
        /* fill table row */
        r[i].type = inscopy_type::InsertCopy;
        r[i].zero_distance_tf = lookup_matrix[j].zero_dist_tf;
        r[i].insert_bits = insert_tabs[insert_code].bits;
        r[i].insert_first = insert_tabs[insert_code].first;
        r[i].copy_bits = copy_tabs[copy_code].bits;
        r[i].copy_first = copy_tabs[copy_code].first;
        r[i].code = static_cast<unsigned short>(i);
      }
      return;
    }
    //END   insert_copy_table / static

    //BEGIN insert_copy_table / rule-of-six
    insert_copy_table::insert_copy_table(inscopy_preset t)
      : p(nullptr), n(0u)
    {
      resize(n);
      switch (t) {
      case inscopy_preset::Deflate:
        resize(286);
        inscopy_1951_fill(p);
        break;
      case inscopy_preset::Brotli:
        resize(704);
        inscopy_7932_fill(p);
        break;
      default:
        throw api_exception(api_error::Param);
        break;
      }
      return;
    }

    insert_copy_table::~insert_copy_table(void) {
      if (this->p) {
        delete[] this->p;
      }
      this->p = nullptr;
      this->n = 0u;
      return;
    }

    insert_copy_table::insert_copy_table(insert_copy_table const& other)
      : p(nullptr), n(0u)
    {
      duplicate(other);
      return;
    }

    insert_copy_table& insert_copy_table::operator=
        (insert_copy_table const& other)
    {
      duplicate(other);
      return *this;
    }

    insert_copy_table::insert_copy_table(insert_copy_table&& other) noexcept
      : p(nullptr), n(0u)
    {
      transfer(static_cast<insert_copy_table&&>(other));
      return;
    }

    insert_copy_table& insert_copy_table::operator=
        (insert_copy_table&& other) noexcept
    {
      transfer(static_cast<insert_copy_table&&>(other));
      return *this;
    }

    void insert_copy_table::duplicate(insert_copy_table const& other) {
      if (this == &other)
        return;
      resize(other.n);
      size_t i;
      for (i = 0; i < n; ++i) {
        p[i] = other.p[i];
      }
      return;
    }

    void insert_copy_table::transfer(insert_copy_table&& other) noexcept {
      inscopy_row* new_p;
      size_t new_n;
      /* release */{
        new_p = other.p;
        other.p = nullptr;
        new_n = other.n;
        other.n = 0u;
      }
      /* reset */{
        if (this->p) {
          delete[] this->p;
        }
        this->p = new_p;
        this->n = new_n;
      }
      return;
    }

    void insert_copy_table::resize(size_t n) {
      struct inscopy_row *ptr;
      if (n == 0u) {
        if (this->p) {
          delete[] this->p;
        }
        this->p = nullptr;
        this->n = 0u;
        return;
      }
      if (n >= std::numeric_limits<size_t>::max()/
          sizeof(struct inscopy_row))
      {
        throw std::bad_alloc();
      }
      ptr = new struct inscopy_row[n];
      if (this->p) {
        delete[] this->p;
      }
      this->p = ptr;
      this->n = n;
      return;
    }
    //END   insert_copy_table / rule-of-six

    //BEGIN insert_copy_table / allocation
    void* insert_copy_table::operator new(std::size_t sz) {
      return ::operator new(sz);
    }

    void* insert_copy_table::operator new[](std::size_t sz) {
      return ::operator new(sz);
    }

    void insert_copy_table::operator delete(void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    void insert_copy_table::operator delete[](void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    insert_copy_table* inscopy_new(inscopy_preset t) noexcept {
      try {
        return new insert_copy_table(t);
      } catch (api_exception const& ) {
        return nullptr;
      } catch (std::bad_alloc const& ) {
        return nullptr;
      }
    }

    util_unique_ptr<insert_copy_table> inscopy_unique
        (inscopy_preset t) noexcept
    {
      return util_unique_ptr<insert_copy_table>(inscopy_new(t));
    }

    void inscopy_destroy(insert_copy_table* x) noexcept {
      if (x) {
        delete x;
      }
    }
    //END   insert_copy_table / allocation

    //BEGIN insert_copy_table / range-based
    inscopy_row const* insert_copy_table::begin(void) const noexcept {
      return this->p;
    }

    inscopy_row const* insert_copy_table::end(void) const noexcept {
      return this->p+this->n;
    }

    inscopy_row* insert_copy_table::begin(void) noexcept {
      return this->p;
    }

    inscopy_row* insert_copy_table::end(void) noexcept {
      return this->p+this->n;
    }
    //END   insert_copy_table / range-based

    //BEGIN insert_copy_table / array-compat
    size_t insert_copy_table::size(void) const noexcept {
      return this->n;
    }

    inscopy_row const& insert_copy_table::operator[](size_t i) const noexcept {
      return this->p[i];
    }

    inscopy_row const& insert_copy_table::at(size_t i) const {
      if (i >= this->n)
        throw std::out_of_range("text_complex::access::insert_copy_table::at");
      return this->p[i];
    }

    inscopy_row& insert_copy_table::operator[](size_t i) noexcept {
      return this->p[i];
    }

    inscopy_row& insert_copy_table::at(size_t i) {
      if (i >= this->n)
        throw std::out_of_range("text_complex::access::insert_copy_table::at");
      return this->p[i];
    }
    //END   insert_copy_table / array-compat

  };
};
