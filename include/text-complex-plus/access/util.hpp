/**
 * @file text-complex-plus/access/util.hpp
 * @brief Utility functions for `text-complex-plus`
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_Util_H_
#define hg_TextComplexAccessP_Util_H_

#include "api.hpp"

namespace text_complex {
  namespace access {
    /**
     * @defgroup util Utility functions for text-complex-plus
     *   (access/util.hpp)
     * @{
     */
    //BEGIN metaprogramming
    /**
     * @brief A `declval` substitute.
     * @tparam t type of object to declare
     * @note Leave this template alone. Use `std::declval` instead.
     */
    template <typename t>
    t&& util_declval(void) noexcept;
    //END   metaprogramming

    //BEGIN allocation
    /**
     * @brief Allocate some memory.
     * @param sz number of bytes to allocate on the "heap"
     * @return a pointer to the memory on success, `nullptr` on failure
     */
    TCMPLX_AP_API
    void* util_op_new(std::size_t sz) noexcept;

    /**
     * @brief Allocate some memory.
     * @param n number of elements to allocate on the "heap"
     * @param sz size of each element
     * @return a pointer to the memory on success, `nullptr` on failure
     */
    TCMPLX_AP_API
    void* util_op_new_count(std::size_t n, std::size_t sz ) noexcept;

    /**
     * @brief Free some memory.
     * @param p the memory to free
     */
    TCMPLX_AP_API
    void util_op_delete(void* p) noexcept;

    /**
     * @brief Allocate and initialize some memory.
     * @tparam t type of instance to allocate
     * @param n number of instances to allocate on the "heap"
     * @param args per-item construction arguments
     * @return a pointer to the memory on success, `nullptr` on failure
     * @note Calls `util_op_new`.
     */
    template <typename t, typename ...u>
    void* util_op_new_type(std::size_t n, u&&... args)
      noexcept(noexcept(t(util_declval<u>()...)));

    /**
     * @brief Free some memory.
     * @tparam t type of instance to free
     * @param sz number of items in the memory
     * @param p the memory to free
     * @note Calls `util_op_delete`.
     */
    template <typename t>
    void util_op_delete_type(std::size_t sz, t* p) noexcept;

    /**
     * @brief Exportable unique pointer.
     * @tparam t type of object to manage
     * @note Leave this class alone. Use `std::unique_ptr` instead.
     */
    template <typename t>
    class util_unique_ptr {
    private:
      t* p;

    public: /** @name typedefs *//** @{ */
      /** @brief Pointer type. */
      typedef t* pointer;
      /** @brief Type of object managed by the `unique_ptr`. */
      typedef t element_type;
      /** @} */

    public: /** @name rule-of-six*//** @{ */
      /**
       * @brief Default constructor. The pointer is empty.
       */
      constexpr util_unique_ptr(void) noexcept;
      /**
       * @brief Null-pointer constructor. The pointer is empty.
       */
      constexpr util_unique_ptr(std::nullptr_t ) noexcept;
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
       * @tparam u type of object from which to receive an object
       * @tparam v @em sfinae `u` should support the `release` operation
       *   of a `std::unique_ptr`
       */
      template <typename u, typename v=decltype(util_declval<u>().release())>
      util_unique_ptr(u&& p) noexcept(noexcept(util_declval<u>().release()));
      /**
       * @brief Move assignment operator.
       * @tparam u type of object from which to receive an object
       * @tparam v @em sfinae `u` should support the `release` operation
       *   of a `std::unique_ptr`
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
       * @tparam u type of object to release
       * @tparam v @em sfinae `u` should support the `reset` operation
       *   of a `std::unique_ptr`
       * @return an object holding the given pointer
       * @note calls `this->release()`
       */
      template <
          typename u,
          typename v = decltype(u().reset(util_declval<t*>()))
        >
      operator u(void) && noexcept(noexcept(u().reset(util_declval<t*>())));

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
      /** @} */

    public: /** @name unique_ptr-compat *//** @{ */
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
      /** @} */
    };

    /**
     * @brief Exportable unique pointer.
     * @tparam t type of objects to manage as an array
     * @note Leave this class alone. Use `std::unique_ptr` instead.
     */
    template <typename t>
    class util_unique_ptr<t[]> {
    private:
      t* p;

    public: /** @name typedefs *//** @{ */
      /** @brief Pointer type. */
      typedef t* pointer;
      /** @brief Type of object managed by the `unique_ptr`. */
      typedef t element_type;
      /** @} */

    public: /** @name rule-of-six*//** @{ */
      /**
       * @brief Default constructor. The pointer is empty.
       */
      constexpr util_unique_ptr(void) noexcept;
      /**
       * @brief Null-pointer constructor. The pointer is empty.
       */
      constexpr util_unique_ptr(std::nullptr_t ) noexcept;
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
       * @tparam u type of object from which to receive an object
       * @tparam v @em sfinae `u` should support the `release` operation
       *   of a `std::unique_ptr`
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
       * @tparam u type of object to release
       * @tparam v @em sfinae `u` should support the `reset` operation
       *   of a `std::unique_ptr`
       * @return an object holding the given pointer
       * @note calls `this->release()`
       */
      template <
          typename u,
          typename v = decltype(u().reset(util_declval<t*>()))
        >
      operator u(void) && noexcept(noexcept(u().reset(util_declval<t*>())));

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
      /** @} */

    public: /** @name unique_ptr-compat *//** @{ */
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
      /** @} */
    };

    /**
     * @brief A `make_unique` substitute.
     * @tparam t type to construct
     * @tparam a argument types
     * @return a unique pointer holding an instance of `t`
     * @note Leave this template alone. Use `std::make_unique` instead.
     */
    template <typename t, typename ...a>
    util_unique_ptr<t> util_make_unique(a&&...);
    //END   allocation

    //BEGIN char array
    /**
     * @brief Move bytes of data.
     * @param dst destination character array
     * @param src source character array
     * @param sz byte count
     */
    TCMPLX_AP_API
    void util_memmove(void* dst, void const* src, size_t sz);
    //END   char array

    //BEGIN limits
    /**
     * @brief Compute the signed maximum for `std::streamsize`.
     * @return a maximum value
     */
    TCMPLX_AP_API
    size_t util_ssize_max(void);

    /**
     * @brief Compute the signed maximum for `long int`.
     * @return a maximum value
     */
    TCMPLX_AP_API
    long int util_long_max(void);

    /**
     * @brief Compute the signed minimum for `long int`.
     * @return a minimum value
     * @note Handle with caution.
     */
    TCMPLX_AP_API
    long int util_long_min(void);
    //END   limits
    /** @} */
  };
};

#include "util.txx"

#endif /*hg_TextComplexAccessP_Util_H_*/
