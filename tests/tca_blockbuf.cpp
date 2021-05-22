/**
 * @brief Test program for block buffer
 */
#include "testfont.hpp"
#include "text-complex-plus/access/blockbuf.hpp"
#include "munit-plus/munit.hpp"
#include <vector>
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <cstring>


static MunitPlusResult test_blockbuf_cycle
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_blockbuf_item
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_blockbuf_gen
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_blockbuf_bypass
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_blockbuf_str_cycle
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_blockbuf_str_item
    (const MunitPlusParameter params[], void* data);
static void* test_blockbuf_setup
    (const MunitPlusParameter params[], void* user_data);
static void test_blockbuf_teardown(void* fixture);


static MunitPlusTest tests_blockbuf[] = {
  {(char*)"cycle", test_blockbuf_cycle,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_SINGLE_ITERATION,
      nullptr},
  {(char*)"item", test_blockbuf_item,
      test_blockbuf_setup,test_blockbuf_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"gen", test_blockbuf_gen,
      test_blockbuf_setup,test_blockbuf_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"bypass", test_blockbuf_bypass,
      test_blockbuf_setup,test_blockbuf_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"string/cycle", test_blockbuf_str_cycle,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"string/item", test_blockbuf_str_item,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {nullptr, nullptr, nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,nullptr}
};

static MunitPlusSuite const suite_blockbuf = {
  (char*)"access/blockbuf/", tests_blockbuf, nullptr,
      1, MUNIT_PLUS_SUITE_OPTION_NONE
};




MunitPlusResult test_blockbuf_cycle
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::block_buffer* ptr[2];
  uint32_t const block_size = munit_plus_rand_int_range(1u,128u);
  uint32_t const window_size = munit_plus_rand_int_range(128u,512u);
  size_t const chain_length =
    static_cast<size_t>(munit_plus_rand_int_range(1u,32u));
  bool const bdict_tf = munit_plus_rand_int_range(0,1)==1;
  (void)params;
  (void)data;
  ptr[0] = text_complex::access::blockbuf_new
    (block_size, window_size, chain_length, bdict_tf);
  ptr[1] = new text_complex::access::block_buffer
    (block_size, window_size, chain_length, bdict_tf);
  std::unique_ptr<text_complex::access::block_buffer> ptr2 =
      text_complex::access::blockbuf_unique
          (block_size, window_size, chain_length, bdict_tf);
  munit_plus_assert_not_null(ptr[0]);
  munit_plus_assert_not_null(ptr[1]);
  munit_plus_assert_not_null(ptr2.get());
  munit_plus_assert_ptr_not_equal(ptr[0],ptr[1]);
  munit_plus_assert_ptr_not_equal(ptr[0],ptr2.get());
  munit_plus_assert_ptr_not_equal(ptr[1],ptr2.get());
  munit_plus_assert_uint32(ptr[0]->capacity(), ==, block_size);
  munit_plus_assert_uint32(ptr[1]->capacity(), ==, block_size);
  munit_plus_assert_uint32(ptr2->capacity(), ==, block_size);
  text_complex::access::blockbuf_destroy(ptr[0]);
  delete ptr[1];
  return MUNIT_PLUS_OK;
}

void* test_blockbuf_setup(const MunitPlusParameter params[], void* user_data) {
  uint32_t const block_size = munit_plus_rand_int_range(1u,128u);
  uint32_t const window_size = munit_plus_rand_int_range(128u,512u);
  size_t const chain_length =
    static_cast<size_t>(munit_plus_rand_int_range(1u,32u));
  bool const bdict_tf = munit_plus_rand_int_range(0,1)==1;
  return text_complex::access::blockbuf_new
          (block_size, window_size, chain_length, bdict_tf);
}

void test_blockbuf_teardown(void* fixture) {
  text_complex::access::blockbuf_destroy(
      static_cast<struct text_complex::access::block_buffer*>(fixture)
    );
  return;
}

