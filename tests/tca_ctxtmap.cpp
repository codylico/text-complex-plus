/**
 * @brief Test program for context map
 */
#include "../tcmplx-access-plus/ctxtmap.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "munit-plus/munit.hpp"
#include <memory>
#include "testfont.hpp"


static MunitPlusResult test_ctxtmap_cycle
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_ctxtmap_item
    (const MunitPlusParameter params[], void* data);
static void* test_ctxtmap_setup
    (const MunitPlusParameter params[], void* user_data);
static void test_ctxtmap_teardown(void* fixture);


static MunitPlusTest tests_ctxtmap[] = {
  {(char*)"cycle", test_ctxtmap_cycle,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_SINGLE_ITERATION,
      nullptr},
  {(char*)"item", test_ctxtmap_item,
      test_ctxtmap_setup,test_ctxtmap_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {nullptr, nullptr, nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,nullptr}
};

static MunitPlusSuite const suite_ctxtmap = {
  (char*)"access/ctxtmap/", tests_ctxtmap, nullptr,
      1, MUNIT_PLUS_SUITE_OPTION_NONE
};




MunitPlusResult test_ctxtmap_cycle
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::context_map* ptr[2];
  (void)params;
  (void)data;
  ptr[0] = text_complex::access::ctxtmap_new();
  ptr[1] = new text_complex::access::context_map();
  std::unique_ptr<text_complex::access::context_map> ptr2 =
      text_complex::access::ctxtmap_unique();
  munit_plus_assert_not_null(ptr[0]);
  munit_plus_assert_not_null(ptr[1]);
  munit_plus_assert_not_null(ptr2.get());
  munit_plus_assert_ptr_not_equal(ptr[0],ptr[1]);
  munit_plus_assert_ptr_not_equal(ptr[0],ptr2.get());
  munit_plus_assert_ptr_not_equal(ptr[1],ptr2.get());
  text_complex::access::ctxtmap_destroy(ptr[0]);
  delete ptr[1];
  return MUNIT_PLUS_OK;
}

void* test_ctxtmap_setup(const MunitPlusParameter params[], void* user_data) {
  return text_complex::access::ctxtmap_new(
      testfont_rand_size_range(1,256), munit_plus_rand_int_range(0,1)?4:64
    );
}

void test_ctxtmap_teardown(void* fixture) {
  text_complex::access::ctxtmap_destroy(
      static_cast<struct text_complex::access::context_map*>(fixture)
    );
  return;
}

MunitPlusResult test_ctxtmap_item
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::context_map* const p =
    static_cast<text_complex::access::context_map*>(data);
  text_complex::access::context_map const* const p_c = p;
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  /* */{
    unsigned char* data_ptr = p->data();
    std::size_t j;
    munit_plus_assert_ptr_equal(data_ptr, p_c->data());
    for (j = 0; j < p->block_types(); ++j) {
      std::size_t i;
      for (i = 0; i < p->contexts(); ++i, ++data_ptr) {
        unsigned char const x =
          static_cast<unsigned char>(testfont_rand_int_range(0,255));
        p->at(j,i) = x;
        munit_plus_assert_ptr_equal(data_ptr, &(*p_c)(j,i));
        munit_plus_assert_ptr_equal(data_ptr, &(*p)(j,i));
        munit_plus_assert_ptr_equal(data_ptr, &p_c->at(j,i));
        munit_plus_assert_ptr_equal(data_ptr, &p->at(j,i));
        munit_plus_assert_uchar(x, ==, p_c->at(j,i));
      }
    }
  }
  return MUNIT_PLUS_OK;
}


int main(int argc, char **argv) {
  return munit_plus_suite_main(&suite_ctxtmap, nullptr, argc, argv);
}
