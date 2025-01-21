/**
 * @file text-complex-plus/access/gaspvec.hpp
 * @brief Prefix gasp vector.
 */
#ifndef hg_TextComplexAccessP_GaspVec_H_
#define hg_TextComplexAccessP_GaspVec_H_

#include "api.hpp"
#include "util.hpp"

namespace text_complex {
  namespace access {
    class prefix_list;

    /**
     * @brief Vector of prefix trees.
     */
    class TCMPLX_AP_API gasp_vector final {
    private:
      prefix_list* p;
      size_t n;

    public:
      /**
       * @ingroup container-compat
       * @brief Data type used for array sizes.
       */
      typedef std::size_t size_type;
      /**
       * @ingroup container-compat
       * @brief Data type stored in the list.
       */
      typedef prefix_list value_type;
      /**
       * @ingroup container-compat
       * @brief Data type used for mutable iteration.
       */
      typedef prefix_list* iterator;
      /**
       * @ingroup container-compat
       * @brief Data type used for const iteration.
       */
      typedef prefix_list const* const_iterator;

    public:
      /**
       * @brief Constructor.
       * @param n number of prefix trees
       * @throw `std::bad_alloc` if something breaks
       */
      explicit gasp_vector(std::size_t n = 0u);
      /**
       * @brief Destructor.
       */
      ~gasp_vector();
      /**
       * @brief Copy constructor.
       */
      gasp_vector(gasp_vector const& );
      /**
       * @brief Copy assignment operator.
       * @return this prefix list
       */
      gasp_vector& operator=(gasp_vector const& );
      /**
       * @brief Move constructor.
       */
      gasp_vector(gasp_vector&& ) noexcept;
      /**
       * @brief Move assignment operator.
       * @return this prefix list
       */
      gasp_vector& operator=(gasp_vector&& ) noexcept;

    public:
      /**
       * @ingroup allocation
       * @brief Scalar memory allocator.
       * @param sz size in `char`s of `gasp_vector` to allocate
       * @return a pointer to memory on success
       * @throw `std::bad_alloc` on allocation error
       */
      static void* operator new(std::size_t sz);
      /**
       * @ingroup allocation
       * @brief Array memory allocator.
       * @param sz size in `char`s of `gasp_vector[]` to allocate
       * @return a pointer to memory on success
       * @throw std::bad_alloc on allocation error
       */
      static void* operator new[](std::size_t sz);
      /**
       * @ingroup allocation
       * @brief Scalar memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `gasp_vector` to free
       */
      static void operator delete(void* p, std::size_t sz) noexcept;
      /**
       * @ingroup allocation
       * @brief Array memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `gasp_vector[]` to free
       */
      static void operator delete[](void* p, std::size_t sz) noexcept;

    public:
      /**
       * @ingroup range-based
       * @brief `begin` method for range-based `for`.
       * @return a pointer to the first line, or `nullptr` for empty tables
       */
      prefix_list* begin() noexcept;
      /**
       * @ingroup range-based
       * @brief `begin` method for range-based `for`.
       * @return a pointer to the first line, or `nullptr` for empty tables
       */
      prefix_list const* begin() const noexcept;
      /**
       * @ingroup range-based
       * @brief `end` method for range-based `for`.
       * @return a pointer to one-past the last line,
       *   or `nullptr` for empty tables
       */
      prefix_list* end() noexcept;
      /**
       * @ingroup range-based
       * @brief `end` method for range-based `for`.
       * @return a pointer to one-past the last line,
       *   or `nullptr` for empty tables
       */
      prefix_list const* end() const noexcept;

    public:
      /**
       * @ingroup array-compat
       * @brief Query the size of the list.
       * @return the number of lines in this list
       */
      size_t size() const noexcept;
      /**
       * @ingroup array-compat
       * @brief Array index operator.
       * @param i array index
       * @return a reference to the line at the given index
       */
      prefix_list& operator[](size_t i) noexcept;
      /**
       * @ingroup array-compat
       * @brief Array index operator.
       * @param i array index
       * @return a reference to the line at the given index
       */
      prefix_list const& operator[](size_t i) const noexcept;
      /**
       * @ingroup array-compat
       * @brief Write to a prefix list.
       * @param i an array index
       * @return a reference to an offset line
       * @throw std::out_of_range on bad index
       */
      prefix_list& at(size_t i);
      /**
       * @ingroup array-compat
       * @brief Read from a prefix list.
       * @param i an array index
       * @return a pointer to a prefix line on success, NULL otherwise
       * @throw std::out_of_range on bad index
       */
      prefix_list const& at(size_t i) const;

    private: /** @name rule-of-six *//** @{ */
      void copy(gasp_vector const& );
      void resize(size_t n);
      /** @} */
    };

#pragma region("gasp_vector / exports")
    template
    class TCMPLX_AP_API util_unique_ptr<gasp_vector>;
#pragma endregion

    /**
     * @ingroup allocation-namespace-local
     * @brief Non-throwing prefix gasp vector allocator.
     * @param n number of prefix trees
     * @return a gasp vector on success, `nullptr` otherwise
     */
    TCMPLX_AP_API
    gasp_vector* gaspvec_new(std::size_t n = 0u) noexcept;

    /**
     * @ingroup allocation-namespace-local
     * @brief Non-throwing prefix gasp vector allocator.
     * @param n number of prefix trees
     * @return a gasp vector on success, `nullptr` otherwise
     */
    TCMPLX_AP_API
    util_unique_ptr<gasp_vector> gaspvec_unique(std::size_t n = 0u) noexcept;

    /**
     * @ingroup allocation-namespace-local
     * @brief Destroy a prefix gasp vector.
     * @param x (nullable) the gasp vector to destroy
     */
    TCMPLX_AP_API
    void gaspvec_destroy(gasp_vector* x) noexcept;
  };
};

#pragma region("gaspvec / inline")
namespace text_complex {
  namespace access {
#if  (!(defined TextComplexAccessP_NO_EXCEPT))
    inline
    prefix_list& gasp_vector::at(size_t i) {
      if (i >= size())
        throw api_exception(api_error::OutOfRange);
      return operator[](i);
    }

    inline
    prefix_list const& gasp_vector::at(size_t i) const {
      if (i >= size())
        throw api_exception(api_error::OutOfRange);
      return operator[](i);
    }
#endif //TextComplexAccessP_NO_EXCEPT
  };
};
#pragma endregion

#endif //hg_TextComplexAccessP_GaspVec_H_
