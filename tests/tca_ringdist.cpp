/**
 * @brief Test program for distance ring
 */
#include "../tcmplx-access-plus/ringdist.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "munit-plus/munit.hpp"
#include <memory>
#include <new>
#include "testfont.hpp"


struct test_ringdist_params {
  bool special_tf;
  unsigned int direct_count;
  unsigned int postfix_size;
  std::unique_ptr<text_complex::access::distance_ring> rd;
};

static struct { unsigned int code; unsigned int bits; }
const test_ringdist_bit_comp_1951[30] = {
    { 0, 0}, {1, 0}, {2, 0}, {3, 0},
    { 4, 1}, {5, 1}, {6, 2}, {7, 2},
    { 8, 3}, {9, 3},{10, 4},{11, 4},
    {12, 5},{13, 5},{14, 6},{15, 6},
    {16, 7},{17, 7},{18, 8},{19, 8},
    {20, 9},{21, 9},{22,10},{23,10},
    {24,11},{25,11},{26,12},{27,12},
    {28,13},{29,13}
  };


static MunitPlusResult test_ringdist_cycle
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_ringdist_1951_bit_count
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_ringdist_7932_bit_count
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_ringdist_1951_decode
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_ringdist_7932_decode
    (const MunitPlusParameter params[], void* data);
static void* test_ringdist_1951_setup
    (const MunitPlusParameter params[], void* user_data);
static void* test_ringdist_7932_setup
    (const MunitPlusParameter params[], void* user_data);
static void test_ringdist_teardown(void* fixture);


static MunitPlusTest tests_ringdist[] = {
  {(char*)"cycle", test_ringdist_cycle,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_SINGLE_ITERATION,
      nullptr},
  {(char*)"1951/bit_count", test_ringdist_1951_bit_count,
      test_ringdist_1951_setup,test_ringdist_teardown,
      MUNIT_PLUS_TEST_OPTION_NONE,nullptr},
  {(char*)"7932/bit_count", test_ringdist_7932_bit_count,
      test_ringdist_7932_setup,test_ringdist_teardown,
      MUNIT_PLUS_TEST_OPTION_NONE,nullptr},
  {(char*)"1951/decode", test_ringdist_1951_decode,
      test_ringdist_1951_setup,test_ringdist_teardown,
      MUNIT_PLUS_TEST_OPTION_NONE,nullptr},
  {(char*)"7932/decode", test_ringdist_7932_decode,
      test_ringdist_7932_setup,test_ringdist_teardown,
      MUNIT_PLUS_TEST_OPTION_TODO,nullptr},
  {nullptr, nullptr, nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,nullptr}
};

static MunitPlusSuite const suite_ringdist = {
  (char*)"access/ringdist/", tests_ringdist, nullptr,
      1, MUNIT_PLUS_SUITE_OPTION_NONE
};




MunitPlusResult test_ringdist_cycle
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::distance_ring* ptr[2];
  (void)params;
  (void)data;
  ptr[0] = text_complex::access::ringdist_new(false);
  ptr[1] = new text_complex::access::distance_ring(false);
  std::unique_ptr<text_complex::access::distance_ring> ptr2 =
      text_complex::access::ringdist_unique(false);
  munit_plus_assert_not_null(ptr[0]);
  munit_plus_assert_not_null(ptr[1]);
  munit_plus_assert_not_null(ptr2.get());
  munit_plus_assert_ptr_not_equal(ptr[0],ptr[1]);
  munit_plus_assert_ptr_not_equal(ptr[0],ptr2.get());
  text_complex::access::ringdist_destroy(ptr[0]);
  delete ptr[1];
  return MUNIT_PLUS_OK;
}

void* test_ringdist_1951_setup
    (const MunitPlusParameter params[], void* user_data)
{
  std::unique_ptr<struct test_ringdist_params> out;
  out.reset(new (std::nothrow) struct test_ringdist_params);
  if (out) {
    out->special_tf = false;
    out->direct_count = 4u;
    out->postfix_size = 0u;
    out->rd = text_complex::access::ringdist_unique
      (out->special_tf, out->direct_count, out->postfix_size);
    if (!out->rd)
      out.reset(nullptr);
  }
  return out.release();
}

