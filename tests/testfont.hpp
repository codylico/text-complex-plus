/**
 * @file tests/testfont.hpp
 * @brief Test fonts.
 */
#ifndef hg_TCMPLXAP_TESTFONT_Hpp_
#define hg_TCMPLXAP_TESTFONT_Hpp_

#include <cstddef>
#include <string>

struct MunitPlusArgument_;
namespace mmaptwo {
  class mmaptwo_i;
};


enum tcmplxAtest_testfont {
  /**
   * @brief Bytes with value zero.
   */
  tcmplxAtest_Zero = 0,
  /**
   * @brief Bytes in ascending order.
   */
  tcmplxAtest_Ascending = 1,
  /**
   * @brief Bytes in descending order.
   */
  tcmplxAtest_Descending = 2,
  /**
   * @brief Bytes in pseudo-random order.
   */
  tcmplxAtest_Pseudorandom = 3,
  tcmplxAtest_MAX
};

struct tcmplxAtest_arg {
  std::string font_path;
};

class tcmplxAtest_fixlist_lex {
private:
  char const* p;
  std::size_t total;
  std::size_t left;
  int prefix_len;
public:
  tcmplxAtest_fixlist_lex(void) noexcept;
  /**
   * @brief Start a code string parsing.
   * @param s the string to parse
   * @return zero on success, other nonzero on parse error
   */
  int start(char const* s) noexcept;
  /**
   * @brief Acquire the next code length from a string.
   * @return a length, or -1 at end of parser
   */
  int next(void) noexcept;
  std::size_t size(void) const noexcept;
};

inline
std::size_t tcmplxAtest_fixlist_lex::size(void) const noexcept {
  return total;
}


/**
 * @brief Generate some data.
 * @param n @link tcmplxAtest_testfont @endlink
 * @param i array index
 * @param seed random seed
 * @return the numeric value, or -1 at end of file
 */
int tcmplxAtest_gen_datum(int n, std::size_t i, unsigned int seed);

/**
 * @brief Query the maximum length of a data source.
 * @param n @link tcmplxAtest_testfont @endlink
 * @return a length, or ~0 if unbounded
 */
std::size_t tcmplxAtest_gen_size(int n);

/**
 * @brief Generate a memory-backed mapper.
 * @param n @link tcmplxAtest_testfont @endlink
 * @param maxsize maximum length of the mapper
 * @return the mapper on success, `nullptr` otherwise
 */
mmaptwo::mmaptwo_i* tcmplxAtest_gen_maptwo
    (int n, std::size_t maxsize, unsigned int seed);

/**
 * @brief Like `munit_plus_rand_int_range`, but protects against
 *   empty intervals.
 * @param a min
 * @param b max
 * @return a number
 */
int testfont_rand_int_range(int a, int b);
/**
 * @brief Like `munit_plus_rand_int_range`, but protects against
 *   empty intervals.
 * @param a min
 * @param b max
 * @return a number
 */
std::size_t testfont_rand_size_range(std::size_t a, std::size_t b);
/**
 * @brief Get an argument list for munit-plus.
 * @return the argument list
 */
struct MunitPlusArgument_ const* tcmplxAtest_get_args(void);

#endif /*hg_TCMPLXAP_TESTFONT_Hpp_*/
