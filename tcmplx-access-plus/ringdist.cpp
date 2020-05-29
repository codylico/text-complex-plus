/**
 * @file tcmplx-access-plus/ringdist.cpp
 * @brief Distance ring buffer
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#define TCMPLX_AP_WIN32_DLL_INTERNAL
#if (defined TextComplexAccessP_NO_EXCEPT)
#  undef TextComplexAccessP_NO_EXCEPT
#endif //TextComplexAccessP_NO_EXCEPT
#include "ringdist.hpp"
#include <new>
#include <cstring>

namespace text_complex {
  namespace access {
    /**
     * @brief Record a back distance in a ring buffer.
     * @param ring the buffer
     * @param i index
     * @param v the value to record
     */
    static
    void ringdist_record
      (uint32* ring, unsigned short& i, uint32 v);
    /**
     * @brief Retrieve a back distance from a ring buffer.
     * @param ring the buffer
     * @param i current buffer index
     * @param nlast position of value to retrieve (2 for second last, ...)
     * @return the recorded value
     */
    static inline
    uint32 ringdist_retrieve
      (uint32* ring, unsigned int i, unsigned int nlast);

    //BEGIN distance_ring / static
    void ringdist_record(uint32* ring, unsigned short& i, uint32 v) {
      ring[i] = v;
      i = (i+1u)%4u;
      return;
    }

    uint32 ringdist_retrieve
      (uint32* ring, unsigned int i, unsigned int nlast)
    {
      return ring[(i+4u-nlast)%4u];
    }
    //END   distance_ring / static

    //BEGIN distance_ring / rule-of-six
    distance_ring::distance_ring(bool s_tf, unsigned int d, unsigned int p)
      : i(0),
        special_size(s_tf?16u:0u),
        sum_direct(d+(s_tf?16u:0u)), direct_one(d+1u),
        postfix(p),
        bit_adjust(p+1u), postmask((1u<<p)-1u)
    {
      if (d > 120 || p > 3)
        throw api_exception(api_error::Param);
      constexpr uint32 base_ring[4] = { 4u, 11u, 15u, 16u };
      std::memcpy(ring, base_ring, 4*sizeof(uint32));
      return;
    }

    distance_ring::~distance_ring(void) {
      return;
    }

    distance_ring::distance_ring(distance_ring const& other) noexcept
      : i(0),
        special_size(0u),
        sum_direct(0u), direct_one(1u),
        postfix(0u),
        bit_adjust(1u), postmask(0u)
    {
      duplicate(other);
      return;
    }

    distance_ring& distance_ring::operator=
        (distance_ring const& other) noexcept
    {
      duplicate(other);
      return *this;
    }

    distance_ring::distance_ring(distance_ring&& other) noexcept
      : i(0),
        special_size(0u),
        sum_direct(0u), direct_one(1u),
        postfix(0u),
        bit_adjust(1u), postmask(0u)
    {
      transfer(static_cast<distance_ring&&>(other));
      return;
    }

    distance_ring& distance_ring::operator=(distance_ring&& other) noexcept {
      transfer(static_cast<distance_ring&&>(other));
      return *this;
    }

    void distance_ring::duplicate(distance_ring const& other) noexcept{
      std::memmove(ring, other.ring, sizeof(ring));
      i = other.i;
      special_size = other.special_size;
      sum_direct = other.sum_direct;
      direct_one = other.direct_one;
      postfix = other.postfix;
      bit_adjust = other.bit_adjust;
      postmask = other.postmask;
      return;
    }

    void distance_ring::transfer(distance_ring&& other) noexcept {
      std::memmove(ring, other.ring, sizeof(ring));
      i = other.i;
      special_size = other.special_size;
      sum_direct = other.sum_direct;
      direct_one = other.direct_one;
      postfix = other.postfix;
      bit_adjust = other.bit_adjust;
      postmask = other.postmask;
      return;
    }
    //END   distance_ring / rule-of-six

    //BEGIN distance_ring / allocation
    void* distance_ring::operator new(std::size_t sz) {
      return ::operator new(sz);
    }

    void* distance_ring::operator new[](std::size_t sz) {
      return ::operator new(sz);
    }

    void distance_ring::operator delete(void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    void distance_ring::operator delete[](void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    distance_ring* ringdist_new
        (bool s_tf, unsigned int d, unsigned int p) noexcept
    {
      try {
        return new distance_ring(s_tf,d,p);
      } catch (api_exception const& ) {
        return nullptr;
      } catch (std::bad_alloc const& ) {
        return nullptr;
      }
    }

    util_unique_ptr<distance_ring> ringdist_unique
        (bool s_tf, unsigned int d, unsigned int p) noexcept
    {
      return util_unique_ptr<distance_ring>(ringdist_new(s_tf,d,p));
    }

    void ringdist_destroy(distance_ring* x) noexcept {
      if (x) {
        delete x;
      }
    }
    //END   distance_ring / allocation

    //BEGIN distance_ring / public
    unsigned int distance_ring::bit_count(unsigned int dcode) const noexcept {
      if (dcode < sum_direct) {
        return 0u;
      } else return 1u + ((dcode - sum_direct)>>bit_adjust);
    }

    uint32 distance_ring::decode
      (unsigned int dcode, uint32 extra, api_error& ae) noexcept
    {
      if (dcode < this->special_size) {
        uint32 out;
        /* identify ring entry */switch (dcode) {
        case 0:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
          out = ringdist_retrieve(this->ring, this->i, 1u);
          break;
        case 1:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
          out = ringdist_retrieve(this->ring, this->i, 2u);
          break;
        case 2:
          out = ringdist_retrieve(this->ring, this->i, 3u);
          break;
        case 3:
          out = ringdist_retrieve(this->ring, this->i, 4u);
          break;
        }
        /* adjust */switch (dcode) {
        case 4:
        case 10:
          if (out > 1u) {
            out -= 1u;
          } else {
            ae = api_error::RingDistUnderflow;
            return 0u;
          } break;
        case 5:
        case 11:
          if (out < 0xFFffFFfe)
            out += 1u;
          else {
            ae = api_error::RingDistOverflow;
            return 0u;
          } break;
        case 6:
        case 12:
          if (out > 2u) {
            out -= 2u;
          } else {
            ae = api_error::RingDistUnderflow;
            return 0u;
          } break;
        case 7:
        case 13:
          if (out < 0xFFffFFfd)
            out += 2u;
          else {
            ae = api_error::RingDistOverflow;
            return 0u;
          } break;
        case 8:
        case 14:
          if (out > 3u) {
            out -= 3u;
          } else {
            ae = api_error::RingDistUnderflow;
            return 0u;
          } break;
        case 9:
        case 15:
          if (out < 0xFFffFFfc)
            out += 3u;
          else {
            ae = api_error::RingDistOverflow;
            return 0u;
          } break;
        }
        if (dcode != 0u) {
          ringdist_record(ring, i, out);
        }
        ae = api_error::Success;
        return out;
      } else if (dcode < this->sum_direct) {
        uint32 const out = (dcode - this->special_size) + 1u;
        ringdist_record(ring, i, out);
        ae = api_error::Success;
        return out;
      } else {
        unsigned int const xcode = dcode - this->sum_direct;
        unsigned int const bit_size = bit_count(dcode);
        uint32 const high = (uint32)(xcode>>(this->postfix));
        uint32 const low = (uint32)(xcode&this->postmask);
        uint32 const offset = ((2u | (high&1)) << bit_size) - 4u;
        uint32 const out =
          ((offset + extra)<<this->postfix) + low + this->direct_one;
        /* record the new flat distance */{
          ringdist_record(ring, i, out);
        }
        ae = api_error::Success;
        return out;
      }
    }
    //END   distance_ring / public
  };
};
