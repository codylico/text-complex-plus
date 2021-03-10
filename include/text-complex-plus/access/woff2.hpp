/**
 * @file tcmplx-access-plus/woff2.hpp
 * @brief WOFF2 file utility API
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_Woff2_H_
#define hg_TextComplexAccessP_Woff2_H_

#include "api.hpp"
#include "util.hpp"

namespace mmaptwo {
  class mmaptwo_i;
};

namespace text_complex {
  namespace access {
    class offset_table;

    //BEGIN woff2 tools
    /**
     * @brief Convert a tag to a WOFF2 table type.
     * @param s the tag to convert
     * @return the table type number, or 63 if unavailable
     */
    TCMPLX_AP_API
    unsigned int woff2_tag_toi(unsigned char const* s);

    /**
     * @brief Convert a WOFF2 table type to a tag.
     * @param s the tag to convert
     * @return a tag name, or NULL if no tag name matches the given value
     * @note Only checks the 6 least significant bits.
     */
    TCMPLX_AP_API
    unsigned char const* woff2_tag_fromi(unsigned int x);
    //END   woff2 tools

    //BEGIN woff2
    TCMPLX_AP_API
    class woff2 final {
    private:
      mmaptwo::mmaptwo_i* fh;
      offset_table* offsets;

    public /*rule-of-six*/:
      /**
       * @brief Constructor.
       * @param xfh File access instance to use for the Woff2.
       * @param sane_tf Sanitize the file before processing. (Default to true)
       * @throw `std::bad_alloc` if something breaks
       */
      woff2(mmaptwo::mmaptwo_i* xfh, bool sane_tf = true);
      /**
       * @brief Destructor.
       */
      ~woff2(void);
      /**
       * @brief Copy constructor.
       */
      woff2(woff2 const& ) = delete;
      /**
       * @brief Copy assignment operator.
       * @return this woff2
       */
      woff2& operator=(woff2 const& ) = delete;
      /**
       * @brief Move constructor.
       */
      woff2(woff2&& ) noexcept;
      /**
       * @brief Move assignment operator.
       * @return this woff2
       */
      woff2& operator=(woff2&& ) noexcept;

    public /* allocation */:
      /**
       * @brief Scalar memory allocator.
       * @param sz size in `char`s of `woff2` to allocate
       * @return a pointer to memory on success
       * @throw `std::bad_alloc` on allocation error
       */
      static void* operator new(std::size_t sz);
      /**
       * @brief Array memory allocator.
       * @param sz size in `char`s of `woff2[]` to allocate
       * @return a pointer to memory on success
       * @throw std::bad_alloc on allocation error
       */
      static void* operator new[](std::size_t sz);
      /**
       * @brief Scalar memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `woff2` to free
       */
      static void operator delete(void* p, std::size_t sz) noexcept;
      /**
       * @brief Array memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `woff2[]` to free
       */
      static void operator delete[](void* p, std::size_t sz) noexcept;

    public /* methods */:
      /**
       * @brief Acquire the list of transformed offsets.
       * @return an offset table
       */
      offset_table const& get_offsets(void) const noexcept;

    private /* rule-of-six */:
      //void duplicate(woff2 const& ) = delete;
      void transfer(woff2&& ) noexcept;
      void transfer(woff2 const& ) = delete;
      void initparse(bool);
    };
    //END   woff2

    //BEGIN woff2 / exports
    template
    class TCMPLX_AP_API util_unique_ptr<woff2>;
    //END   woff2 / exports

    //BEGIN woff2 / namespace local
    /**
     * @brief Non-throwing woff2 allocator.
     * @param xfh File access instance to use for the Woff2.
     * @param sane_tf Sanitize the file before processing. (Default to true)
     * @return a woff2 on success, `nullptr` otherwise
     * @note The woff2 takes ownership of the mmaptwo instance.
     */
    TCMPLX_AP_API
    woff2* woff2_new(mmaptwo::mmaptwo_i* xfh, bool sane_tf = true) noexcept;

    /**
     * @brief Non-throwing woff2 allocator.
     * @param xfh File access instance to use for the Woff2.
     * @param sane_tf Sanitize the file before processing. (Default to true)
     * @return a woff2 on success, `nullptr` otherwise
     * @note The woff2 takes ownership of the mmaptwo instance.
     */
    TCMPLX_AP_API
    util_unique_ptr<woff2> woff2_unique
      (mmaptwo::mmaptwo_i* xfh, bool sane_tf = true) noexcept;

    /**
     * @brief Destroy a woff2.
     * @param x (nullable) the woff2 to destroy
     */
    TCMPLX_AP_API
    void woff2_destroy(woff2* x) noexcept;
    //END   woff2 / namespace local
  };
};

#endif //hg_TextComplexAccessP_Woff2_H_
