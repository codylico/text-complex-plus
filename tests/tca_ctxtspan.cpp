/**
 * @brief Test program for context map
 */
#include "testfont.hpp"
#include "text-complex-plus/access/ctxtspan.hpp"
#include "munit-plus/munit.hpp"
#include <memory>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>


static MunitPlusResult test_ctxtspan_guess
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_ctxtspan_guess_lsb
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_ctxtspan_guess_msb
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_ctxtspan_guess_utf8
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_ctxtspan_guess_signed
    (const MunitPlusParameter params[], void* data);


static MunitPlusTest tests_ctxtspan[] = {
  {(char*)"guess", test_ctxtspan_guess,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE, nullptr},
  {(char*)"guess/lsb", test_ctxtspan_guess_lsb,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE, nullptr},
  {(char*)"guess/msb", test_ctxtspan_guess_msb,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE, nullptr},
  {(char*)"guess/utf8", test_ctxtspan_guess_utf8,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE, nullptr},
  {(char*)"guess/signed", test_ctxtspan_guess_signed,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE, nullptr},
  {nullptr, nullptr, nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,nullptr}
};

static MunitPlusSuite const suite_ctxtspan = {
  (char*)"access/ctxtspan/", tests_ctxtspan, nullptr,
      1, MUNIT_PLUS_SUITE_OPTION_NONE
};

namespace tca = text_complex::access;


MunitPlusResult test_ctxtspan_guess
  (const MunitPlusParameter params[], void* data)
{
  tca::context_score score = {};
  unsigned char buf[32] = {0};
  (void)params;
  (void)data;
  munit_plus_rand_memory(sizeof(buf), buf);
  tca::ctxtspan_guess(score, buf, sizeof(buf));
  munit_plus_assert(score.vec[0] || score.vec[1] || score.vec[2] || score.vec[3]);
  return MUNIT_PLUS_OK;
}


MunitPlusResult test_ctxtspan_guess_lsb
  (const MunitPlusParameter params[], void* data)
{
  tca::context_score score = {};
  unsigned char buf[32] = {0};
  (void)params;
  (void)data;
  munit_plus_rand_memory(sizeof(buf), buf);
  for (int i = 0; i < sizeof(buf); ++i)
    buf[i] &= 63;
  tca::ctxtspan_guess(score, buf, sizeof(buf));
  munit_plus_assert(score[tca::context_map_mode::LSB6] > score[tca::context_map_mode::MSB6]);
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_ctxtspan_guess_msb
  (const MunitPlusParameter params[], void* data)
{
  tca::context_score score = {};
  unsigned char buf[32] = {0};
  (void)params;
  (void)data;
  munit_plus_rand_memory(sizeof(buf), buf);
  for (int i = 0; i < sizeof(buf); ++i)
    buf[i] &= 252;
  tca::ctxtspan_guess(score, buf, sizeof(buf));
  munit_plus_assert(score[tca::context_map_mode::MSB6] > score[tca::context_map_mode::LSB6]);
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_ctxtspan_guess_utf8
  (const MunitPlusParameter params[], void* data)
{
  tca::context_score score = {};
  unsigned char buf[32] = {0};
  (void)params;
  (void)data;
  munit_plus_rand_memory(sizeof(buf), buf);
  for (int i = 0; i < sizeof(buf); ++i) {
    if ((buf[i] & 0x80) && (i < sizeof(buf)-1)) {
      buf[i] = (buf[i] | 0xC0) & 0xDF;
      i += 1;
      buf[i] = (buf[i] | 0x80) & 0xBF;
    } else {
      buf[i] &= 0x7F;
    }
  }
  tca::ctxtspan_guess(score, buf, sizeof(buf));
  munit_plus_assert(score[tca::context_map_mode::UTF8] > score[tca::context_map_mode::LSB6]);
  return MUNIT_PLUS_OK;
}


MunitPlusResult test_ctxtspan_guess_signed
  (const MunitPlusParameter params[], void* data)
{
  tca::context_score score = {};
  unsigned char buf[32] = {0};
  (void)params;
  (void)data;
  munit_plus_rand_memory(sizeof(buf), buf);
  bool const sign = (buf[0] & 128) != 0;
  if (sign) {
    for (int i = 1; i < sizeof(buf); ++i)
      buf[i] |= 224;
  } else {
    for (int i = 1; i < sizeof(buf); ++i)
      buf[i] &= 31;
  }
  tca::ctxtspan_guess(score, buf, sizeof(buf));
  if (sign) {
    munit_plus_assert(score[tca::context_map_mode::Signed] > score[tca::context_map_mode::UTF8]);
  }
  munit_plus_assert(score[tca::context_map_mode::Signed] > score[tca::context_map_mode::MSB6]);
  return MUNIT_PLUS_OK;
}


int main(int argc, char **argv) {
  return munit_plus_suite_main(&suite_ctxtspan, nullptr, argc, argv);
}