MunitPlusResult test_blockbuf_item
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::block_buffer* const p =
    static_cast<text_complex::access::block_buffer*>(data);
  std::size_t const count = testfont_rand_size_range(1, p->capacity());
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  /* add some stuff */{
    unsigned char buf[128];
    munit_plus_rand_memory(count, static_cast<munit_plus_uint8_t*>(buf));
#if !(defined TextComplexAccessP_NO_EXCEPT)
    p->write(buf, count);
#else
    text_complex::access::api_error ae;
    p->write(buf, count, ae);
    munit_plus_assert_op(ae, ==, text_complex::access::api_error::Success);
#endif /*TextComplexAccessP_NO_EXCEPT*/
    munit_plus_assert_uint32(p->input_size(), ==, count);
  }
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_blockbuf_bypass
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::block_buffer* const p =
    static_cast<text_complex::access::block_buffer*>(data);
  std::size_t const count = p->capacity();
  unsigned char buf[3];
  if (p == nullptr || count < 3)
    return MUNIT_PLUS_SKIP;
  (void)params;
  /* build the text */{
    std::size_t i;
    munit_plus_rand_memory(3, static_cast<munit_plus_uint8_t*>(buf));
    for (i = 0; i < 3; ++i) {
      buf[i] = (buf[i]&3u)|80u;
    }
  }
  /* add to slide ring */{
    size_t bypass_count;
#if !(defined TextComplexAccessP_NO_EXCEPT)
    bypass_count = p->bypass(buf, 3);
#else
    text_complex::access::api_error ae;
    bypass_count = p->bypass(buf, 3, ae);
    munit_plus_assert_op(ae, ==, text_complex::access::api_error::Success);
#endif /*TextComplexAccessP_NO_EXCEPT*/
    munit_plus_assert_uint32(p->input_size(), ==, 0);
    munit_plus_assert_size(bypass_count, ==, 3);
  }
  /* add to input */{
#if !(defined TextComplexAccessP_NO_EXCEPT)
    p->write(buf, 3);
#else
    text_complex::access::api_error ae;
    p->write(buf, 3, ae);
    munit_plus_assert_op(ae, ==, text_complex::access::api_error::Success);
#endif /*TextComplexAccessP_NO_EXCEPT*/
    munit_plus_assert_uint32(p->input_size(), ==, 3);
  }
  /* */{
#if !(defined TextComplexAccessP_NO_EXCEPT)
    p->flush();
#else
    text_complex::access::api_error ae;
    p->flush(ae);
    munit_plus_assert_op(ae, ==, text_complex::access::api_error::Success);
#endif /*TextComplexAccessP_NO_EXCEPT*/
    munit_plus_assert_uint32(p->input_size(), ==, 0);
  }
  /* inspect */{
    unsigned char const copy_cmd[] = {131u, 128u, 2u};
    text_complex::access::block_string const& output = p->str();
    munit_plus_assert_uint32(output.size(), ==, 3);
    munit_plus_assert_ptr_not_null(output.data());
    munit_plus_assert_memory_equal(3, output.data(), copy_cmd);
  }
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_blockbuf_gen
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::block_buffer* const p =
    static_cast<text_complex::access::block_buffer*>(data);
  std::size_t const count = p->capacity();
  unsigned char buf[128];
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  /* build the text */{
    std::size_t i;
    munit_plus_rand_memory(count, static_cast<munit_plus_uint8_t*>(buf));
    for (i = 0; i < count; ++i) {
      buf[i] = (buf[i]&3u)|80u;
    }
  }
  /* add to input */{
#if !(defined TextComplexAccessP_NO_EXCEPT)
    p->write(buf, count);
#else
    text_complex::access::api_error ae;
    p->write(buf, count, ae);
    munit_plus_assert_op(ae, ==, text_complex::access::api_error::Success);
#endif /*TextComplexAccessP_NO_EXCEPT*/
    munit_plus_assert_uint32(p->input_size(), ==, count);
  }
  /* */{
#if !(defined TextComplexAccessP_NO_EXCEPT)
    p->flush();
#else
    text_complex::access::api_error ae;
    p->flush(ae);
    munit_plus_assert_op(ae, ==, text_complex::access::api_error::Success);
#endif /*TextComplexAccessP_NO_EXCEPT*/
    munit_plus_assert_uint32(p->input_size(), ==, 0);
  }
  /* inspect */{
    text_complex::access::block_string const& output = p->str();
    munit_plus_assert_uint32(output.size(), >, 0);
    munit_plus_assert_ptr_not_null(output.data());
    /* parse the command buffer */{
      std::vector<unsigned char> inflated_buf;
      std::uint32_t i;
      for (i = 0; i < output.size(); ++i) {
        std::uint32_t n;
        unsigned char s80 = output[i]&0x80;
        if (output[i] & 0x40) {
          munit_plus_assert_uint32(i+2, <=, output.size());
          n = ((output[i]&0x3f)*256)+output[i+1];
          i += 2;
        } else {
          n = output[i]&0x3f;
          i += 1;
        }
        if (s80) {
          std::uint32_t backward;
          std::uint32_t x;
          munit_plus_assert_uint32(i+2, <=, output.size());
          munit_plus_assert_uchar(output[i]&0xC0, ==, 0x80)
            /* short distance */;
          backward = (output[i]&0x3f)*256 + output[i+1] + 1;
          munit_plus_assert_size(backward, <=, inflated_buf.size());
          for (x = 0; x < n; ++x) {
            inflated_buf.push_back
              (inflated_buf[inflated_buf.size()-backward]);
          }
          i += 1;
        } else {
          std::uint32_t x;
          for (x = 0; x < n; ++x, ++i) {
            munit_plus_assert_uint32(i, <, output.size());
            inflated_buf.push_back(output[i]);
          }
          i -= 1;
        }
      }
      munit_plus_assert_size(inflated_buf.size(), ==, count);
      munit_plus_assert_memory_equal(count, inflated_buf.data(), buf);
    }
  }
  /* clear */{
    p->clear_output();
    munit_plus_assert_uint32(p->str().size(), ==, 0);
  }
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_blockbuf_str_cycle
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::block_string str;
  uint32_t size = munit_plus_rand_int_range(1,99);
  (void)params;
  (void)data;
