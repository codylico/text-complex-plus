/**
 * @file src/hashchain.cpp
 * @brief Duplicate lookup hash chain
 */
#define TCMPLX_AP_WIN32_DLL_INTERNAL
#include "text-complex-plus/access/hashchain.hpp"
#include <new>
#include <limits>
#include <cstring>

namespace text_complex {
  namespace access {
    static
    constexpr size_t hashchain_max(void) {
      return (std::numeric_limits<size_t>::max()/sizeof(uint32))
        / 251u;
    }
    /**
     * @brief Calculate a hash.
     * @param b three bytes to use as input
     * @return a hash value
     */
    static unsigned int hashchain_hash(unsigned char const* b);
    /**
     * @brief Fetch some bytes from the past.
     * @param sr slide ring to use
     * @param[out] b three bytes to use as output
     * @param x backward distance to inspect
     */
    static void hashchain_fetch
        (slide_ring const& sr, unsigned char* b, uint32 x);

    //BEGIN HashChain / static
    unsigned int hashchain_hash(unsigned char const* b) {
      return static_cast<unsigned int>((b[0]<<6)+(b[1]<<3)+b[2])%251u;
    }

    void hashchain_fetch
        (slide_ring const& sr, unsigned char* b, uint32 x)
    {
      b[2] = sr[x];
      b[1] = sr[x+1u];
      b[0] = sr[x+2u];
      return;
    }
    //END   HashChain / static

    //BEGIN hash_chain / string-compat
    uint32 const hash_chain::npos = static_cast<uint32>(-1);
    //END   hash_chain / string-compat

    //BEGIN hash_chain / rule-of-six
    hash_chain::hash_chain(uint32 n, size_t chain_length)
      : sr(n), last_count(0u), counter(0u), chains(nullptr),
        positions(nullptr), chain_length(chain_length)
    {
      std::memset(last_bytes, 0, sizeof(unsigned char)*3u);
      if (chain_length >= hashchain_max())
        throw api_exception(api_error::Memory);
      chains = new uint32[chain_length*251u];
      positions = new size_t[251u];
      std::memset(chains, 0, sizeof(uint32)*chain_length*251u);
      std::memset(positions, 0, sizeof(size_t)*251u);
      return;
    }

    hash_chain::~hash_chain(void) {
      delete[] positions;
      positions = nullptr;
      delete[] chains;
      chains = nullptr;
      return;
    }

    hash_chain::hash_chain(hash_chain&& other) noexcept
      : sr(std::move(other.sr)), last_count(0u),
        counter(0u), chains(nullptr),
        positions(nullptr), chain_length(0u)
    {
      transfer(static_cast<hash_chain&&>(other));
      return;
    }

    hash_chain& hash_chain::operator=(hash_chain&& other) noexcept {
      sr = std::move(other.sr);
      transfer(static_cast<hash_chain&&>(other));
      return *this;
    }

    void hash_chain::transfer(hash_chain&& other) noexcept {
      last_count = util_exchange(other.last_count, 0u);
      counter = util_exchange(other.counter, 0u);
      chains = util_exchange(other.chains, nullptr);
      positions = util_exchange(other.positions, nullptr);
      chain_length = util_exchange(other.chain_length, 0u);
      /* */{
        std::memcpy(last_bytes, other.last_bytes, 3*sizeof(unsigned char));
      }
      return;
    }
    //END   hash_chain / rule-of-six

    //BEGIN hash_chain / allocation
    void* hash_chain::operator new(std::size_t sz) {
      return ::operator new(sz);
    }

    void* hash_chain::operator new[](std::size_t sz) {
      return ::operator new(sz);
    }

    void hash_chain::operator delete(void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    void hash_chain::operator delete[](void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    hash_chain* hashchain_new(uint32 n, size_t chain_length) noexcept {
      try {
        return new hash_chain(n, chain_length);
      } catch (api_exception const& ) {
        return nullptr;
      } catch (std::bad_alloc const& ) {
        return nullptr;
      }
    }

    util_unique_ptr<hash_chain> hashchain_unique
        (uint32 n, size_t chain_length) noexcept
    {
      return util_unique_ptr<hash_chain>(hashchain_new(n, chain_length));
    }

    void hashchain_destroy(hash_chain* x) noexcept {
      if (x) {
        delete x;
      }
    }
    //END   hash_chain / allocation

    //BEGIN hash_chain / container-compat
    void hash_chain::push_front(unsigned char v, api_error& ae) noexcept {
      /* rotate the bytes */{
        unsigned char* const bytes = last_bytes;
        bytes[0] = bytes[1];
        bytes[1] = bytes[2];
        bytes[2] = v;
      }
      /* add to hash table */{
        unsigned int const i = hashchain_hash(last_bytes);
        size_t const pos = positions[i];
        uint32 *const chain = chains+(i*chain_length);
        chain[pos] = counter++;
        positions[i] = (pos+1u >= chain_length ? 0u : pos+1u);
      }
      sr.push_front(v, ae);
      return;
    }

    uint32 hash_chain::size(void) const noexcept {
      return sr.size();
    }

    unsigned char const& hash_chain::operator[](uint32 i) const noexcept {
      return sr[i];
    }

    uint32 hash_chain::extent(void) const noexcept {
      return sr.extent();
    }
    //END   hash_chain / container-compat

    //BEGIN hash_chain / public
    uint32 hash_chain::find
        (unsigned char const* b, uint32 pos) const noexcept
    {
      if (sr.size() < 3u)
        return npos;
      else {
        unsigned int const i = hashchain_hash(b);
        size_t chain_i = positions[i];
        uint32 const here = counter;
        uint32 *const chain = chains+(i*chain_length);
        uint32 const size = sr.size()-2u;
        size_t j;
        for (j = 0u; j < chain_length; ++j) {
          if (chain_i == 0u)
            chain_i = chain_length;
          uint32 const y = here-chain[--chain_i]-1u;
          if (y < pos)
            continue;
          else if (y >= size)
            return npos;
          else {
            unsigned char tmp[3];
            hashchain_fetch(sr, tmp, y);
            if (std::memcmp(tmp, b, 3u*sizeof(unsigned char)) == 0) {
              return y+2u;
            }
          }
        }
        return npos;
      }
    }
    //END   hash_chain / public
  };
};
