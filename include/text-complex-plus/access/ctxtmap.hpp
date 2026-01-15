/**
 * @file text-complex-plus/access/ctxtmap.hpp
 * @brief Context map for compressed streams
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_CtxtMap_H_
#define hg_TextComplexAccessP_CtxtMap_H_

#include "api.hpp"
#include "util.hpp"

namespace text_complex {
  namespace access {
    /**
     * @defgroup ctxtmap Context map for compressed streams
     *   (access/ctxtmap.hpp)
     * @{
     */
    //BEGIN context map
    /**
     * @brief Literal context modes.
     */
    enum struct context_map_mode : int {
      LSB6 = 0,
      MSB6 = 1,
      UTF8 = 2,
      Signed = 3,
      ModeMax
    };

    /**
     * @brief Context map for compressed streams.
     * @note The layout for a context map is block-type major.
     */
    class TCMPLX_AP_API context_map final {
    private:
      unsigned char *p;
      unsigned char *modes;
      size_t btypes;
      size_t ctxts;

    public: /** @name rule-of-six*//** @{ */
      /**
       * @brief Constructor.
       * @param btypes number of block types
       * @param ctxts number of contexts
       * @throw std::bad_alloc if something breaks
       */
      context_map(size_t btypes = 0u, size_t ctxts = 0u);
      /**
       * @brief Destructor.
       */
      ~context_map(void);
      /**
       * @brief Copy constructor.
       */
      context_map(context_map const& );
      /**
       * @brief Copy assignment operator.
       * @return this context map
       */
      context_map& operator=(context_map const& );
      /**
       * @brief Move constructor.
       */
      context_map(context_map&& ) noexcept;
      /**
       * @brief Move assignment operator.
       * @return this context map
       */
      context_map& operator=(context_map&& ) noexcept;
      /** @} */

    public: /** @name allocation *//** @{ */
      /**
       * @brief Scalar memory allocator.
       * @param sz size in `char`s of `context_map` to allocate
       * @return a pointer to memory on success
       * @throw std::bad_alloc on allocation error
       */
      static void* operator new(std::size_t sz);
      /**
       * @brief Array memory allocator.
       * @param sz size in `char`s of `context_map[]` to allocate
       * @return a pointer to memory on success
       * @throw std::bad_alloc on allocation error
       */
      static void* operator new[](std::size_t sz);
      /**
       * @brief Scalar memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `context_map` to free
       */
      static void operator delete(void* p, std::size_t sz) noexcept;
      /**
       * @brief Array memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `context_map[]` to free
       */
      static void operator delete[](void* p, std::size_t sz) noexcept;
      /** @} */

    public: /** @name methods *//** @{ */
      /**
       * @brief Check the number of contexts in this map.
       * @return a context count
       */
      size_t contexts(void) const noexcept;
      /**
       * @brief Query the number of block type rows of the map.
       * @return the number of entries in this map
       */
      size_t block_types(void) const noexcept;
      /**
       * @brief Access the contiguous storage for this map.
       * @return a pointer to entry zero-zero in the map
       */
      unsigned char* data(void) noexcept;
      /**
       * @brief Access the contiguous storage for this map.
       * @return a pointer to entry zero-zero in the map
       */
      unsigned char const* data(void) const noexcept;
      /** @} */

      /**
       * @ingroup methods
       * @brief Inspect the context mode for a block type in a context map.
       * @param i row (block type) selector
       * @param[out] ae error code on failure
       * @return the context mode at the given coordinates
       */
      context_map_mode get_mode(std::size_t i, api_error& ae) const noexcept;
      /**
       * @ingroup methods
       * @brief Inspect the context mode for a block type in a context map.
       * @param i row (block type) selector
       * @return the context mode at the given coordinates
       */
      context_map_mode get_mode(std::size_t i) const;
      /**
       * @ingroup methods
       * @brief Modify the context mode for a block type in a context map.
       * @param i row (block type) selector
       * @param v new context mode
       * @param[out] ae error code on failure
       */
      void set_mode(std::size_t i, context_map_mode v, api_error& ae) noexcept;
      /**
       * @ingroup methods
       * @brief Modify the context mode for a block type in a context map.
       * @param i row (block type) selector
       * @param v new context mode
       */
      void set_mode(std::size_t i, context_map_mode v);

    public: /** @name matrix-compat *//** @{ */
      /**
       * @brief Array index operator.
       * @param i a block type index
       * @param j a context index
       * @return a reference to a prefix code identifier on success
       */
      unsigned char& operator()(size_t i, size_t j) noexcept;
      /**
       * @brief Array index operator.
       * @param i a block type index
       * @param j a context index
       * @return a reference to a prefix code identifier on success
       */
      unsigned char const& operator()(size_t i, size_t j) const noexcept;
      /**
       * @brief Write to a context map.
       * @param i a block type index
       * @param j a context index
       * @return a reference to a prefix code identifier on success
       * @throw std::out_of_range on bad index
       */
      unsigned char& at(size_t i, size_t j);
      /**
       * @brief Read from a context map.
       * @param i a block type index
       * @param j a context index
       * @return a reference to a prefix code identifier on success
       * @throw std::out_of_range on bad index
       */
      unsigned char const& at(size_t i, size_t j) const;
      /** @} */

    private: /** @name rule-of-six *//** @{ */
      void duplicate(context_map const& );
      void transfer(context_map&& ) noexcept;
      void transfer(context_map const& ) = delete;
      void resize(size_t b, size_t c);
      /** @} */
    };
    //END   context map

    //BEGIN context map / exports
    template
    class TCMPLX_AP_API util_unique_ptr<context_map>;
    //END   context map / exports

    //BEGIN context map / allocation (namespace local)
    /**
     * @brief Non-throwing context map allocator.
     * @return a context map on success, `nullptr` otherwise
     */
    TCMPLX_AP_API
    context_map* ctxtmap_new(size_t btypes = 0u, size_t ctxts = 0u) noexcept;

    /**
     * @brief Non-throwing context map allocator.
     * @return a context map on success, `nullptr` otherwise
     */
    TCMPLX_AP_API
    util_unique_ptr<context_map> ctxtmap_unique
        (size_t btypes = 0u, size_t ctxts = 0u) noexcept;

    /**
     * @brief Destroy a context map.
     * @param x (nullable) the context map to destroy
     */
    TCMPLX_AP_API
    void ctxtmap_destroy(context_map* x) noexcept;
    //END   context map / allocation (namespace local)

    //BEGIN context map / namespace local
    /**
     * @brief Calculate a distance context from a copy length.
     * @param copylen a copy length
     * @param[out] ae @em error-code api_error::Success on success,
     *   nonzero otherwise
     * @return a distance context on success, SIZE_MAX otherwise
     */
    TCMPLX_AP_API
    size_t ctxtmap_distance_context
        (unsigned long int copylen, api_error& ae) noexcept;

