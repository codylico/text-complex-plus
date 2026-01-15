/**
 * @file text-complex-plus/access/seq.hpp
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
    /**
     * @defgroup seq Adapter providing sequential access to bytes
     *   (access/seq.hpp)
     * @{
     */
    //BEGIN sequential whence
    /**
     * @brief Sequential seek starting position.
     */
    enum struct seq_whence : int {
      /**
       * @brief Count from zero.
       */
      Set = 0,
      /**
       * @brief Count from current position.
       */
      Cur = 1,
      /**
       * @brief Count from the end.
       */
      End = 2
    };
    //END   sequential whence

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

    public: /** @name rule-of-six*//** @{ */
      /**
       * @brief Constructor.
       * @param xfh mmaptwo instance
       * @throw std::bad_alloc if something breaks
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

    public: /** @name allocation *//** @{ */
      /**
       * @brief Scalar memory allocator.
       * @param sz size in `char`s of `sequential` to allocate
       * @return a pointer to memory on success
       * @throw std::bad_alloc on allocation error
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
      /** @} */

    public: /** @name methods *//** @{ */
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
       * @brief Configure the read position.
       * @param i a read position
       * @param whence a @link seq_whence @endlink value
       * @return the new position on success, -1 if the position is too
       *   large for a `long int`, other negative value on error
       */
      long int seek(long int i, seq_whence whence) noexcept;
      /**
       * @brief Read a single byte from the sequential.
       * @return the byte on success, -1 at end of stream, -2 otherwise
       */
      int get_byte(void) noexcept;
      /** @} */

    private: /** @name rule-of-six *//** @{ */
      void duplicate(sequential const& );
      void transfer(sequential&& ) noexcept;
      void transfer(sequential const& ) = delete;
      /** @} */

    private:
      bool reset_sync(size_t);
    };
    //END   sequential

    //BEGIN sequential / exports
    template
    class TCMPLX_AP_API util_unique_ptr<sequential>;
    //END   sequential / exports

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
    /** @} */
  };
};

#if  (!(defined TextComplexAccessP_NO_IOSTREAM)) \
  && (!(defined TextComplexAccessP_NO_LOCALE))
#include <streambuf>
#include <istream>
#include <locale>

namespace text_complex {
  namespace access {
    /**
     * @addtogroup seq
     * @{
     */
    //BEGIN sequential streambuf
    /**
     * @brief Standard stream buffer backed by a sequential.
     * @tparam ch character type
     * @tparam tr character traits type
     */
    template <class ch, class tr = std::char_traits<ch> >
    class basic_sequentialbuf final : public std::basic_streambuf<ch,tr> {
    private:
      util_unique_ptr<sequential> seq;
      ch* sb_buf;
      std::size_t sb_size;
      bool seq_noconv : 1;
      bool sb_external : 1;
      unsigned int sb_error_loss : 6;
      ch sb_one;
      int seq_encode_len;
      typename tr::state_type seq_mb;
      typename tr::state_type seq_startmb;
      std::size_t seq_startpos;
      ch* sb_retellp;

    public: /** @name rule-of-six *//** @{ */
      /**
       * @brief Construct a stream buffer.
       */
      basic_sequentialbuf(void) noexcept;
      /**
       * @brief Destructor.
       */
      ~basic_sequentialbuf(void) noexcept override;
      /**
       * @brief Move constructor.
       * @param other another sequential buffer
       */
      basic_sequentialbuf(basic_sequentialbuf<ch,tr>&& other);
      /**
       * @brief Move assignment operator.
       * @param other another sequential buffer
       */
      basic_sequentialbuf<ch,tr>& operator=(basic_sequentialbuf<ch,tr>&& other);

      basic_sequentialbuf(basic_sequentialbuf<ch,tr> const& ) = delete;
      basic_sequentialbuf<ch,tr>& operator=(basic_sequentialbuf<ch,tr> const& ) =delete;
      /** @} */

    public: /** @name allocation *//** @{ */
      /**
       * @brief Scalar memory allocator.
       * @param sz size in `char`s of `basic_sequentialbuf` to allocate
       * @return a pointer to memory on success, `nullptr` otherwise
       */
      static void* operator new(std::size_t sz) noexcept;
      /**
       * @brief Array memory allocator.
       * @param sz size in `char`s of `basic_sequentialbuf[]` to allocate
       * @return a pointer to memory on success, `nullptr` otherwise
       */
      static void* operator new[](std::size_t sz) noexcept;
      /**
       * @brief Scalar memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `basic_sequentialbuf` to free
       */
      static void operator delete(void* p, std::size_t sz) noexcept;
      /**
       * @brief Array memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `basic_sequentialbuf[]` to free
       */
      static void operator delete[](void* p, std::size_t sz) noexcept;
      /** @} */

