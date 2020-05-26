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
#include <limits>
#include <cerrno>
#include <cstdio>



static
bool tcmplxAtest_arg_fp_parse
  ( const MunitPlusSuite* suite, void* user_data, int* arg, int argc,
    char* const argv[]);
static
void tcmplxAtest_arg_fp_help
  ( const MunitPlusArgument* argument, void* user_data);

MunitPlusArgument const tcmplxAtest_arglist[] = {
  { (char*)"font-path", tcmplxAtest_arg_fp_parse, tcmplxAtest_arg_fp_help },
  { NULL, NULL, NULL }
};



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
  mmaptwo::page_i* acquire(std::size_t siz, std::size_t off) noexcept {
    if (off >= len
    ||  siz > (len-off))
    {
      /*throw std::out_of_range("tcmplxAtest_mmt::acquire");*/
      mmaptwo::set_errno(EDOM);
      return nullptr;
    } else {
      size_t i;
      unsigned char* data = new unsigned char[siz];
      for (i = 0; i < siz; ++i) {
        data[i] = (unsigned char)tcmplxAtest_gen_datum(gen, i+off, seed);
      }
      try {
        tcmplxAtest_mmtp* const out =
          new tcmplxAtest_mmtp(siz,off,data);
        return out;
      } catch (std::bad_alloc const& ) {
        delete[] data;
#if (defined ENOMEM)
        mmaptwo::set_errno(ENOMEM);
#else
        mmaptwo::set_errno(ERANGE);
#endif /*ENOMEM*/
        return nullptr;
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

static
int testfont_clamp_size(std::size_t z);


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

int testfont_clamp_size(std::size_t z) {
  if (z > static_cast<unsigned int>(std::numeric_limits<int>::max())) {
    return std::numeric_limits<int>::max();
  } else return static_cast<int>(z);
}

std::size_t testfont_rand_size_range(std::size_t a, std::size_t b) {
  if (a == b)
    return a;
  else return munit_plus_rand_int_range(
      testfont_clamp_size(a), testfont_clamp_size(b)
    );
}

struct MunitPlusArgument_ const* tcmplxAtest_get_args(void) {
  return tcmplxAtest_arglist;
}

bool tcmplxAtest_arg_fp_parse
  ( const MunitPlusSuite* suite, void* user_data, int* a, int argc,
    char* const argv[])
{
  int &arg = *a;
  struct tcmplxAtest_arg& tfa =
    *static_cast<struct tcmplxAtest_arg*>(user_data);
  ++arg;
  if (arg < argc) {
    try {
      tfa.font_path = argv[arg];
    } catch (std::bad_alloc const& ) {
      std::fprintf(stdout, "File name broke --font-path.\n");
      return false;
    }
    return true;
  } else {
    std::fprintf(stdout, "Missing file name for --font-path.\n");
    return false;
  }
}

void tcmplxAtest_arg_fp_help
  ( const MunitPlusArgument* argument, void* user_data)
{
  std::fprintf(stdout, " --font-path\n"
    "           Path of font file against which to test.\n");
  return;
}


tcmplxAtest_fixlist_lex::tcmplxAtest_fixlist_lex(void) noexcept
  : p(nullptr), total(0u), left(0u), prefix_len(0)
{
  return;
}

int tcmplxAtest_fixlist_lex::start(char const* s) noexcept {
  this->p = nullptr;
  this->total = 0u;
  this->left = 0u;
  this->prefix_len = -2;
  if (s == nullptr) return -1;
  else {
    char const* q;
    for (q = s; *q != '\0'; ++q) {
      char* ep;
      size_t r_count = 1u;
      unsigned long int n = std::strtoul(q, &ep, 0);
      if ((*ep) != '\0' && (*ep) != ',' && (*ep) != '*') {
        return -1;
      } else if (n >
          static_cast<unsigned long>(std::numeric_limits<int>::max()))
      {
        /* overflow, so */return -2;
      } else {
        /* this is the prefix_len */
      }
      q = ep;
      if ((*q) == '*') {
        ++q;
        unsigned long int new_r = std::strtoul(q, &ep, 0);
        if (((*ep) != '\0' && (*ep) != ',')) {
          return -1;
        } else if (new_r >
            (std::numeric_limits<size_t>::max() - this->total))
        {
          /* overflow, so */return -2;
        } else if (new_r > 0u) {
          /* this is the repeat_count */
          r_count = (size_t)new_r;
        }
        q = ep;
      }
      this->total += r_count;
      if (*q == '\0') break;
    }
  }
  this->p = s;
  return 0;
}

int tcmplxAtest_fixlist_lex::next(void) noexcept {
  if (this->prefix_len == -2 || this->left == 0u) {
    /* start */
    char const* q = this->p;
    char* ep;
    size_t r_count = 1u;
    if ((*q) == '\0')
      return -1;
    this->prefix_len = (int)std::strtoul(q, &ep, 0);
    q = ep;
    if ((*q) == '*') {
      ++q;
      unsigned long int new_r = std::strtoul(q, &ep, 0);
      if (new_r > 0u) {
        r_count = (size_t)new_r;
      }
      q = ep;
    }
    this->left = r_count;
    if (*q == ',') ++q;
    this->p = q;
  }
  this->left -= 1u;
  return this->prefix_len;
}
