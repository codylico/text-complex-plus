
#include "../tcmplx-access-plus/offtable.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "munit-plus/munit.hpp"
#include <memory>

#ifndef NDEBUG
#  define test_sharp2(s) #s
#  define test_sharp(s) test_sharp2(s)
#  define test_break(s) \
     { munit_error(s); return MUNIT_FAIL; }
#else
#  define test_break(s)
#endif /*NDEBUG*/

typedef int (*test_fn)(void);
static MunitPlusResult test_offtable_cycle
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_offtable_item
    (const MunitPlusParameter params[], void* data);
static void* test_offtable_setup
    (const MunitPlusParameter params[], void* user_data);
static void test_offtable_teardown(void* fixture);


static MunitPlusTest tests_offtable[] = {
  {(char*)"cycle", test_offtable_cycle,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_SINGLE_ITERATION,
      nullptr},
  {(char*)"item", test_offtable_item,
      test_offtable_setup,test_offtable_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {nullptr, nullptr, nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,nullptr}
};

static MunitPlusSuite const suite_offtable = {
  (char*)"access/offtable/", tests_offtable, nullptr,
      1, MUNIT_PLUS_SUITE_OPTION_NONE
};




MunitPlusResult test_offtable_cycle
  (const MunitPlusParameter params[], void* data)
{
  struct text_complex::access::offset_table* ptr[2];
  (void)params;
  (void)data;
  ptr[0] = text_complex::access::offtable_new(12);
  ptr[1] = new text_complex::access::offset_table(12);
  std::unique_ptr<text_complex::access::offset_table> ptr2 =
      text_complex::access::offtable_unique(12);
  munit_plus_assert_not_null(ptr[0]);
  munit_plus_assert_not_null(ptr[1]);
  munit_plus_assert_not_null(ptr[2]);
  munit_plus_assert_ptr_not_equal(ptr[0],ptr[1]);
  munit_plus_assert_ptr_not_equal(ptr[0],ptr[2]);
  text_complex::access::offtable_destroy(ptr[0]);
  delete ptr[1];
  return MUNIT_PLUS_OK;
}

void* test_offtable_setup(const MunitPlusParameter params[], void* user_data) {
  return text_complex::access::offtable_new(
      static_cast<std::size_t>(munit_plus_rand_int_range(4,256))
    );
}

void test_offtable_teardown(void* fixture) {
  text_complex::access::offtable_destroy(
      static_cast<struct text_complex::access::offset_table*>(fixture)
    );
  return;
}

MunitPlusResult test_offtable_item
  (const MunitPlusParameter params[], void* data)
{
  struct text_complex::access::offset_table* const offtable =
    (struct text_complex::access::offset_table*)data;
  struct text_complex::access::offset_table const* const offtable_c = offtable;
  if (offtable == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  (void)data;
  struct text_complex::access::offset_line const* dsp[4];
  std::size_t sz = offtable->size();
  munit_plus_assert_size(sz,>=,4);
  munit_plus_assert_size(sz,<=,256);
  std::size_t i = static_cast<std::size_t>(
        munit_plus_rand_int_range(0,static_cast<int>(sz)-1)
      );
  dsp[0] = &(*offtable)[i];
  dsp[1] = &(*offtable_c)[i];
  dsp[2] = &offtable->at(i);
  dsp[3] = &offtable_c->at(i);
  munit_plus_assert_not_null(dsp[0]);
  munit_plus_assert_ptr(dsp[0],==,dsp[1]);
  munit_plus_assert_ptr(dsp[0],==,dsp[2]);
  munit_plus_assert_ptr(dsp[0],==,dsp[3]);
  return MUNIT_PLUS_OK;
}


int main(int argc, char **argv) {
  return munit_plus_suite_main(&suite_offtable, nullptr, argc, argv);
}