    public: /** @name streambuf-compat*//** @{ */
      /**
       * @brief Swap two sequential buffers.
       * @param other the other buffer with which to swap
       */
      void swap(basic_sequentialbuf<ch,tr>& other);
      /** @} */

    public: /** @name methods*//** @{ */
      /**
       * @brief Set the file access instance to use.
       * @param fh new instance
       * @return `this` on success, `nullptr` otherwise
       */
      basic_sequentialbuf<ch,tr>* set_mapping(mmaptwo::mmaptwo_i* fh);
      /**
       * @brief Check if this stream buffer has an active file access.
       * @return whether a file access has been set
       */
      bool is_mapped(void) const noexcept;
      /**
       * @brief Unset this object's file access instance.
       * @note Sets the failbit if unsuccessful.
       * @note Does not destroy any associated file access instance.
       */
      void unmap(void);
      /** @} */

    protected: /** @name streambuf-override*//** @{ */
      /**
       * @brief `imbue` implementation for `std::streambuf` interface.
       * @param loc locale to use
       * @see `std::streambuf::imbue`
       */
      void imbue(std::locale const& loc) override;
      /**
       * @brief `sync` implementation for `std::streambuf` interface.
       * @return zero on success, nonzero otherwise
       * @see `std::streambuf::sync`
       */
      int sync(void) override;
      /**
       * @brief `pbackfail` implementation for `std::streambuf` interface.
       * @param c the value to put back into the stream
       * @return `eof`; `basic_sequentialbuf` does not support
       *   value put back
       * @see `std::streambuf::pbackfail`
       */
      typename tr::int_type pbackfail(typename tr::int_type c) override;
      /**
       * @brief `seekpos` implementation for `std::streambuf` interface.
       * @param sp absolute seek position
       * @return the input seek position on success, -1 otherwise
       * @see `std::streambuf::seekpos`
       */
      typename tr::pos_type seekpos
          (typename tr::pos_type sp, std::ios_base::openmode) override;
      /**
       * @brief `seekoff` implementation for `std::streambuf` interface.
       * @param off relative seek position
       * @param dir seek direction
       * @return the result absolute seek position on success, -1 otherwise
       * @see `std::streambuf::seekoff`
       */
      typename tr::pos_type seekoff
          ( typename tr::off_type off, std::ios_base::seekdir dir,
            std::ios_base::openmode) override;
      /**
       * @brief `underflow` implementation for `std::streambuf` interface.
       * @return the next value from the stream on success, `eof` otherwise
       * @see `std::streambuf::underflow`
       */
      typename tr::int_type underflow(void) override;
      /**
       * @brief `setbuf` implementation for `std::streambuf` interface.
       * @param s buffer to use, or `nullptr` for unbuffered
       * @param n size of buffer in `ch` units
       * @return `this`
       * @see `std::streambuf::setbuf`
       */
      std::basic_streambuf<ch,tr>* setbuf(ch* s, std::streamsize n) override;
      /** @} */

    private:
      void transfer(basic_sequentialbuf<ch,tr> const& ) = delete;
      void transfer(basic_sequentialbuf<ch,tr>&& );
      void reset_sync(void);
      ch* sb_getbuf(void);
      void sb_dropbuf(void);
      int seq_retellg(std::size_t& pos, typename tr::state_type& state);
    };
    //END   sequential streambuf

    //BEGIN sequential streambuf / namespace local
    /**
     * @brief Specialization for use with `std::swap`.
     * @tparam ch character type
     * @tparam tr character traits type
     * @param lhs one object to swap
     * @param rhs other object to swap
     * @see `std::swap`
     */
    template <typename ch, typename tr>
    void swap
      (basic_sequentialbuf<ch,tr>& lhs, basic_sequentialbuf<ch,tr>& rhs);
    //END   sequential streambuf / namespace local

    //BEGIN sequential streambuf / type aliases
    /* TCMPLX_AP_API omitted here (see https://stackoverflow.com/a/1440948). */
    /**
     * @brief `basic_sequentialbuf` specialization for `char`
     */
    using sequentialbuf = basic_sequentialbuf<char>;
    /**
     * @brief `basic_sequentialbuf` specialization for `wchar_t`
     */
    using wsequentialbuf = basic_sequentialbuf<wchar_t>;
    //END   sequential streambuf / type aliases

