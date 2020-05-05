/**
 * @file tcmplx-access-plus/seq.hpp
 * @brief Adapter providing sequential access to bytes from a mmaptwo
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_Seq_H_
#define hg_TextComplexAccessP_Seq_H_

#include "api.hpp"
#include "util.hpp"

namespace mmaptwo {
  class mmaptwo_i;
  class page_i;
};

namespace text_complex {
  namespace access {
    //BEGIN sequential
    /**
     * @brief Adapter providing sequential access to bytes from a mmaptwo
     */
    class TCMPLX_AP_API sequential final {
    private:
      mmaptwo::mmaptwo_i* fh;
      mmaptwo::page_i* hpage;
      unsigned char* hptr;
      size_t pos;
      size_t off;
      size_t last;

    public /*rule-of-six*/:
      /**
       * @brief Constructor.
       * @param xfh mmaptwo instance
       * @throw `std::bad_alloc` if something breaks
       * @note Does not take control of the mmaptwo instance's lifetime.
       */
      sequential(mmaptwo::mmaptwo_i* xfh);
      /**
       * @brief Destructor.
       */
      ~sequential(void);
      /**
       * @brief Copy constructor.
       */
      sequential(sequential const& );
      /**
       * @brief Copy assignment operator.
       * @return this sequential
       */
      sequential& operator=(sequential const& );
      /**
       * @brief Move constructor.
       */
      sequential(sequential&& ) noexcept;
      /**
       * @brief Move assignment operator.
       * @return this sequential
       */
      sequential& operator=(sequential&& ) noexcept;

    public /* allocation */:
      /**
       * @brief Scalar memory allocator.
       * @param sz size in `char`s of `sequential` to allocate
       * @return a pointer to memory on success
       * @throw `std::bad_alloc` on allocation error
       */
      static void* operator new(std::size_t sz);
      /**
       * @brief Array memory allocator.
       * @param sz size in `char`s of `sequential[]` to allocate
       * @return a pointer to memory on success
       * @throw std::bad_alloc on allocation error
       */
      static void* operator new[](std::size_t sz);
      /**
       * @brief Scalar memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `sequential` to free
       */
      static void operator delete(void* p, std::size_t sz) noexcept;
      /**
       * @brief Array memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `sequential[]` to free
       */
      static void operator delete[](void* p, std::size_t sz) noexcept;

    public /* methods */:
      /**
       * @brief Query the current read position.
       * @return a read position
       */
      size_t get_pos(void) const noexcept;
      /**
       * @brief Configure the read position.
       * @param i a read position
       * @return the new position, ~0 otherwise
       */
      size_t set_pos(size_t i) noexcept;
      /**
       * @brief Read a single byte from the sequential.
       * @return the byte on success, -1 at end of stream, -2 otherwise
       */
      int get_byte(void) noexcept;

    private /* rule-of-six */:
      void duplicate(sequential const& );
      void transfer(sequential&& ) noexcept;
      void transfer(sequential const& ) = delete;

    private:
      bool reset_sync(size_t);
    };
    //END   sequential

    //BEGIN sequential / namespace local
    /**
     * @brief Non-throwing sequential allocator.
     * @param xfh mmaptwo instance
     * @return a sequential on success, `nullptr` otherwise
     * @note Does not take control of the mmaptwo instance's lifetime.
     */
    TCMPLX_AP_API
    sequential* seq_new(mmaptwo::mmaptwo_i* xfh) noexcept;

    /**
     * @brief Non-throwing sequential allocator.
     * @param xfh mmaptwo instance
     * @return a sequential on success, `nullptr` otherwise
     * @note Does not take control of the mmaptwo instance's lifetime.
     */
    TCMPLX_AP_API
    util_unique_ptr<sequential> seq_unique(mmaptwo::mmaptwo_i* xfh) noexcept;

    /**
     * @brief Destroy a sequential.
     * @param x (nullable) the sequential to destroy
     */
    TCMPLX_AP_API
    void seq_destroy(sequential* x) noexcept;
    //END   sequential / namespace local
  };
};

#include "seq.txx"
#endif //hg_TextComplexAccessP_Seq_H_