#if !(defined TextComplexAccessP_NO_EXCEPT)
  str.reserve(size);
#else
  /* */{
    text_complex::access::api_error ae;
    str.reserve(size, ae);
    munit_plus_assert_op(ae, ==, text_complex::access::api_error::Success);
  }
#endif /*TextComplexAccessP_NO_EXCEPT*/
  munit_plus_assert_ptr_not_null(str.data());
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_blockbuf_str_item
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::block_string str;
  int const size = munit_plus_rand_int_range(1,64);
  int const next_size = munit_plus_rand_int_range(1,64);
  unsigned char buf[64];
  (void)params;
  (void)data;
  /* add some items */{
    munit_plus_rand_memory(size, static_cast<munit_plus_uint8_t*>(buf));
    for (int i = 0; i < size; ++i) {
#if !(defined TextComplexAccessP_NO_EXCEPT)
      str.push_back(buf[i]);
#else
      text_complex::access::api_error ae;
      str.push_back(buf[i], ae);
      munit_plus_assert_op(ae, ==, text_complex::access::api_error::Success);
#endif /*TextComplexAccessP_NO_EXCEPT*/
    }
  }
  /* inspect */{
    munit_plus_assert_uint32(str.size(), ==, size);
    munit_plus_assert_uint32(str.capacity(), >=, size);
    munit_plus_assert_ptr_not_null(str.data());
    munit_plus_assert_memory_equal(size, str.data(), buf);
    munit_plus_assert_memory_equal(size, &str[0], buf);
  }
  /* resize */{
#if !(defined TextComplexAccessP_NO_EXCEPT)
    str.resize(next_size, 0);
#else
    text_complex::access::api_error ae;
    str.resize(next_size, 0, ae);
    munit_plus_assert_op(ae, ==, text_complex::access::api_error::Success);
#endif /*TextComplexAccessP_NO_EXCEPT*/
  }
  /* inspect */{
    int const min_size = (next_size<size) ? next_size : size;
    munit_plus_assert_uint32(str.size(), ==, next_size);
    munit_plus_assert_uint32(str.capacity(), >=, next_size);
    munit_plus_assert_ptr_not_null(str.data());
    munit_plus_assert_memory_equal(min_size, str.data(), buf);
  }
  return MUNIT_PLUS_OK;
}


int main(int argc, char **argv) {
  return munit_plus_suite_main(&suite_blockbuf, nullptr, argc, argv);
}
