/**
 * @file tcmplx-access-plus/ctxtmap.hpp
 * @brief Context map for compressed streams
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_CtxtMap_H_
#define hg_TextComplexAccessP_CtxtMap_H_

#include "api.hpp"
#include "util.hpp"

namespace text_complex {
  namespace access {
    //BEGIN context map
    /**
     * @brief Context map for compressed streams.
     * @note The layout for a context map is block-type major.
     */
    class TCMPLX_AP_API context_map final {
    private:
      unsigned char *p;
      size_t btypes;
      size_t ctxts;

    public /*rule-of-six*/:
      /**
       * @brief Constructor.
       * @param btypes number of block types
       * @param ctxts number of contexts
       * @throw `std::bad_alloc` if something breaks
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

    public /* allocation */:
      /**
       * @brief Scalar memory allocator.
       * @param sz size in `char`s of `context_map` to allocate
       * @return a pointer to memory on success
       * @throw `std::bad_alloc` on allocation error
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

    public /* methods */:
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

    public /* matrix-compat */:
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
       * @param x the list to write
       * @param i a block type index
       * @param j a context index
       * @return a reference to a prefix code identifier on success
       * @throw std::out_of_range on bad index
       */
      unsigned char& at(size_t i, size_t j);
      /**
       * @brief Read from a context map.
       * @param x the list to read
       * @param i a block type index
       * @param j a context index
       * @return a reference to a prefix code identifier on success
       * @throw std::out_of_range on bad index
       */
      unsigned char const& at(size_t i, size_t j) const;

    private /* rule-of-six */:
      void duplicate(context_map const& );
      void transfer(context_map&& ) noexcept;
      void transfer(context_map const& ) = delete;
      void resize(size_t b, size_t c);
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
  };
};

#include "ctxtmap.txx"

#endif //hg_TextComplexAccessP_CtxtMap_H_
