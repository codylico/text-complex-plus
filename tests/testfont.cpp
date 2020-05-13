/**
 * @file tests/testfont.hpp
 * @brief Test fonts.
 */
#include "testfont.hpp"
#include <climits>
#include "../mmaptwo-plus/mmaptwo.hpp"
#include <stdexcept>
#include <new>
#include "munit-plus/munit.hpp"

class tcmplxAtest_mmtp : public mmaptwo::page_i {
public:
  std::size_t const len;
  std::size_t const off;
  unsigned char* const data;
  
public:
  tcmplxAtest_mmtp(std::size_t l, std::size_t o, unsigned char* d)
    : len(l), off(o), data(d)
  {
  }
  ~tcmplxAtest_mmtp(void) {
    delete[] data;
  }
  void* get(void) noexcept { return data; }
  void const* get(void) const noexcept { return data; }
  std::size_t length(void) const noexcept { return len; }
  std::size_t offset(void) const noexcept { return off; }
};
class tcmplxAtest_mmt : public mmaptwo::mmaptwo_i {
public:
  std::size_t const len;
  int const gen;
  unsigned int const seed;

public:
  tcmplxAtest_mmt(std::size_t l, int g, unsigned int s)
    : len(
        (l>tcmplxAtest_gen_size(g) ? tcmplxAtest_gen_size(g) : l)
      ), gen(g), seed(s)
  {
  }
  mmaptwo::page_i* acquire(std::size_t siz, std::size_t off) {
    if (off >= len
    ||  siz > (len-off))
    {
      throw std::out_of_range("tcmplxAtest_mmt::acquire");
    } else {
      size_t i;
      unsigned char* data = new unsigned char[siz];
      for (i = 0; i < siz; ++i) {
        data[i] = (unsigned char)tcmplxAtest_gen_datum(gen, i+off, seed);
      }
      try {
        struct tcmplxAtest_mmtp* const out =
          new struct tcmplxAtest_mmtp(siz,off,data);
        return out;
      } catch (std::bad_alloc const& ) {
        delete[] data;
        throw;
      }
    }
    return nullptr;
  }
  size_t length(void) const noexcept {
    return len;
  }
  size_t offset(void) const noexcept {
    return 0u;
  }
};



int tcmplxAtest_gen_datum(int n, size_t i, unsigned int seed) {
  switch (n) {
  case tcmplxAtest_Zero:
    return 0u;
  case tcmplxAtest_Ascending:
    return (unsigned char)((i+seed)&255u);
  case tcmplxAtest_Descending:
    return (unsigned char)(255u-((i+seed)&255u));
  case tcmplxAtest_Pseudorandom:
    {
      unsigned int const addend = (seed&255u);
      unsigned int const factor = ((seed>>8)&255u);
      unsigned int const count = (unsigned int)(i&511u);
      unsigned int i;
#if UINT_MAX <= 65535u
      unsigned int x = 0u;
#else
      unsigned int x = seed>>16u;
#endif /*UINT_MAX*/
      for (i = 0; i < count; ++i) {
        x = (x*factor)+addend;
      }
      return (unsigned char)(x&255);
    }
  default:
    return -1;
  }
}

std::size_t tcmplxAtest_gen_size(int n) {
  switch (n) {
  case tcmplxAtest_Zero:
  case tcmplxAtest_Ascending:
  case tcmplxAtest_Descending:
  case tcmplxAtest_Pseudorandom:
    return ~(std::size_t)0u;
  default:
    return 0u;
  }
}

mmaptwo::mmaptwo_i* tcmplxAtest_gen_maptwo
    (int n, size_t maxsize, unsigned int seed)
{
  return new tcmplxAtest_mmt(maxsize,n,seed);
}

int testfont_rand_int_range(int a, int b) {
  if (a == b)
    return a;
  else return munit_plus_rand_int_range(a,b);
}
