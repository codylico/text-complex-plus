/**
 * @file tcmplx-access-plus/seq.txx
 * @brief Adapter providing sequential access to bytes from a mmaptwo
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_Seq_Txx_
#define hg_TextComplexAccessP_Seq_Txx_

#include "seq.hpp"

#if  (!(defined TextComplexAccessP_NO_IOSTREAM)) \
  && (!(defined TextComplexAccessP_NO_LOCALE))
namespace text_complex {
  namespace access {
    //BEGIN sequential streambuf / rule-of-six
    template <class ch, class tr>
    basic_sequentialbuf<ch,tr>::basic_sequentialbuf(void) noexcept
      : sb_buf(nullptr), sb_size(0u), sb_external(false), sb_error_loss(0),
        seq_startmb(), seq_startpos(0u), sb_retellp(nullptr)
    {
      imbue(this->getloc());
    }

    template <class ch, class tr>
    basic_sequentialbuf<ch,tr>::~basic_sequentialbuf(void) noexcept {
      sb_dropbuf();
    }

    template <class ch, class tr>
    basic_sequentialbuf<ch,tr>::basic_sequentialbuf
        (basic_sequentialbuf<ch,tr>&& other)
      : sb_buf(nullptr), sb_size(0u), sb_external(false), sb_error_loss(0),
        seq_startmb(), seq_startpos(0u), sb_retellp(nullptr)
    {
      transfer(static_cast<basic_sequentialbuf<ch,tr>&&>(other));
    }

    template <class ch, class tr>
    basic_sequentialbuf<ch,tr>& basic_sequentialbuf<ch,tr>::operator=
      (basic_sequentialbuf<ch,tr>&& other)
    {
      transfer(static_cast<basic_sequentialbuf<ch,tr>&&>(other));
      return *this;
    }

    template <class ch, class tr>
    void basic_sequentialbuf<ch,tr>::transfer
        (basic_sequentialbuf<ch,tr>&& other)
    {
      /* superclass transfer */{
        std::basic_streambuf<ch,tr>::operator=
          (static_cast<basic_sequentialbuf<ch,tr>&&>(other));
      }
      seq = static_cast<util_unique_ptr<sequential>&&>(other.seq);
      /* buffer transfer */{
        /* release */
        ch* const transfer_sb_buf = other.sb_buf;
        std::size_t transfer_sb_size = other.sb_size;
        std::streamsize right_sb_retell =
          other.sb_retellp ? other.sb_retellp-sb_buf : -1;
        ch transfer_sb_one = static_cast<ch&&>(other.sb_one);
        other.sb_buf = nullptr;
        other.sb_size = 0u;
        /* reset */
        sb_dropbuf();
        sb_buf = transfer_sb_buf;
        sb_size = transfer_sb_size;
        sb_one = static_cast<ch&&>(transfer_sb_one);
        sb_retellp = (right_sb_retell < 0) ? nullptr : sb_buf+right_sb_retell;
        other.sb_retellp = nullptr;
      }
      seq_noconv = other.seq_noconv;
      sb_external = other.sb_external;
      sb_error_loss = other.sb_error_loss;
      seq_encode_len = other.seq_encode_len;
      seq_mb = other.seq_mb;
      seq_startmb = other.seq_startmb;
      seq_startpos = other.seq_startpos;
      return;
    }
    //END   sequential streambuf / rule-of-six

    //BEGIN sequential streambuf / public methods
    template <class ch, class tr>
    basic_sequentialbuf<ch,tr>* basic_sequentialbuf<ch,tr>::set_mapping
        (mmaptwo::mmaptwo_i* fh)
    {
      util_unique_ptr<sequential> tmp;
      if (fh) {
        tmp = seq_unique(fh);
        if (!tmp.get()) {
          return nullptr;
        }
      } else {
        /* tmp = nullptr; */
      }
      seq.reset(tmp.release());
      reset_sync();
      return this;
    }

    template <class ch, class tr>
    bool basic_sequentialbuf<ch,tr>::is_mapped(void) const noexcept {
      return static_cast<bool>(seq);
    }

    template <class ch, class tr>
    void basic_sequentialbuf<ch,tr>::unmap(void) {
      seq.reset(nullptr);
      reset_sync();
      return;
    }
    //END   sequential streambuf / public methods

    //BEGIN sequential streambuf / streambuf-compat
    template <class ch, class tr>
    void basic_sequentialbuf<ch,tr>::swap(basic_sequentialbuf<ch,tr>& other) {
      /* swap superclass */{
        std::basic_streambuf<ch,tr>::swap(other);
      }
      /* swap sequential */{
        util_unique_ptr<sequential> tmp_seq;
        tmp_seq.reset(seq.release());
            seq.reset(other.seq.release());
                other.seq.reset(tmp_seq.release());
      }
      /* swap buffer pointer and one unit buffer */{
        ch* tmp_sb_buf;
        ch tmp_sb_one;
        std::streamsize left_sb_retell = sb_retellp ? sb_retellp-sb_buf : -1;
        std::streamsize right_sb_retell =
          other.sb_retellp ? other.sb_retellp-sb_buf : -1;
        tmp_sb_one = static_cast<ch&&>(sb_one);
        tmp_sb_buf = ((sb_buf==&sb_one) ? &tmp_sb_one : sb_buf);
            sb_one = static_cast<ch&&>(other.sb_one);
            sb_buf = ((other.sb_buf==&other.sb_one) ? &sb_one : other.sb_buf);
                other.sb_one = static_cast<ch&&>(tmp_sb_one);
                other.sb_buf = ((tmp_sb_buf==&tmp_sb_one)
                                ? &other.sb_one
                                : tmp_sb_buf);
        sb_retellp = (right_sb_retell < 0) ? nullptr : sb_buf+right_sb_retell;
        other.sb_retellp =
          (left_sb_retell < 0) ? nullptr : other.sb_buf+left_sb_retell;
      }
      /* swap buffer size */{
        std::size_t tmp_sb_size;
        tmp_sb_size = sb_size;
            sb_size = other.sb_size;
                other.sb_size = tmp_sb_size;
      }
      /* swap noconv cache */{
        bool tmp_seq_noconv;
        tmp_seq_noconv = seq_noconv;
            seq_noconv = other.seq_noconv;
                other.seq_noconv = tmp_seq_noconv;
      }
      /* swap buffer external flag */{
        bool tmp_sb_external;
        tmp_sb_external = sb_external;
            sb_external = other.sb_external;
                other.sb_external = tmp_sb_external;
      }
      /* swap buffer external flag */{
        bool tmp_sb_error_loss;
        tmp_sb_error_loss = sb_error_loss;
            sb_error_loss = other.sb_error_loss;
                other.sb_error_loss = tmp_sb_error_loss;
      }
      /* swap encode_len cache */{
        int tmp_seq_encode_len;
        tmp_seq_encode_len = seq_encode_len;
            seq_encode_len = other.seq_encode_len;
                other.seq_encode_len = tmp_seq_encode_len;
      }
      /* swap multibyte conversion state */{
        std::mbstate_t tmp_seq_mb;
        tmp_seq_mb = seq_mb;
            seq_mb = other.seq_mb;
                other.seq_mb = tmp_seq_mb;
      }
      /* swap multibyte conversion state */{
        std::mbstate_t tmp_seq_startmb;
        tmp_seq_startmb = seq_startmb;
            seq_startmb = other.seq_startmb;
                other.seq_startmb = tmp_seq_startmb;
      }
      /* swap get buffer starting position */{
        std::size_t tmp_seq_startpos;
        tmp_seq_startpos = seq_startpos;
            seq_startpos = other.seq_startpos;
                other.seq_startpos = tmp_seq_startpos;
      }
      return;
    }
    //END   sequential streambuf / streambuf-compat

    //BEGIN sequential streambuf / allocation
    template <class ch, class tr>
    void* basic_sequentialbuf<ch,tr>::operator new(std::size_t sz) noexcept {
      return util_op_new(sz);
    }

    template <class ch, class tr>
    void* basic_sequentialbuf<ch,tr>::operator new[](std::size_t sz) noexcept {
      return util_op_new(sz);
    }

    template <class ch, class tr>
    void basic_sequentialbuf<ch,tr>::operator delete
      (void* p, std::size_t sz) noexcept
    {
      util_op_delete(p);
      return;
    }

    template <class ch, class tr>
    void basic_sequentialbuf<ch,tr>::operator delete[]
      (void* p, std::size_t sz) noexcept
    {
      util_op_delete(p);
      return;
    }
    //END   sequential streambuf / allocation

    //BEGIN sequential streambuf / protected streambuf-override
    template <class ch, class tr>
    int basic_sequentialbuf<ch,tr>::sync(void) {
      if (!seq) {
        /* set the empty get buffer */
        ch *const to = sb_getbuf();
        sb_retellp = to;
        sb_error_loss = 0u;
        this->setg(to,to,to);
        return 0;
      } else {
        using cvt_type = std::codecvt<ch,char,std::mbstate_t>;
        using cvt_result = typename cvt_type::result;
        int sync_res = 0;
        cvt_type const& cvt =
          std::use_facet<cvt_type>(this->getloc());
        /* convert bytes */
        std::mbstate_t mb = seq_startmb;
        ch *const to = sb_getbuf();
        ch *const to_end = to+sb_size;
        ch *to_next = to;
        unsigned int error_loss = 0u;
        if (seq->set_pos(seq_startpos) != seq_startpos) {
          return -1;
        }
        while (to_next < to_end && sync_res == 0) {
          int inch = seq->get_byte();
          if (inch == -1)
            break;
          else if (inch < -1) {
            sync_res = -1;
          } else {
            char one = static_cast<char>(static_cast<unsigned char>(inch&255));
            char const *from = &one, *from_end = (&one)+1, *from_next = &one;
            ch *const current_to = to_next;
            cvt_result const res =
              cvt.in(mb,from,from_end,from_next,current_to,to_end,to_next);
            switch (res) {
            case cvt_type::noconv:
              (*to_next) = static_cast<ch>(inch&255);
              to_next += 1;
              break;
            case cvt_type::ok:
              break;
            case cvt_type::error:
              sync_res = -1;
              break;
            case cvt_type::partial:
              break;
            }
            error_loss = (to_next==current_to) ? (error_loss+1u) : 0u;
          }
        }/*end while*/
        seq_mb = mb;
        sb_retellp = to_next;
        sb_error_loss = error_loss;
        this->setg(to,to,to_next);
        /* ensure that valid conversions are usable */
        return (to_next > to) ? 0 : sync_res;
      }
    }

    template <class ch, class tr>
    void basic_sequentialbuf<ch,tr>::imbue(std::locale const& loc) {
      using mb_type = typename tr::state_type;
      if (std::has_facet<std::codecvt<ch,char,mb_type> >(loc)) {
        std::codecvt<ch,char,mb_type> const& facet =
          std::use_facet<std::codecvt<ch,char,mb_type> >(loc);
        seq_noconv = facet.always_noconv();
        seq_encode_len = facet.encoding();
      } else {
        seq_noconv = true;
        seq_encode_len = 1;
      }
      /* zero-initialize */{
        seq_mb = mb_type();
      }
    }

    template <class ch, class tr>
    typename tr::int_type basic_sequentialbuf<ch,tr>::pbackfail
        (typename tr::int_type c)
    {
      return tr::eof();
    }

    template <class ch, class tr>
    typename tr::pos_type basic_sequentialbuf<ch,tr>::seekpos
        (typename tr::pos_type sp, std::ios_base::openmode)
    {
      using seek_type = typename tr::pos_type;
      using offset_type = typename tr::off_type;
      offset_type const sp_loc = static_cast<offset_type>(sp);
      if (!seq) {
        if (sp_loc == 0) /* special zero case (tcmplxA_seq-compat) */{
          ch *const to = sb_getbuf();
          seq_startpos = 0u;
          seq_startmb = sp.state();
          sb_retellp = to;
          sb_error_loss = 0u;
          this->setg(to,to,to);
          return sp;
        } else return seek_type(offset_type(-1));
      } else if (sp_loc < 0
      ||  sp_loc >= static_cast<std::streamsize>(util_ssize_max()))
      {
        return seek_type(offset_type(-1));
      } else {
        constexpr size_t nonzero = ~(size_t)0u;
        size_t sz = seq->set_pos(static_cast<size_t>(sp));
        if (sz == nonzero) {
          return seek_type(offset_type(-1));
        } else {
          ch *const to = sb_getbuf();
          seq_startpos = sz;
          seq_startmb = sp.state();
          sb_retellp = to;
          sb_error_loss = 0u;
          this->setg(to,to,to);
          return sp;
        }
      }
    }

    template <class ch, class tr>
    typename tr::pos_type basic_sequentialbuf<ch,tr>::seekoff
        ( typename tr::off_type off, std::ios_base::seekdir dir,
          std::ios_base::openmode)
    {
      using seek_type = typename tr::pos_type;
      using offset_type = typename tr::off_type;
      using mb_type = typename tr::state_type;
      if (!seq) {
        if (off == 0) {
          ch *const to = sb_getbuf();
          sb_retellp = to;
          sb_error_loss = 0u;
          this->setg(to,to,to);
          return seek_type(offset_type(0));
        } else return seek_type(offset_type(-1));
      } else if (off == 0 && dir == std::ios_base::cur) {
        /* don't move */
        std::size_t n;
        mb_type mb;
        int const retellg_res = seq_retellg(n,mb);
        if (retellg_res == -1) {
          return seek_type(offset_type(-1));
        } else {
          seek_type out = seek_type(offset_type(n));
          out.state(mb);
          return out;
        }
      } else if (seq_encode_len <= 0 && off != 0) {
        /* do not try to determine corresponding absolute position */
        return seek_type(offset_type(-1));
      } else /* TODO see if can seek within eback()..egptr() */
      {
        long int const lmax = util_long_max();
        long int const lmin = util_long_min();
        std::streamsize const gdiff =
          (dir == std::ios_base::cur ? (sb_retellp-this->gptr()) : 0);
        std::streamsize const loss =
          static_cast<std::streamsize>(dir == std::ios_base::cur ? sb_error_loss : 0);
        if (dir != std::ios_base::cur) {
          /* continue; */
        } else if (off < lmin+gdiff) {
          /* range-check `long int` overflow */
          return seek_type(offset_type(-1));
        } else if (seq_encode_len > 0 && (
                (off-gdiff) > lmax/seq_encode_len
            ||  (off-gdiff) < lmin/seq_encode_len)
        ) {
          /* range-check `long int` overflow */
          return seek_type(offset_type(-1));
        } else if ((off-gdiff)*seq_encode_len < lmin+loss) {
          /* range-check `long int` overflow */
          return seek_type(offset_type(-1));
        }
        seq_whence whence = ((dir==std::ios_base::beg)
            ? seq_whence::Set : ((dir==std::ios_base::end)
              ? seq_whence::End : seq_whence::Cur)
            );
        long int const seek_res =
          seq->seek(
                static_cast<long int>((off-gdiff)*seq_encode_len-loss),
                whence
            );
        if (seek_res < -1) {
          return seek_type(offset_type(-1));
        } else if (seek_res == -1) {
          /* refetch position (possible race condition) */
          ch *const to = sb_getbuf();
          size_t const sgpos = seq->get_pos();
          seq_startpos = sgpos;
          seq_startmb = mb_type();
          sb_retellp = to;
          sb_error_loss = 0u;
          this->setg(to,to,to);
          return seek_type(offset_type(sgpos));
        } else {
          ch *const to = sb_getbuf();
          std::size_t seek_res_sized = static_cast<std::size_t>(seek_res);
          seq_startpos = seek_res_sized;
          seq_startmb = mb_type();
          sb_retellp = to;
          sb_error_loss = 0u;
          this->setg(to,to,to);
          return seek_type(offset_type(seek_res));
        }
      }
    }

    template <class ch, class tr>
    typename tr::int_type basic_sequentialbuf<ch,tr>::underflow(void) {
      using mb_type = typename tr::state_type;
      if (!seq) {
        return tr::eof();
      } else {
        std::size_t n;
        mb_type mb;
        int const retellg_res = seq_retellg(n,mb);
        if (retellg_res == -1) {
          return tr::eof();
        }
        seq_startpos = n;
        seq_startmb = mb;
        (void)sync();
        if (this->gptr() < this->egptr()) {
          return tr::to_int_type(this->gptr()[0]);
        } else return tr::eof();
      }
    }

    template <class ch, class tr>
    std::basic_streambuf<ch,tr>* basic_sequentialbuf<ch,tr>::setbuf
        (ch* s, std::streamsize n)
    {
      using mb_type = typename tr::state_type;
      bool external_tf;
      if ((!s) || n == 0) {
        if (&sb_one == sb_buf) {
          /* same (absence of) buffer, no change */
          return this;
        } else {
          /* unbuffered */
          s = &sb_one;
          n = 1;
          external_tf = false;
          /* [[fallthrough]] */
        }
      } else {
        if (s == sb_buf && static_cast<std::streamsize>(sb_size) == n) {
          /* same buffer, no change */
          return this;
        } else {
          /* buffered */
          external_tf = true;
          /* [[fallthrough]] */
        }
      }
      /* set the buffer */{
        std::size_t pos;
        mb_type mb;
        int const retellg_res = seq_retellg(pos, mb);
        if (retellg_res == 0) {
          seq_startpos = pos;
          seq_startmb = mb;
          sb_buf = s;
          sb_size = static_cast<std::size_t>(n);
          sb_retellp = s;
          sb_external = external_tf;
          sb_error_loss = 0u;
          this->setg(s, s, s);
        }
      }
      return /*unconditional*/ this;
    }
    //END   sequential streambuf / protected streambuf-override

    //BEGIN sequential streambuf / private methods
    template <class ch, class tr>
    void basic_sequentialbuf<ch,tr>::reset_sync(void) {
      using mb_type = typename tr::state_type;
      seq_mb = mb_type();
      seq_startmb = mb_type();
      seq_startpos = 0u;
      sync();
      return;
    }

    template <class ch, class tr>
    ch* basic_sequentialbuf<ch,tr>::sb_getbuf(void) {
      if (!sb_buf) {
        sb_buf = &sb_one;
        sb_size = 1u;
        sb_external = false;
      }
      return sb_buf;
    }

    template <class ch, class tr>
    void basic_sequentialbuf<ch,tr>::sb_dropbuf(void) {
      if (sb_buf && (!sb_external) && (sb_buf != &sb_one)) {
        delete[] sb_buf;
        sb_buf = nullptr;
      }
    }

    template <class ch, class tr>
    int basic_sequentialbuf<ch,tr>::seq_retellg
        (std::size_t& pos, typename tr::state_type& state)
    {
      using mb_type = typename tr::state_type;
      if (this->gptr() == this->eback()) {
        pos = seq_startpos;
        state = seq_startmb;
      } else if (seq_noconv) {
        pos =
          seq_startpos + static_cast<std::size_t>(this->gptr()-this->eback());
        state = seq_startmb;
      } else if (!seq) {
        seq_startpos = 0u;
        seq_startmb = mb_type();
      } else if (this->gptr() == sb_retellp) {
        pos = seq->get_pos()-sb_error_loss;
        state = seq_mb;
      } else {
        /*
         * recompute sequential position from
         * seq_startpos, seq_startmb, eback(), and gptr()
         */
        std::size_t const start = seq_startpos;
        std::size_t const count =
          static_cast<std::size_t>(this->gptr()-this->eback());
        std::size_t in_pos = 0u;
        size_t finishpos = start;
        using cvt_type = std::codecvt<ch,char,std::mbstate_t>;
        int retellg_res = 0;
        cvt_type const& cvt =
          std::use_facet<cvt_type>(this->getloc());
        mb_type mb = seq_startmb;
        unsigned int error_loss = 0u;
        if (seq->set_pos(start) != start) {
          return -1;
        } else while (in_pos < count && retellg_res == 0) {
          int inch = seq->get_byte();
          if (inch == -1)
            break;
          else if (inch < -1) {
            retellg_res = -1;
          } else {
            ch in_one;
            char one = static_cast<char>(static_cast<unsigned char>(inch&255));
            char const *from = &one, *from_end = (&one)+1;
            int const len_res =
              cvt.length(mb, from, from_end, count-in_pos);
            if (len_res < 0) {
              retellg_res = -1;
            } else {
              finishpos += 1u;
            }
            in_pos += static_cast<std::size_t>(len_res);
            error_loss = (len_res<=0) ? (error_loss+1u) : 0u;
            if (error_loss >= 63u)
              break;
          }
        }/*end while*/
        sb_retellp = this->eback()+in_pos;
        sb_error_loss = error_loss;
        if (retellg_res == 0) {
          pos = finishpos;
          state = mb;
        } else return retellg_res;
      }
      return 0;
    }
    //END   sequential streambuf / private methods

    //BEGIN sequential streambuf / namespace local
    template <typename ch, typename tr>
    void swap
      (basic_sequentialbuf<ch,tr>& lhs, basic_sequentialbuf<ch,tr>& rhs)
    {
      lhs.swap(rhs);
    }
    //END   sequential streambuf / namespace local

    //BEGIN sequential istream / rule-of-six
    template <typename ch, typename tr>
    basic_isequentialstream<ch,tr>::basic_isequentialstream(void)
      : std::basic_istream<ch,tr>(nullptr)
    {
      seq_rdbuf = util_make_unique<basic_sequentialbuf<ch,tr> >();
      std::basic_istream<ch,tr>::rdbuf(seq_rdbuf.get());
      return;
    }

    template <typename ch, typename tr>
    basic_isequentialstream<ch,tr>::~basic_isequentialstream(void) = default;

    template <typename ch, typename tr>
    basic_isequentialstream<ch,tr>::basic_isequentialstream
        (basic_isequentialstream<ch,tr>&& rhs)
      : std::basic_istream<ch,tr>(nullptr)
    {
      transfer(static_cast<basic_isequentialstream<ch,tr>&&>(rhs));
      return;
    }

    template <typename ch, typename tr>
    basic_isequentialstream<ch,tr>& basic_isequentialstream<ch,tr>::operator=
        (basic_isequentialstream<ch,tr>&& rhs)
    {
      transfer(static_cast<basic_isequentialstream<ch,tr>&&>(rhs));
      return *this;
    }

    template <typename ch, typename tr>
    void basic_isequentialstream<ch,tr>::transfer
        (basic_isequentialstream<ch,tr>&& rhs)
    {
      std::basic_istream<ch,tr>::operator=
          (static_cast<basic_isequentialstream<ch,tr>&&>(rhs));
      seq_rdbuf.reset(rhs.seq_rdbuf.release());
      return;
    }
    //END   sequential istream / rule-of-six

    //BEGIN sequential istream / istream-compat
    template <typename ch, typename tr>
    basic_sequentialbuf<ch,tr>* basic_isequentialstream<ch,tr>::rdbuf
        (void) const
    {
      return seq_rdbuf.get();
    }

    template <typename ch, typename tr>
    void basic_isequentialstream<ch,tr>::swap
        (basic_isequentialstream<ch,tr>& other)
    {
      std::basic_istream<ch,tr>::swap(other);
      rdbuf()->swap(*other.rdbuf());
      return;
    }
    //END   sequential istream / istream-compat

    //BEGIN sequential istream / ifstream-compat
    template <typename ch, typename tr>
    basic_isequentialstream<ch,tr>::basic_isequentialstream
        (mmaptwo::mmaptwo_i* fh)
      : std::basic_istream<ch,tr>(nullptr)
    {
      seq_rdbuf = util_make_unique<basic_sequentialbuf<ch,tr> >();
      std::basic_istream<ch,tr>::rdbuf(seq_rdbuf.get());
      set_mapping(fh);
      return;
    }
    //END   sequential istream / ifstream-compat

    //BEGIN sequential istream / public methods
    template <typename ch, typename tr>
    bool basic_isequentialstream<ch,tr>::is_mapped(void) const {
      return seq_rdbuf->is_mapped();
    }

    template <typename ch, typename tr>
    void basic_isequentialstream<ch,tr>::set_mapping(mmaptwo::mmaptwo_i* fh) {
      seq_rdbuf->set_mapping(fh);
    }

    template <typename ch, typename tr>
    void basic_isequentialstream<ch,tr>::unmap(void) {
      seq_rdbuf->unmap();
    }
    //END   sequential istream / public methods

    //BEGIN sequential istream / allocation
    template <typename ch, typename tr>
    void* basic_isequentialstream<ch,tr>::operator new
        (std::size_t sz) noexcept
    {
      return util_op_new(sz);
    }

    template <typename ch, typename tr>
    void* basic_isequentialstream<ch,tr>::operator new[]
        (std::size_t sz) noexcept
    {
      return util_op_new(sz);
    }

    template <typename ch, typename tr>
    void basic_isequentialstream<ch,tr>::operator delete
        (void* p, std::size_t sz) noexcept
    {
      util_op_delete(p);
      return;
    }

    template <typename ch, typename tr>
    void basic_isequentialstream<ch,tr>::operator delete[]
        (void* p, std::size_t sz) noexcept
    {
      util_op_delete(p);
      return;
    }
    //END   sequential istream / allocation

    //BEGIN sequential istream / namespace local
    template <typename ch, typename tr>
    void swap
      (basic_isequentialstream<ch,tr>& lhs, basic_isequentialstream<ch,tr>& rhs)
    {
      lhs.swap(rhs);
    }
    //END   sequential istream / namespace local
  };
};
#endif //TextComplexAccessP_NO_IOSTREAM && TextComplexAccessP_NO_LOCALE

#endif //hg_TextComplexAccessP_Seq_Txx_
