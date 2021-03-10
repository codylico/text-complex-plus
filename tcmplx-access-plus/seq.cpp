/**
 * @file tcmplx-access-plus/seq.cpp
 * @brief Adapter providing sequential access to bytes from a mmaptwo
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#define TCMPLX_AP_WIN32_DLL_INTERNAL
#include "seq.hpp"
#include "mmaptwo-plus/mmaptwo.hpp"
#include <new>
#include <stdexcept>
#include <limits>

namespace text_complex {
  namespace access {
    //BEGIN sequential / rule-of-six
    sequential::sequential(mmaptwo::mmaptwo_i* xfh)
      : fh(xfh), hpage(nullptr), pos(0u), off(0u), last(0u), hptr(nullptr)
    {
      reset_sync(0u);
      return;
    }

    sequential::~sequential(void) {
      if (hpage) {
        delete hpage;
        hpage = nullptr;
      }
      return;
    }

    sequential::sequential(sequential const& other)
      : fh(nullptr), hpage(nullptr), pos(0u), off(0u), last(0u), hptr(nullptr)
    {
      duplicate(other);
      return;
    }

    sequential& sequential::operator=(sequential const& other) {
      duplicate(other);
      return *this;
    }

    sequential::sequential(sequential&& other) noexcept
      : fh(nullptr), hpage(nullptr), pos(0u), off(0u), last(0u), hptr(nullptr)
    {
      transfer(static_cast<sequential&&>(other));
      return;
    }

    sequential& sequential::operator=(sequential&& other) noexcept {
      transfer(static_cast<sequential&&>(other));
      return *this;
    }

    void sequential::duplicate(sequential const& other) {
      if (this == &other)
        return;
      fh = other.fh;
      reset_sync(other.pos);
      return;
    }

    void sequential::transfer(sequential&& other) noexcept {
      if (this == &other)
        return;
      fh = other.fh; other.fh = nullptr;
      hpage = other.hpage; other.hpage = 0u;
      pos = other.pos; other.pos = 0u;
      off = other.off; other.off = 0u;
      last = other.last; other.last = 0u;
      hptr = other.hptr; other.hptr = nullptr;
      return;
    }
    //END   sequential / rule-of-six

    //BEGIN sequential / allocation
    void* sequential::operator new(std::size_t sz) {
      return ::operator new(sz);
    }

    void* sequential::operator new[](std::size_t sz) {
      return ::operator new(sz);
    }

    void sequential::operator delete(void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    void sequential::operator delete[](void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    sequential* seq_new(mmaptwo::mmaptwo_i* xfh) noexcept {
      try {
        return new sequential(xfh);
      } catch (std::bad_alloc const& ) {
        return nullptr;
      }
    }

    util_unique_ptr<sequential> seq_unique(mmaptwo::mmaptwo_i* xfh) noexcept {
      return util_unique_ptr<sequential>(seq_new(xfh));
    }

    void seq_destroy(sequential* x) noexcept {
      if (x) {
        delete x;
      }
    }
    //END   sequential / allocation

    //BEGIN sequential / public
    size_t sequential::get_pos(void) const noexcept {
      return pos;
    }

    size_t sequential::set_pos(size_t i) noexcept {
      if (i >= off && i < last) {
        pos = i;
        return pos;
      } else {
        bool const res = reset_sync(i);
        if (res)
          return pos;
        else return std::numeric_limits<size_t>::max();
      }
    }

    int sequential::get_byte(void) noexcept {
      if (pos >= last) {
        bool const res = reset_sync(pos);
        if (!res) {
          /* acquisition error */
          return -2;
        }/* else continue; */
      }
      if (pos < last) {
        int const out = (unsigned char)(hptr[pos-off]) & 255;
        pos += 1;
        return out;
      } else /* end of stream, so */return -1;
    }

    long int sequential::seek(long int i, seq_whence whence) noexcept {
      long int l_out;
      switch (whence) {
      case seq_whence::Set:
        {
          if (i < 0)
            l_out = -2L;
          else {
            size_t const out = set_pos(static_cast<size_t>(i));
            if (out == ~(size_t)0u) {
              l_out = -2L;
            } else if (out >
                static_cast<size_t>(std::numeric_limits<long int>::max()))
            {
              l_out = -1L;
            } else l_out = (long int)out;
          }
        }break;
      case seq_whence::Cur:
      case seq_whence::End:
        {
          constexpr unsigned long int nonzero =
            static_cast<unsigned long int>(std::numeric_limits<size_t>::max());
          size_t here = (whence == seq_whence::Cur) ? pos : (
              (fh != NULL) ? fh->length() : 0u
            );
          if (i == 0) {
            /* do nothing */
          } else if (i < 0) {
            long int i_prime[2];
            /* break apart any LONG_MIN values */{
              constexpr long int branch =
                (std::numeric_limits<long int>::min()/2);
              if (i <= branch) {
                i_prime[0] = i-branch;
                i_prime[1] = branch;
              } else {
                i_prime[0] = 0;
                i_prime[1] = i;
              }
            }
            /* */{
              int j;
              for (j = 0; j < 2; ++j) {
                if (((unsigned long int)-i_prime[j]) > nonzero) {
                  /* negative overflow */
                  break;
                } else {
                  size_t const n_prime = (size_t)-i_prime[j];
                  if (here >= n_prime)
                    here -= n_prime;
                  else break;
                }
              }
              if (j < 2) {
                l_out = -2L;
                break;
              }
            }
          } else if (((unsigned long int)i) > nonzero) {
            /* positive overflow */
            l_out = -2L;
            break;
          } else {
            size_t n_prime = (size_t)i;
            if (n_prime > std::numeric_limits<size_t>::max() - here) {
              l_out = -2L;
              break;
            } else here += n_prime;
          }
          /* perform seek */{
            size_t const out = set_pos(static_cast<size_t>(here));
            if (out == ~(size_t)0u) {
              l_out = -2L;
            } else if (out >
                static_cast<size_t>(std::numeric_limits<long int>::max()))
            {
              l_out = -1L;
            } else l_out = (long int)out;
          }
        }break;
      default:
        l_out = -2L;
        break;
      }
      return l_out;
    }
    //END   sequential / public

    //BEGIN sequential / private
    bool sequential::reset_sync(size_t p) {
      bool res = false;
      mmaptwo::page_i* n_hpage = nullptr;
      size_t n_pos = p;
      size_t n_off = 0u;
      size_t n_last = 0u;
      unsigned char const* n_hptr = nullptr;
      do {
        if (fh) {
          size_t const len = fh->length();
          size_t pagestart;
          size_t pagelen;
          if (n_pos > len) {
            res = false;
          } else if (n_pos == len) {
            /* repair pos */
            n_off = len;
            n_last = len;
            res = true;
          } else /* fetch the page */{
            size_t const presize = mmaptwo::get_page_size();
            size_t const pagesize = (presize < 256u ? 256u : presize);
            size_t const pagediff = n_pos%pagesize;
            pagestart = n_pos - pagediff;
            pagelen =
              ((len-pagestart<pagesize*2u) ? len-pagestart : pagesize*2u);
            n_hpage = fh->acquire(pagelen, pagestart);
            if (n_hpage) {
              n_off = pagestart;
              n_last = pagelen+pagestart;
              res = true;
            }
          }
        } else {
          if (p == 0u) {
            n_pos = 0u;
            res = true;
          } else res = false;
        }
      } while (false);
      if (res) {
        if (hpage) {
          delete hpage;
        }
        hpage = n_hpage;
        off = n_off;
        last = n_last;
        pos = n_pos;
        hptr = static_cast<unsigned char*>(
            n_hpage ? n_hpage->get() : nullptr
          );
      }
      return res;
    }
    //END   sequential / private
  };
};