    //BEGIN sequential istream
    /**
     * @brief Standard input stream backed by a sequential.
     * @tparam ch character type
     * @tparam tr character traits type
     */
    template <class ch, class tr = std::char_traits<ch> >
    class basic_isequentialstream final : public std::basic_istream<ch,tr> {
    private:
      util_unique_ptr<basic_sequentialbuf<ch,tr> > seq_rdbuf;

    public: /** @name rule-of-six*//** @{ */
      /**
       * @brief Default constructor.
       */
      basic_isequentialstream(void);
      /**
       * @brief Destructor.
       */
      ~basic_isequentialstream(void) override;
      /**
       * @brief Move constructor.
       */
      basic_isequentialstream(basic_isequentialstream<ch,tr>&& rhs);
      /**
       * @brief Move assignment operator.
       */
      basic_isequentialstream<ch,tr>& operator=
        (basic_isequentialstream<ch,tr>&& rhs);

      basic_isequentialstream(basic_isequentialstream<ch,tr> const& ) = delete;
      basic_isequentialstream<ch,tr>& operator=
        (basic_isequentialstream<ch,tr> const& ) = delete;
      /** @} */

    public: /** @name ifstream-compat*//** @{ */
      /**
       * @brief Open constructor.
       * @param fh the file access instance to use
       * @note Clears the `istream` state on success, else sets the failbit.
       */
      explicit basic_isequentialstream(mmaptwo::mmaptwo_i* fh);
      /** @} */

    public: /** @name istream-compat*//** @{ */
      /**
       * @brief Get a pointer to the stream buffer.
       * @return a pointer to the stream buffer
       */
      basic_sequentialbuf<ch,tr>* rdbuf(void) const;
      /**
       * @brief Swap two instances of `isequentialstream`.
       * @param other another `isequentialstream`
       */
      void swap(basic_isequentialstream<ch,tr>& other);
      /** @} */

    public: /** @name methods*//** @{ */
      /**
       * @brief Check if this stream buffer has an active file access.
       * @return whether a file access has been set
       */
      bool is_mapped(void) const;
      /**
       * @brief Set the file access instance to use.
       * @param fh new instance
       * @note Clears the `istream` state on success, else sets the failbit.
       */
      void set_mapping(mmaptwo::mmaptwo_i* fh);
      /**
       * @brief Unset this object's file access instance.
       * @note Sets the failbit if unsuccessful.
       * @note Does not destroy any associated file access instance.
       */
      void unmap(void);
      /** @} */

    public: /** @name allocation *//** @{ */
      /**
       * @brief Scalar memory allocator.
       * @param sz size in `char`s of `basic_isequentialstream` to allocate
       * @return a pointer to memory on success, `nullptr` otherwise
       */
      static void* operator new(std::size_t sz) noexcept;
      /**
       * @brief Array memory allocator.
       * @param sz size in `char`s of `basic_isequentialstream[]` to allocate
       * @return a pointer to memory on success, `nullptr` otherwise
       */
      static void* operator new[](std::size_t sz) noexcept;
      /**
       * @brief Scalar memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `basic_isequentialstream` to free
       */
      static void operator delete(void* p, std::size_t sz) noexcept;
      /**
       * @brief Array memory free callback.
       * @param p pointer to memory to free
       * @param sz size in `char`s of `basic_isequentialstream[]` to free
       */
      static void operator delete[](void* p, std::size_t sz) noexcept;
      /** @} */

    private:
      void transfer(basic_isequentialstream<ch,tr> const& ) = delete;
      void transfer(basic_isequentialstream<ch,tr>&& );
    };
    //END   sequential istream

    //BEGIN sequential istream / namespace local
    /**
     * @brief Specialization for use with `std::swap`.
     * @tparam ch character type
     * @tparam tr character traits type
     * @param lhs one object to swap
     * @param rhs other object to swap
     * @see `std::swap`
     */
    template <typename ch, typename tr>
    void swap
      ( basic_isequentialstream<ch,tr>& lhs,
        basic_isequentialstream<ch,tr>& rhs);
    //END   sequential istream / namespace local

    //BEGIN sequential istream / type aliases
    /* TCMPLX_AP_API omitted here (see https://stackoverflow.com/a/1440948). */
    /**
     * @brief `basic_isequentialstream` specialization for `char`
     */
    using isequentialstream = basic_isequentialstream<char>;
    /**
     * @brief `basic_isequentialstream` specialization for `wchar_t`
     */
    using wisequentialstream = basic_isequentialstream<wchar_t>;
    //END   sequential istream / type aliases
    /** @} */
  };
};
#endif //TextComplexAccessP_NO_IOSTREAM && TextComplexAccessP_NO_LOCALE

#include "seq.txx"
#endif //hg_TextComplexAccessP_Seq_H_
