/**
 * @brief Test program for Brotli conversion API.
 */
#include "testfont.hpp"
#include "text-complex-plus/access/brcvt.hpp"
#include "text-complex-plus/access/brmeta.hpp"
#include "text-complex-plus/access/blockbuf.hpp"
#include "text-complex-plus/access/zutil.hpp"
#include "munit-plus/munit.hpp"
#include <memory>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace tca = text_complex::access;

static MunitPlusResult test_brcvt_cycle
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_brcvt_item
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_brcvt_metadata_ptr
  (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_brcvt_metadata_cycle
  (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_brcvt_zsrtostr_none
  (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_brcvt_flush
  (const MunitPlusParameter params[], void* data);
static void* test_brcvt_setup
    (const MunitPlusParameter params[], void* user_data);
static void test_brcvt_teardown(void* fixture);


static MunitPlusTest tests_brcvt[] = {
  {(char*)"cycle", test_brcvt_cycle,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,nullptr},
  {(char*)"item", test_brcvt_item,
      test_brcvt_setup,test_brcvt_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"metadata_ptr", test_brcvt_metadata_ptr,
    test_brcvt_setup,test_brcvt_teardown,MUNIT_PLUS_TEST_OPTION_NONE,nullptr},
  {(char*)"metadata_cycle", test_brcvt_metadata_cycle,
    test_brcvt_setup,test_brcvt_teardown,MUNIT_PLUS_TEST_OPTION_NONE,nullptr},
  {(char*)"in/none", test_brcvt_zsrtostr_none,
    test_brcvt_setup,test_brcvt_teardown,MUNIT_PLUS_TEST_OPTION_NONE,NULL},
  {(char*)"flush", test_brcvt_flush,
    test_brcvt_setup,test_brcvt_teardown,MUNIT_PLUS_TEST_OPTION_NONE,NULL},
  {nullptr, nullptr, nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,nullptr}
};

static MunitPlusSuite const suite_brcvt = {
  (char*)"access/brcvt/", tests_brcvt, nullptr,
      1, MUNIT_PLUS_SUITE_OPTION_NONE
};




MunitPlusResult test_brcvt_cycle
  (const MunitPlusParameter params[], void* data)
{
  unsigned int reserve = munit_plus_rand_int_range(0,15);
  tca::brcvt_state* ptr[2];
  (void)params;
  (void)data;
  ptr[0] = tca::brcvt_new(reserve);
  ptr[1] = new tca::brcvt_state(reserve);
  std::unique_ptr<tca::brcvt_state> ptr2 =
      text_complex::access::brcvt_unique(reserve);
  munit_plus_assert_not_null(ptr[0]);
  munit_plus_assert_not_null(ptr[1]);
  munit_plus_assert_not_null(ptr2.get());
  munit_plus_assert_ptr_not_equal(ptr[0],ptr[1]);
  munit_plus_assert_ptr_not_equal(ptr[0],ptr2.get());
  tca::brcvt_destroy(ptr[0]);
  delete ptr[1];
  return MUNIT_PLUS_OK;
}

void* test_brcvt_setup(const MunitPlusParameter params[], void* user_data) {
  return tca::brcvt_new(64,128,16);
}

void test_brcvt_teardown(void* fixture) {
  tca::brcvt_destroy(
      static_cast<tca::brcvt_state*>(fixture)
    );
  return;
}

MunitPlusResult test_brcvt_item
  (const MunitPlusParameter params[], void* data)
{
  tca::brcvt_state* const state = static_cast<tca::brcvt_state*>(data);
  (void)params;
  if (!state)
    return MUNIT_PLUS_SKIP;
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_brcvt_metadata_ptr
  (const MunitPlusParameter params[], void* data)
{
  tca::brcvt_state* const p = (tca::brcvt_state*)data;
  if (p == NULL)
    return MUNIT_PLUS_SKIP;
  (void)params;
  munit_plus_assert(&p->get_metadata() != nullptr);
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_brcvt_metadata_cycle
  (const MunitPlusParameter params[], void* data)
{
  tca::brcvt_state* const p = static_cast<tca::brcvt_state*>(data);
  std::unique_ptr<tca::brcvt_state> const q =
    tca::brcvt_unique(4096,4096,4096);
  unsigned char text[16] = {0};
  int const text_len = munit_plus_rand_int_range(1,16);
  unsigned char buf[256] = {0};
  unsigned char* buf_end = buf;
  munit_plus_rand_memory(sizeof(text), &text[0]);
  if (!p || !q) {
    return MUNIT_PLUS_SKIP;
  }
  (void)params;
  /* store */
  {
    tca::brotli_meta& meta = p->get_metadata();
    tca::api_error res = {};
    meta.emplace(text_len, res);
    munit_plus_assert(res == tca::api_error::Success);
    std::memcpy(&meta.at(0)[0], text, text_len);
  }
  /* encode */
  {
    unsigned char dummy[1] = {0};
    unsigned char const *dummy_p = dummy;
    tca::api_error res = tca::brcvt_out(*p,
      dummy, dummy+sizeof(dummy), dummy_p,
      buf, buf+sizeof(buf), buf_end);
    munit_plus_assert(res == tca::api_error::Partial);
    munit_plus_assert(buf_end <= buf+sizeof(buf));
    res = tca::brcvt_unshift(*p, buf_end, buf+sizeof(buf), buf_end);
    munit_plus_assert(res >= tca::api_error::Success);
    munit_plus_assert(buf_end <= buf+sizeof(buf));
  }
  /* decode */
  {
    unsigned char dummy[16] = {0};
    unsigned char* dummy_ptr = dummy;
    unsigned char const* buf_ptr = buf;
    tca::api_error res = tca::brcvt_in(*q, buf, buf_end, buf_ptr,
      dummy, dummy+sizeof(dummy), dummy_ptr);
    munit_plus_assert(res >= tca::api_error::Success);
  }
  /* inspect */
  {
    tca::brotli_meta const& q_meta = q->get_metadata();
    munit_plus_assert(&q_meta != nullptr);
    munit_plus_assert(q_meta.size() == 1);
    munit_plus_assert(q_meta.at(0).size() == text_len);
    munit_plus_assert_memory_equal(text_len,
      &q_meta.at(0)[0], text);
  }
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_brcvt_zsrtostr_none
  (const MunitPlusParameter params[], void* data)
{
  tca::brcvt_state* const p = static_cast<tca::brcvt_state*>(data);
  std::array<unsigned char, 128+5> buf = {{12u}};
  std::size_t const len = static_cast<size_t>(munit_plus_rand_int_range(0,128));
  if (p == NULL)
    return MUNIT_PLUS_SKIP;
  (void)params;
  /* "compress" some data */if (len == 0) {
    buf[0] = 6;
  } else {
    size_t const r = len - 1;
    unsigned char *const check_buffer = buf.data()+4+len;
    munit_plus_rand_memory(len, &buf[4]);
    buf[1] = static_cast<unsigned char>((r & 31u) << 3);
    buf[2] = static_cast<unsigned char>(r >> 5);
    buf[3] = static_cast<unsigned char>(8 + (r >> 13));
    check_buffer[0] = 3;
  }
  /* extract some data */{
    size_t const total = len > 0 ? len+5 : 1;
    std::array<unsigned char,128> to_buf = {};
    unsigned char* ret = nullptr;
    unsigned char const* src = buf.data();
    tca::api_error res;
    res = tca::brcvt_in(*p, buf.data(), buf.data()+total, src,
      to_buf.data(), to_buf.data()+sizeof(to_buf), ret);
    munit_plus_assert(res == tca::api_error::EndOfFile);
    munit_plus_assert(ret-to_buf.data() == len);
    munit_plus_assert(src-buf.data() == total);
    munit_plus_assert_memory_equal(len, to_buf.data(), buf.data()+4);
  }
  return MUNIT_PLUS_OK;
}


MunitPlusResult test_brcvt_flush
  (const MunitPlusParameter params[], void* data)
{
  tca::brcvt_state* const p = static_cast<tca::brcvt_state*>(data);
  std::unique_ptr<tca::brcvt_state> const q =
    tca::brcvt_unique(4096,4096,4096);
  unsigned char text[16] = {0};
  int const text_len = munit_plus_rand_int_range(3,16);
  int const flush_len = munit_plus_rand_int_range(1,text_len-1);
  unsigned char buf[256] = {0};
  unsigned char* buf_end = buf;
  munit_plus_rand_memory(sizeof(text), &text[0]);
  if (p == NULL || q == NULL) {
    return MUNIT_PLUS_SKIP;
  }
  (void)params;
  /* encode */
  {
    unsigned char const *text_p = text;
    tca::api_error res = tca::brcvt_out(*p,
      text, text+flush_len, text_p,
      buf, buf+sizeof(buf), buf_end);
    munit_plus_assert(res == tca::api_error::Partial);
    munit_plus_assert(buf_end <= buf+sizeof(buf));
    res = tca::brcvt_flush(*p, buf_end, buf+sizeof(buf), buf_end);
    munit_plus_assert(res >= tca::api_error::Partial);
    munit_plus_assert(buf_end <= buf+sizeof(buf));
    res = tca::brcvt_out(*p,
      text+flush_len, text+text_len, text_p,
      buf_end, buf+sizeof(buf), buf_end);
    munit_plus_assert(res == tca::api_error::Partial);
    munit_plus_assert(buf_end <= buf+sizeof(buf));
    res = tca::brcvt_unshift(*p, buf_end, buf+sizeof(buf), buf_end);
    munit_plus_assert(res >= tca::api_error::EndOfFile);
    munit_plus_assert(buf_end <= buf+sizeof(buf));
  }
  /* decode */
  {
    unsigned char dummy[16] = {0};
    unsigned char* dummy_ptr = dummy;
    unsigned char const* buf_ptr = buf;
    tca::api_error res = tca::brcvt_in(*q, buf, buf_end, buf_ptr,
      dummy, dummy+sizeof(dummy), dummy_ptr);
    munit_plus_assert(res >= tca::api_error::Success);
    munit_plus_assert(dummy_ptr-dummy == text_len);
    munit_plus_assert_memory_equal(text_len, dummy, text);
  }
  return MUNIT_PLUS_OK;
}


int main(int argc, char **argv) {
  return munit_plus_suite_main(&suite_brcvt, nullptr, argc, argv);
}
