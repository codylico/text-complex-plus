/**
 * @brief Test program for zcvt state
 */
#include "testfont.hpp"
#include "text-complex-plus/access/zcvt.hpp"
#include "text-complex-plus/access/zutil.hpp"
#include "munit-plus/munit.hpp"
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <cstring>


static MunitPlusResult test_zcvt_cycle
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_zcvt_item
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_zcvt_in_none
    (const MunitPlusParameter params[], void* data);
static void* test_zcvt_setup
    (const MunitPlusParameter params[], void* user_data);
static void test_zcvt_teardown(void* fixture);


static MunitPlusTest tests_zcvt[] = {
  {(char*)"cycle", test_zcvt_cycle,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_SINGLE_ITERATION,
      nullptr},
  {(char*)"item", test_zcvt_item,
      test_zcvt_setup,test_zcvt_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"in/none", test_zcvt_in_none,
      test_zcvt_setup,test_zcvt_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {nullptr, nullptr, nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,nullptr}
};

static MunitPlusSuite const suite_zcvt = {
  (char*)"access/zcvt/", tests_zcvt, nullptr,
      1, MUNIT_PLUS_SUITE_OPTION_NONE
};




MunitPlusResult test_zcvt_cycle
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::zcvt_state* ptr[2];
  (void)params;
  (void)data;
  ptr[0] = text_complex::access::zcvt_new(64,128,16);
  ptr[1] = new text_complex::access::zcvt_state(64,128,16);
  std::unique_ptr<text_complex::access::zcvt_state> ptr2 =
      text_complex::access::zcvt_unique(64,128,16);
  munit_plus_assert_not_null(ptr[0]);
  munit_plus_assert_not_null(ptr[1]);
  munit_plus_assert_not_null(ptr2.get());
  munit_plus_assert_ptr_not_equal(ptr[0],ptr[1]);
  munit_plus_assert_ptr_not_equal(ptr[0],ptr2.get());
  munit_plus_assert_ptr_not_equal(ptr[1],ptr2.get());
  text_complex::access::zcvt_destroy(ptr[0]);
  delete ptr[1];
  return MUNIT_PLUS_OK;
}

void* test_zcvt_setup(const MunitPlusParameter params[], void* user_data) {
  return text_complex::access::zcvt_new(64,128,16);
}

void test_zcvt_teardown(void* fixture) {
  text_complex::access::zcvt_destroy(
      static_cast<struct text_complex::access::zcvt_state*>(fixture)
    );
  return;
}

MunitPlusResult test_zcvt_item
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::zcvt_state* const p =
    static_cast<text_complex::access::zcvt_state*>(data);
  text_complex::access::zcvt_state const* const p_c = p;
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_zcvt_in_none
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::zcvt_state* const p =
    static_cast<text_complex::access::zcvt_state*>(data);
  unsigned char buf[128+11] = {8u,29u,1u};
  std::size_t const len =
    static_cast<std::size_t>(munit_plus_rand_int_range(0,128));
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  /* "compress" some data */{
    std::uint32_t checksum;
    unsigned char *const check_buffer = buf+7+len;
    munit_plus_rand_memory(len, static_cast<munit_plus_uint8_t*>(buf+7));
    checksum = text_complex::access::zutil_adler32(len, buf+7);
    buf[3] = (len)&0xff;
    buf[4] = (len>>8)&0xff;
    buf[5] = (~len)&0xff;
    buf[6] = ((~len)>>8)&0xff;
    check_buffer[0] = (checksum>>24)&0xff;
    check_buffer[1] = (checksum>>16)&0xff;
    check_buffer[2] = (checksum>> 8)&0xff;
    check_buffer[3] = (checksum    )&0xff;
  }
  /* extract some data */{
    std::size_t const total = len+11;
    unsigned char to_buf[128];
    unsigned char const* from_next;
    unsigned char* to_next;
    text_complex::access::api_error ae =
      text_complex::access::zcvt_in(*p, buf, buf+total, from_next,
          to_buf, to_buf+128, to_next);
    munit_plus_assert_op(ae, ==, text_complex::access::api_error::Success);
    munit_plus_assert_size(to_next-to_buf, ==, len);
    munit_plus_assert_size(from_next-buf, ==, total);
    munit_plus_assert_memory_equal(len, to_buf, buf+7);
  }
  return MUNIT_PLUS_OK;
}


int main(int argc, char **argv) {
  return munit_plus_suite_main(&suite_zcvt, nullptr, argc, argv);
}
