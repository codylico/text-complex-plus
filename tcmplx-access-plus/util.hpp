/*
 * \file tcmplx-access/util.hpp
 * \brief Utility functions for `text-complex-plus`
 * \author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_Util_H_
#define hg_TextComplexAccessP_Util_H_

#include "api.hpp"

namespace text_complex {
  namespace access {
    template <typename t>
    t&& util_declval(void) noexcept;

    /**
     * @brief Allocate some memory.
     * @param sz number of bytes to allocate on the "heap"
     * @return a pointer to the memory on success, `nullptr` on failure
     */
    TCMPLX_AP_API
    void* util_op_new(std::size_t sz) noexcept;

    /**
     * @brief Free some memory.
     * @param p the memory to free
     */
    TCMPLX_AP_API
    void util_op_delete(void* p) noexcept;

    /**
     * @brief Exportable unique pointer.
     * @note Leave this class alone. Use `std::unique_ptr` instead.
     */
    template <typename t>
    class util_unique_ptr {
    private:
      t* p;

    public /* typedefs */:
      typedef t* pointer;
      typedef t element_type;

    public /*rule-of-six*/:
      /**
       * @brief Default constructor. The pointer is empty.
       */
      constexpr util_unique_ptr(void) noexcept;
      /**
       * @brief Capture constructor.
       * @param p a pointer to the object to give to the `util_unique_ptr`.
       */
      explicit util_unique_ptr(t* p) noexcept;
      /**
       * @brief Move constructor.
       */
      util_unique_ptr(util_unique_ptr<t>&& p) noexcept;
      /**
       * @brief Move assignment operator.
       */
      util_unique_ptr<t>& operator=(util_unique_ptr<t>&& p) noexcept;
      /**
       * @brief Move constructor.
       */
      template <typename u, typename v=decltype(util_declval<u>().release())>
      util_unique_ptr(u&& p) noexcept(noexcept(util_declval<u>().release()));
      /**
       * @brief Move assignment operator.
       */
      template <typename u, typename v=decltype(util_declval<u>().release())>
      util_unique_ptr& operator=(u&& p)
          noexcept(noexcept(util_declval<u>().release()));
      /**
       * @brief Destructor.
       */
      ~util_unique_ptr(void) noexcept;

      /**
       * @brief Conversion operator.
       * @return an object holding the given pointer
       * @note calls `this->release()`
       */
      template <typename u, typename v=decltype(u(util_declval<t*>()))>
      operator u(void) && noexcept(noexcept(u(util_declval<t*>())));

      /**
       * @brief Relinquish ownership of the contained object.
       * @return a pointer to the contained object, or `nullptr` if empty
       */
      t* release(void) noexcept;

      /**
       * @brief Destroy any contained object, then replaces with another.
       * @param p the replacement object (nullable)
       */
      void reset(t* p) noexcept;

      util_unique_ptr(util_unique_ptr const& ) = delete;
      util_unique_ptr& operator=(util_unique_ptr const& ) = delete;

    public /* unique_ptr-matching */:
      /**
       * @brief Check if this pointer is non-empty.
       * @return true if an object is inside, false otherwise
       */
      operator bool(void) const noexcept;

      /**
       * @brief Obtain a reference to the contained object.
       * @return a reference to the contained object
       * @note Undefined if this is empty.
       */
      t& operator*(void) const noexcept;
      /**
       * @brief Obtain a pointer to the contained object.
       * @return a pointer to the contained object
       */
      t* operator->(void) const noexcept;
      /**
       * @brief Obtain a pointer to the contained object.
       * @return a pointer to the contained object
       */
      t* get(void) const noexcept;
    };

    /**
     * @brief Exportable unique pointer.
     * @note Leave this class alone. Use `std::unique_ptr` instead.
     */
    template <typename t>
    class util_unique_ptr<t[]> {
    private:
      t* p;

    public /* typedefs */:
      typedef t* pointer;
      typedef t element_type;

    public /*rule-of-six*/:
      /**
       * @brief Default constructor. The pointer is empty.
       */
      constexpr util_unique_ptr(void) noexcept;
      /**
       * @brief Capture constructor.
       * @param p a pointer to the object to give to the `util_unique_ptr`.
       */
      explicit util_unique_ptr(t* p) noexcept;
      /**
       * @brief Move constructor.
       */
      util_unique_ptr(util_unique_ptr<t[]>&& p) noexcept;
      /**
       * @brief Move assignment operator.
       */
      util_unique_ptr<t[]>& operator=(util_unique_ptr<t[]>&& p) noexcept;
      /**
       * @brief Move constructor.
       */
      template <typename u, typename v=decltype(util_declval<u>().release())>
      util_unique_ptr(u&& p) noexcept(noexcept(util_declval<u>().release()));
      /**
       * @brief Move assignment operator.
       */
      template <typename u, typename v=decltype(util_declval<u>().release())>
      util_unique_ptr<t[]>& operator=(u&& p)
          noexcept(noexcept(util_declval<u>().release()));
      /**
       * @brief Destructor.
       */
      ~util_unique_ptr(void) noexcept;

      /**
       * @brief Conversion operator.
       * @return an object holding the given pointer
       * @note calls `this->release()`
       */
      template <typename u, typename v=decltype(u(util_declval<t*>()))>
      operator u(void) && noexcept(noexcept(u(util_declval<t*>())));

      /**
       * @brief Relinquish ownership of the contained object.
       * @return a pointer to the contained object, or `nullptr` if empty
       */
      t* release(void) noexcept;

      /**
       * @brief Deestroy any contained object, then replaces with another.
       * @param p the replacement object (nullable)
       */
      void reset(t* p) noexcept;

      util_unique_ptr(util_unique_ptr const& ) = delete;
      util_unique_ptr& operator=(util_unique_ptr const& ) = delete;

    public /* unique_ptr-matching */:
      /**
       * @brief Check if this pointer is non-empty.
       * @return true if an object is inside, false otherwise
       */
      operator bool(void) const noexcept;
      /**
       * @brief Obtain a reference to one of the objects in the
       *   contained array.
       * @param i an array index
       * @return a reference to a contained object
       * @note Undefined if this is empty, or the index is out of range.
       */
      t& operator[](size_t i) const noexcept;
      /**
       * @brief Obtain a pointer to the array of contained objects.
       * @return a pointer to the array of contained objects
       */
      t* get(void) const noexcept;
    };

    template <typename t, typename ...a>
    util_unique_ptr<t> util_make_unique(a&&...);
  };
};

#include "util.txx"

#endif /*hg_TextComplexAccessP_Util_H_*/
