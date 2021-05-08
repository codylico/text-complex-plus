/**
 * @brief Test program for slide ring
 */
#include "testfont.hpp"
#include "text-complex-plus/access/ringslide.hpp"
#include "munit-plus/munit.hpp"
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <cstring>


static MunitPlusResult test_ringslide_cycle
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_ringslide_item
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_ringslide_maxsize
    (const MunitPlusParameter params[], void* data);
static void* test_ringslide_setup
    (const MunitPlusParameter params[], void* user_data);
static void test_ringslide_teardown(void* fixture);


static MunitPlusTest tests_ringslide[] = {
  {(char*)"cycle", test_ringslide_cycle,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"item", test_ringslide_item,
      test_ringslide_setup,test_ringslide_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"maxsize", test_ringslide_maxsize,
      test_ringslide_setup,test_ringslide_teardown,
      MUNIT_PLUS_TEST_OPTION_SINGLE_ITERATION,
      nullptr},
  {nullptr, nullptr, nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,nullptr}
};

static MunitPlusSuite const suite_ringslide = {
  (char*)"access/ringslide/", tests_ringslide, nullptr,
      1, MUNIT_PLUS_SUITE_OPTION_NONE
};




MunitPlusResult test_ringslide_cycle
  (const MunitPlusParameter params[], void* data)
{
  uint32_t const num =
    static_cast<uint32_t>(munit_plus_rand_int_range(128,16777216));
  text_complex::access::slide_ring* ptr[2];
  (void)params;
  (void)data;
  ptr[0] = text_complex::access::ringslide_new(num);
  ptr[1] = new text_complex::access::slide_ring(num);
  std::unique_ptr<text_complex::access::slide_ring> ptr2 =
      text_complex::access::ringslide_unique(num);
  munit_plus_assert_not_null(ptr[0]);
  munit_plus_assert_not_null(ptr[1]);
  munit_plus_assert_not_null(ptr2.get());
  munit_plus_assert_uint32(ptr[0]->extent(),==,num);
  munit_plus_assert_uint32(ptr[1]->extent(),==,num);
  munit_plus_assert_uint32(ptr2->extent(),==,num);
  munit_plus_assert_ptr_not_equal(ptr[0],ptr[1]);
  munit_plus_assert_ptr_not_equal(ptr[0],ptr2.get());
  munit_plus_assert_ptr_not_equal(ptr[1],ptr2.get());
  text_complex::access::ringslide_destroy(ptr[0]);
  delete ptr[1];
  return MUNIT_PLUS_OK;
}

void* test_ringslide_setup(const MunitPlusParameter params[], void* user_data) {
  uint32_t const num =
    static_cast<uint32_t>(munit_plus_rand_int_range(128,16777216));
  return text_complex::access::ringslide_new(num);
}

void test_ringslide_teardown(void* fixture) {
  text_complex::access::ringslide_destroy(
      static_cast<struct text_complex::access::slide_ring*>(fixture)
    );
  return;
}

MunitPlusResult test_ringslide_item
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::slide_ring* const p =
    static_cast<text_complex::access::slide_ring*>(data);
  text_complex::access::slide_ring const* const p_c = p;
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_ringslide_maxsize
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::slide_ring* const p =
    static_cast<text_complex::access::slide_ring*>(data);
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  munit_plus_assert_uint32(p->max_size(), >=, 16777200);
  return MUNIT_PLUS_OK;
}


int main(int argc, char **argv) {
  return munit_plus_suite_main(&suite_ringslide, nullptr, argc, argv);
}
