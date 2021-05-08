/**
 * @file text-complex-plus/access/ringslide.hpp
 * @brief Sliding window of past bytes
 */
#ifndef hg_TextComplexAccessP_RingSlide_H_
#define hg_TextComplexAccessP_RingSlide_H_

#include "api.hpp"
#include "util.hpp"

namespace text_complex {
  namespace access {
    /**
     * @defgroup ringslide Sliding window of past bytes
     *   (access/ringslide.hpp)
     * @{
     */
    //BEGIN slide ring
    /**
     * @brief Sliding window of past bytes.
     */
    class TCMPLX_AP_API slide_ring final {
    private:
      uint32 n;
      uint32 pos;
      size_t cap;
      unsigned char* p;

    public: /** @name container-compat *//** @{ */
      /** @brief Data type used for size and indexing. */
      typedef uint32 size_type;
      /** @brief Data type used for byte storage. */
      typedef unsigned char value_type;
      /** @} */

    public: /** @name rule-of-six*//** @{ */
      /**
       * @brief Constructor.
       * @param n size of sliding window in bytes
       * @throw std::bad_alloc if something breaks
       */
      slide_ring(uint32 n);
      /**
       * @brief Destructor.
       */
      ~slide_ring(void);
      /**
       * @brief Copy constructor.
       */
      slide_ring(slide_ring const& );
      /**
       * @brief Copy assignment operator.
       * @return this slide ring
       */
      slide_ring& operator=(slide_ring const& );
      /**
       * @brief Move constructor.
       */
      slide_ring(slide_ring&& ) noexcept;
      /**
       * @brief Move assignment operator.
       * @return this slide ring
       */
      slide_ring& operator=(slide_ring&& ) noexcept;
      /** @} */

    public: /** @name allocation *//** @{ */
      /**
       * @brief Scalar memory allocator.
       * @param sz size in `char`s of `slide_ring` to allocate
       * @return a pointer to memory on success
       * @throw std::bad_alloc on allocation error
       */
      static void* operator new(std::size_t sz);
      /**
       * @brief Array memory allocator.
       * @param sz size in `char`s of `slide_ring[]` to allocate
       * @return a pointer to memory on success
       * @throw std::bad_alloc on allocation error
       */
      static void* operator new[](std::size_t sz);
      /**
       * @brief Scalar memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `slide_ring` to free
       */
      static void operator delete(void* p, std::size_t sz) noexcept;
      /**
       * @brief Array memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `slide_ring[]` to free
       */
      static void operator delete[](void* p, std::size_t sz) noexcept;
      /** @} */

    public: /** @name vector-compat *//** @{ */
      /**
       * @brief Query the size of the sliding window.
       */
      uint32 max_size(void) const noexcept;
      /** @} */

    public: /** @name methods *//** @{ */
      uint32 extent(void) const noexcept;
      /** @} */

    private: /** @name rule-of-six *//** @{ */
      void duplicate(slide_ring const& );
      void transfer(slide_ring&& ) noexcept;
      void transfer(slide_ring const& ) = delete;
      /** @} */
    };
    //END   slide ring

    //BEGIN slide ring / exports
    template
    class TCMPLX_AP_API util_unique_ptr<slide_ring>;
    //END   slide ring / exports

    //BEGIN slide ring / allocation (namespace local)
    /**
     * @brief Non-throwing slide ring allocator.
     * @param n size of sliding window in bytes
     * @return a slide ring on success, `nullptr` otherwise
     */
    TCMPLX_AP_API
    slide_ring* ringslide_new(uint32 n) noexcept;

    /**
     * @brief Non-throwing slide ring allocator.
     * @param n size of sliding window in bytes
     * @return a slide ring on success, `nullptr` otherwise
     */
    TCMPLX_AP_API
    util_unique_ptr<slide_ring> ringslide_unique(uint32 n) noexcept;

    /**
     * @brief Destroy a slide ring.
     * @param x (nullable) the slide ring to destroy
     */
    TCMPLX_AP_API
    void ringslide_destroy(slide_ring* x) noexcept;
    //END   slide ring / allocation (namespace local)
    /** @} */
  };
};

#endif //hg_TextComplexAccessP_RingSlide_H_