void* test_ringdist_7932_setup
    (const MunitPlusParameter params[], void* user_data)
{
  std::unique_ptr<struct test_ringdist_params> out;
  out.reset(new (std::nothrow) struct test_ringdist_params);
  if (out) {
    out->special_tf = true;
    out->direct_count = testfont_rand_uint_range(0u,120u);
    out->postfix_size = testfont_rand_uint_range(0u,3u);
    out->rd = text_complex::access::ringdist_unique
      (out->special_tf, out->direct_count, out->postfix_size);
    if (!out->rd)
      out.reset(nullptr);
  }
  return out.release();
}

void test_ringdist_teardown(void* fixture) {
  struct test_ringdist_params *fixt =
    static_cast<struct test_ringdist_params*>(fixture);
  if (fixt) {
    delete fixt;
  }
  return;
}

MunitPlusResult test_ringdist_1951_bit_count
  (const MunitPlusParameter params[], void* data)
{
  struct test_ringdist_params *const fixt =
    static_cast<struct test_ringdist_params*>(data);
  text_complex::access::distance_ring* const p =
    (fixt) ? fixt->rd.get() : nullptr;
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  /* inspect */{
    size_t i;
    for (i = 0u; i < 30u; ++i) {
      munit_plus_assert_uint(
          p->bit_count(test_ringdist_bit_comp_1951[i].code),
            ==,test_ringdist_bit_comp_1951[i].bits
        );
    }
  }
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_ringdist_7932_bit_count
  (const MunitPlusParameter params[], void* data)
{
  struct test_ringdist_params *const fixt =
    static_cast<struct test_ringdist_params*>(data);
  text_complex::access::distance_ring* const p =
    (fixt) ? fixt->rd.get() : nullptr;
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  unsigned int alpha_size;
  /* recompute alphabet size */{
    alpha_size = 16 + fixt->direct_count + (48 << fixt->postfix_size);
  }
  /* iterate over the alphabet */{
    unsigned int dcode;
    unsigned int const ndirect = fixt->direct_count;
    unsigned int const npostfix = fixt->postfix_size;
    for (dcode = 0; dcode < alpha_size; ++dcode) {
      if (dcode < 16) {
        /* special code */
        munit_plus_assert_uint(p->bit_count(dcode), ==, 0);
      } else if (dcode < 16 + ndirect) {
        /* direct code */
        munit_plus_assert_uint(p->bit_count(dcode), ==, 0);
      } else {
        /* indirect code */
        unsigned int const ndistbits =
          1 + ((dcode - ndirect - 16) >> (npostfix + 1));
        munit_plus_assert_uint(p->bit_count(dcode), ==, ndistbits);
      }
    }
  }
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_ringdist_1951_decode
  (const MunitPlusParameter params[], void* data)
{
  struct test_ringdist_params *const fixt =
    static_cast<struct test_ringdist_params*>(data);
  text_complex::access::distance_ring* const p =
    (fixt) ? fixt->rd.get() : nullptr;
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  unsigned int back_dist;
  unsigned int decomposed_code;
  unsigned int decomposed_extra;
  (void)params;
  /* compose 1951 encoding of distance */{
    unsigned int back_bit_count = 0u;
    unsigned int num_bits;
    back_dist = testfont_rand_uint_range(1u,32768u);
    /* count bits in distance */{
      unsigned int bd;
      for (bd = back_dist-1u; bd > 0u; bd >>= 1u) {
        back_bit_count += 1u;
      }
    }
    /* select bit length */{
      num_bits = back_bit_count > 2u ? (back_bit_count-2u) : 0u;
    }
    /* decompose */{
      if (num_bits == 0u) {
        decomposed_code = back_dist-1u;
        decomposed_extra = 0u;
      } else {
        decomposed_code = (num_bits*2u)+2u + (
              (back_dist-1u)&(1<<(back_bit_count-2u))
            ?1u:0u);
        decomposed_extra = (back_dist-1u)&((1<<num_bits)-1u);
      }
    }
  }
  /* inspect */{
    munit_plus_assert_uint(
        p->decode(decomposed_code,decomposed_extra),
          ==,back_dist
      );
  }
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_ringdist_7932_decode
  (const MunitPlusParameter params[], void* data)
{
  struct test_ringdist_params *const fixt =
    static_cast<struct test_ringdist_params*>(data);
  text_complex::access::distance_ring* const p =
    (fixt) ? fixt->rd.get() : nullptr;
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  return MUNIT_PLUS_SKIP;
}


int main(int argc, char **argv) {
  return munit_plus_suite_main(&suite_ringdist, nullptr, argc, argv);
}