#if  (!(defined TextComplexAccessP_NO_EXCEPT))
    /**
     * @brief Calculate a distance context from a copy length.
     * @param copylen a copy length
     * @return a distance context on success
     * @throw api_exception on copy length parameter error
     */
    TCMPLX_AP_API
    size_t ctxtmap_distance_context(unsigned long int copylen);
#endif //TextComplexAccessP_NO_EXCEPT

    /**
     * @brief Calculate a literal context from recent history.
     * @param mode a context map mode
     * @param p1 most recent byte
     * @param p2 the byte before the most recent
     * @param[out] ae @em error-code api_error::Success on success,
     *   nonzero otherwise
     * @return a literal context on success, negative otherwise
     */
    TCMPLX_AP_API
    size_t ctxtmap_literal_context
      ( context_map_mode mode, unsigned char p1, unsigned char p2,
        api_error& ae) noexcept;

#if  (!(defined TextComplexAccessP_NO_EXCEPT))
    /**
     * @brief Calculate a literal context from recent history.
     * @param mode a context map mode
     * @param p1 most recent byte
     * @param p2 the byte before the most recent
     * @return a literal context on success, negative otherwise
     */
    TCMPLX_AP_API
    size_t ctxtmap_literal_context
      (context_map_mode mode, unsigned char p1, unsigned char p2);
#endif //TextComplexAccessP_NO_EXCEPT

    /**
     * @brief Apply a move-to-front transform to the map.
     * @param x the map to modify
     * @note This function intends to reverse the inverse
     *   move-to-front transform described in RFC 7932 section 7.3.
     */
    TCMPLX_AP_API
    void ctxtmap_apply_movetofront(context_map& x) noexcept;
    /**
     * @brief Apply the Brotli inverse move-to-front transform to the map.
     * @param x the map to modify
     * @see RFC 7932 section 7.3.
     */
    TCMPLX_AP_API
    void ctxtmap_revert_movetofront(context_map& x) noexcept;

    /**
     * @brief Pre-increment a context map mode.
     * @param mode variable to increment
     * @return sum
     */
    TCMPLX_AP_API
    context_map_mode operator++(context_map_mode& mode) noexcept;
    /**
     * @brief Post-increment a context map mode.
     * @param mode variable to increment
     * @return original value
     */
    TCMPLX_AP_API
    context_map_mode operator++(context_map_mode& mode, int) noexcept;
    //END   context map / namespace local
    /** @} */
  };
};

#include "ctxtmap.txx"

#endif //hg_TextComplexAccessP_CtxtMap_H_
