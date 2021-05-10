/**
 * @brief Test program for hash chain
 */
#include "testfont.hpp"
#include "text-complex-plus/access/hashchain.hpp"
#include "munit-plus/munit.hpp"
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <cstring>


static MunitPlusResult test_hashchain_cycle
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_hashchain_item
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_hashchain_add
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_hashchain_addring
    (const MunitPlusParameter params[], void* data);
static void* test_hashchain_setupsmall
    (const MunitPlusParameter params[], void* user_data);
static void* test_hashchain_setup
    (const MunitPlusParameter params[], void* user_data);
static void test_hashchain_teardown(void* fixture);


static MunitPlusTest tests_hashchain[] = {
  {(char*)"cycle", test_hashchain_cycle,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"item", test_hashchain_item,
      test_hashchain_setup,test_hashchain_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"add", test_hashchain_add,
      test_hashchain_setup,test_hashchain_teardown,
      MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"add/ring", test_hashchain_addring,
      test_hashchain_setupsmall,test_hashchain_teardown,
      MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {nullptr, nullptr, nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,nullptr}
};

static MunitPlusSuite const suite_hashchain = {
  (char*)"access/hashchain/", tests_hashchain, nullptr,
      1, MUNIT_PLUS_SUITE_OPTION_NONE
};




MunitPlusResult test_hashchain_cycle
  (const MunitPlusParameter params[], void* data)
{
  uint32_t const num =
    static_cast<uint32_t>(munit_plus_rand_int_range(128,16777216));
  size_t const len =
    static_cast<size_t>(munit_plus_rand_int_range(1,128));
  text_complex::access::hash_chain* ptr[2];
  (void)params;
  (void)data;
  ptr[0] = text_complex::access::hashchain_new(num, len);
  ptr[1] = new text_complex::access::hash_chain(num, len);
  std::unique_ptr<text_complex::access::hash_chain> ptr2 =
      text_complex::access::hashchain_unique(num, len);
  munit_plus_assert_not_null(ptr[0]);
  munit_plus_assert_not_null(ptr[1]);
  munit_plus_assert_not_null(ptr2.get());
  munit_plus_assert_uint32(ptr[0]->extent(),==,num);
  munit_plus_assert_uint32(ptr[1]->extent(),==,num);
  munit_plus_assert_uint32(ptr2->extent(),==,num);
  munit_plus_assert_ptr_not_equal(ptr[0],ptr[1]);
  munit_plus_assert_ptr_not_equal(ptr[0],ptr2.get());
  munit_plus_assert_ptr_not_equal(ptr[1],ptr2.get());
  text_complex::access::hashchain_destroy(ptr[0]);
  delete ptr[1];
  return MUNIT_PLUS_OK;
}

void* test_hashchain_setup(const MunitPlusParameter params[], void* user_data) {
  uint32_t const num =
    static_cast<uint32_t>(munit_plus_rand_int_range(128,16777216));
  size_t const len =
    static_cast<size_t>(munit_plus_rand_int_range(1,128));
  return text_complex::access::hashchain_new(num, len);
}

void* test_hashchain_setupsmall
    (const MunitPlusParameter params[], void* user_data)
{
  uint32_t const num =
    static_cast<uint32_t>(munit_plus_rand_int_range(128,512));
  size_t const len =
    static_cast<size_t>(munit_plus_rand_int_range(1,16));
  return text_complex::access::hashchain_new(num, len);
}

void test_hashchain_teardown(void* fixture) {
  text_complex::access::hashchain_destroy(
      static_cast<struct text_complex::access::hash_chain*>(fixture)
    );
  return;
}

MunitPlusResult test_hashchain_item
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::hash_chain* const p =
    static_cast<text_complex::access::hash_chain*>(data);
  text_complex::access::hash_chain const* const p_c = p;
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_hashchain_add
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::hash_chain* const p =
    static_cast<text_complex::access::hash_chain*>(data);
  int const add_count = munit_plus_rand_int_range(1,64);
  unsigned char buf[64];
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  /* fill the buffer */{
    munit_plus_rand_memory(add_count, static_cast<munit_plus_uint8_t*>(buf));
  }
  /* add the items */{
    int i;
    for (i = 0; i < add_count; ++i) {
#if !(defined TextComplexAccessP_NO_EXCEPT)
      p->push_front(buf[i]);
#else
      text_complex::access::api_error ae;
      p->push_front(buf[i], ae);
      munit_plus_assert_op(ae,==,text_complex::access::api_error::Success);
#endif /*TextComplexAccessP_NO_EXCEPT*/
    }
  }
  /* check the size */{
    munit_plus_assert_uint32(p->size(), ==, add_count);
  }
  /* check the stored bytes */{
    int j;
    for (j = 0; j < add_count; ++j) {
      int const i = add_count-j-1;
#if !(defined TextComplexAccessP_NO_EXCEPT)
      munit_plus_assert_uchar(p->at(i),==,buf[j]);
#else
      munit_plus_assert_uchar((*p)[i],==,buf[j]);
#endif /*TextComplexAccessP_NO_EXCEPT*/
    }
  }
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_hashchain_addring
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::hash_chain* const p =
    static_cast<text_complex::access::hash_chain*>(data);
  int const add_count = munit_plus_rand_int_range(1,64);
  uint32_t const extent = p->extent();
  uint32_t const skip_count = extent - munit_plus_rand_int_range(1,64);
  unsigned char buf[64];
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  /* fill the buffer */{
    munit_plus_rand_memory(add_count, static_cast<munit_plus_uint8_t*>(buf));
  }
  /* skip some items */{
    uint32_t i;
    for (i = 0; i < skip_count; ++i) {
      text_complex::access::api_error ae;
      p->push_front(static_cast<unsigned int>(i)&255u, ae);
    }
  }
  /* add the items */{
    int i;
    for (i = 0; i < add_count; ++i) {
#if !(defined TextComplexAccessP_NO_EXCEPT)
      p->push_front(buf[i]);
#else
      text_complex::access::api_error ae;
      p->push_front(buf[i], ae);
      munit_plus_assert_op(ae,==,text_complex::access::api_error::Success);
#endif /*TextComplexAccessP_NO_EXCEPT*/
    }
  }
  /* check the size */{
    uint32_t const expect_size =
        (static_cast<unsigned int>(add_count+skip_count) > extent)
      ? extent : add_count+skip_count;
    munit_plus_assert_uint32(p->size(), ==, expect_size);
  }
  /* check the stored bytes */{
    int j;
    for (j = 0; j < add_count; ++j) {
      int const i = add_count-j-1;
#if !(defined TextComplexAccessP_NO_EXCEPT)
      munit_plus_assert_uchar(p->at(i),==,buf[j]);
#else
      munit_plus_assert_uchar((*p)[i],==,buf[j]);
#endif /*TextComplexAccessP_NO_EXCEPT*/
    }
  }
  return MUNIT_PLUS_OK;
}


int main(int argc, char **argv) {
  return munit_plus_suite_main(&suite_hashchain, nullptr, argc, argv);
}
