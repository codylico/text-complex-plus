/**
 * @file text-complex-plus/access/ringdist.hpp
 * @brief Distance ring buffer
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_RingDist_H_
#define hg_TextComplexAccessP_RingDist_H_

#include "api.hpp"
#include "util.hpp"

namespace text_complex {
  namespace access {
    /**
     * @defgroup ringdist Distance ring buffer
     *   (access/ringdist.hpp)
     * @{
     */
    //BEGIN distance ring
    /**
     * @brief Distance ring buffer.
     */
    class TCMPLX_AP_API distance_ring final {
    private:
      uint32 ring[4];
      unsigned short int i;
      unsigned short int special_size;
      unsigned short int sum_direct : 8;
      unsigned short int direct_one : 8;
      unsigned short int postfix : 4;
      unsigned short int bit_adjust : 4;
      unsigned short int postmask : 8;

    public: /** @name rule-of-six*//** @{ */
      /**
       * @brief Constructor.
       * @param special whether to support Brotli distance codes
       * @param direct number of direct codes supported (0..120)
       * @param postfix postfix bit count (0..3)
       * @throw std::bad_alloc if something breaks
       * @note For RFC 1951 semantics, use `special_tf=0`,
       *   `direct=4`, `postfix=0`.
       */
      distance_ring
        (bool special, unsigned int direct=4, unsigned int postfix=0);
      /**
       * @brief Destructor.
       */
      ~distance_ring(void);
      /**
       * @brief Copy constructor.
       */
      distance_ring(distance_ring const& ) noexcept;
      /**
       * @brief Copy assignment operator.
       * @return this distance ring
       */
      distance_ring& operator=(distance_ring const& ) noexcept;
      /**
       * @brief Move constructor.
       */
      distance_ring(distance_ring&& ) noexcept;
      /**
       * @brief Move assignment operator.
       * @return this distance ring
       */
      distance_ring& operator=(distance_ring&& ) noexcept;
      /** @} */

    public: /** @name allocation *//** @{ */
      /**
       * @brief Scalar memory allocator.
       * @param sz size in `char`s of `distance_ring` to allocate
       * @return a pointer to memory on success
       * @throw std::bad_alloc on allocation error
       */
      static void* operator new(std::size_t sz);
      /**
       * @brief Array memory allocator.
       * @param sz size in `char`s of `distance_ring[]` to allocate
       * @return a pointer to memory on success
       * @throw std::bad_alloc on allocation error
       */
      static void* operator new[](std::size_t sz);
      /**
       * @brief Scalar memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `distance_ring` to free
       */
      static void operator delete(void* p, std::size_t sz) noexcept;
      /**
       * @brief Array memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `distance_ring[]` to free
       */
      static void operator delete[](void* p, std::size_t sz) noexcept;
      /** @} */

    public: /** @name methods *//** @{ */
      /**
       * @brief Compute the number of extra bits for a distance code.
       * @param dcode distance code to check
       * @return a bit count
       * @note This function does not advance the ring buffer. It also does
       *   not check for distance code overflow.
       */
      unsigned int bit_count(unsigned int dcode) const noexcept;
      
      /**
       * @brief Convert a distance code to a flat backward distance.
       * @param dcode distance code to convert
       * @param extra any extra bits required by the code
       * @param[out] ae @em error-code api_error::Success on success,
       *   nonzero otherwise
       * @return a flat distance, or zero on conversion error
       * @note On successful conversion, the distance ring buffer is advanced
       *   as necessary.
       */
      uint32 decode
        (unsigned int dcode, uint32 extra, api_error& ae) noexcept;

      /**
       * @brief Convert a distance code to a flat backward distance.
       * @param dcode distance code to convert
       * @param extra any extra bits required by the code
       * @return a flat distance
       * @throw api_exception on code length error
       * @note On successful conversion, the distance ring buffer is advanced
       *   as necessary.
       */
      uint32 decode(unsigned int dcode, uint32 extra);

      /**
       * @brief Convert a flat backward distance to a distance code.
       * @param back_dist backward distance to convert
       * @param[out] extra any extra bits required by the code
       * @param[out] ae @em error-code api_error::Success on success,
       *   nonzero otherwise
       * @return a distance code, or
       *   `std::numeric_limits<unsigned int>::%max()` on error
       * @note On successful conversion, the distance ring buffer is advanced
       *   as necessary.
       */
      unsigned int encode
        (uint32 back_dist, uint32& extra, api_error& ae) noexcept;

      /**
       * @brief Convert a flat backward distance to a distance code.
       * @param back_dist backward distance to convert
       * @param[out] extra any extra bits required by the code
       * @return a distance code
       * @throw api_exception on code length error
       * @note On successful conversion, the distance ring buffer is advanced
       *   as necessary.
       */
      unsigned int encode(uint32 back_dist, uint32& extra);
      /** @} */

    private: /** @name rule-of-six *//** @{ */
      void duplicate(distance_ring const& ) noexcept;
      void transfer(distance_ring&& ) noexcept;
      void transfer(distance_ring const& ) = delete;
      /** @} */
    };
    //END   distance ring

    //BEGIN distance ring / exports
    template
    class TCMPLX_AP_API util_unique_ptr<distance_ring>;
    //END   distance ring / exports

    //BEGIN distance ring / allocation (namespace local)
    /**
     * @brief Non-throwing distance ring allocator.
     * @param special whether to support Brotli distance codes
     * @param direct number of direct codes supported (0..120)
     * @param postfix postfix bit count (0..3)
     * @return a distance ring on success, `nullptr` otherwise
     */
    TCMPLX_AP_API
    distance_ring* ringdist_new
      (bool special, unsigned int direct=4, unsigned int postfix=0) noexcept;

    /**
     * @brief Non-throwing distance ring allocator.
     * @param special whether to support Brotli distance codes
     * @param direct number of direct codes supported (0..120)
     * @param postfix postfix bit count (0..3)
     * @return a distance ring on success, `nullptr` otherwise
     */
    TCMPLX_AP_API
    util_unique_ptr<distance_ring> ringdist_unique
      (bool special, unsigned int direct=4, unsigned int postfix=0) noexcept;

    /**
     * @brief Destroy a distance ring.
     * @param x (nullable) the distance ring to destroy
     */
    TCMPLX_AP_API
    void ringdist_destroy(distance_ring* x) noexcept;
    //END   distance ring / allocation (namespace local)
    /** @} */
  };
};

#include "ringdist.txx"

#endif //hg_TextComplexAccessP_RingDist_H_
