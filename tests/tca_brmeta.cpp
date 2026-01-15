/**
 * @brief Test program for Brotli metadata storage.
 */
#include "testfont.hpp"
#include "text-complex-plus/access/brmeta.hpp"
#include "text-complex-plus/access/blockbuf.hpp"
#include "munit-plus/munit.hpp"
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace tca = text_complex::access;

static MunitPlusResult test_brmeta_cycle
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_brmeta_item
    (const MunitPlusParameter params[], void* data);
static void* test_brmeta_setup
    (const MunitPlusParameter params[], void* user_data);
static void test_brmeta_teardown(void* fixture);


static MunitPlusTest tests_brmeta[] = {
  {(char*)"cycle", test_brmeta_cycle,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,nullptr},
  {(char*)"item", test_brmeta_item,
      test_brmeta_setup,test_brmeta_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {nullptr, nullptr, nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,nullptr}
};

static MunitPlusSuite const suite_brmeta = {
  (char*)"access/brmeta/", tests_brmeta, nullptr,
      1, MUNIT_PLUS_SUITE_OPTION_NONE
};




MunitPlusResult test_brmeta_cycle
  (const MunitPlusParameter params[], void* data)
{
  unsigned int reserve = munit_plus_rand_int_range(0,15);
  tca::brotli_meta* ptr[2];
  (void)params;
  (void)data;
  ptr[0] = tca::brmeta_new(reserve);
  ptr[1] = new tca::brotli_meta(reserve);
  std::unique_ptr<tca::brotli_meta> ptr2 =
      text_complex::access::brmeta_unique(reserve);
  munit_plus_assert_not_null(ptr[0]);
  munit_plus_assert_not_null(ptr[1]);
  munit_plus_assert_not_null(ptr2.get());
  munit_plus_assert_ptr_not_equal(ptr[0],ptr[1]);
  munit_plus_assert_ptr_not_equal(ptr[0],ptr2.get());
  text_complex::access::brmeta_destroy(ptr[0]);
  delete ptr[1];
  return MUNIT_PLUS_OK;
}

void* test_brmeta_setup(const MunitPlusParameter params[], void* user_data) {
  return tca::brmeta_new(static_cast<size_t>(
    munit_plus_rand_int_range(0,16)));
}

void test_brmeta_teardown(void* fixture) {
  tca::brmeta_destroy(
      static_cast<tca::brotli_meta*>(fixture)
    );
  return;
}

MunitPlusResult test_brmeta_item
  (const MunitPlusParameter params[], void* data)
{
  tca::brotli_meta* const table = static_cast<tca::brotli_meta*>(data);
  int sz;
  int i;
  unsigned char* ptr;
  if (!table)
    return MUNIT_PLUS_SKIP;
  (void)params;
  munit_plus_assert(table->size() == 0);
  sz = munit_plus_rand_int_range(1,256);
  {
    tca::api_error res;
    table->emplace(sz, res);
    munit_plus_assert(res == tca::api_error::Success);
  }
  munit_plus_assert(table->size() == 1);
  munit_plus_assert(table->at(0).size() == sz);
  ptr = &table->at(0)[0];
  munit_plus_assert(ptr == &(*table)[0][0]);
  munit_plus_assert(ptr ==
    &static_cast<tca::brotli_meta*>(table)->at(0)[0]);
  for (i = 0; i < sz; ++i)
    ptr[i] = (unsigned char)i;
  {
    tca::api_error res;
    table->emplace(2, res);
    munit_plus_assert(res == tca::api_error::Success);
  }
  munit_plus_assert(table->at(0).size() == sz);
  munit_plus_assert(ptr == &table->at(0)[0]);
  return MUNIT_PLUS_OK;
}


int main(int argc, char **argv) {
  return munit_plus_suite_main(&suite_brmeta, nullptr, argc, argv);
}
