/**
 * @file tests/testfont.hpp
 * @brief Test fonts.
 */
#ifndef hg_TCMPLXAP_TESTFONT_Hpp_
#define hg_TCMPLXAP_TESTFONT_Hpp_

#include <cstddef>

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

#endif /*hg_TCMPLXAP_TESTFONT_Hpp_*/
