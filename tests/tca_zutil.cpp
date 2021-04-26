
#include "text-complex-plus/access/zutil.hpp"
#include "munit-plus/munit.hpp"
#include <vector>
#include <cstddef>



static MunitPlusResult test_zutil_adler32
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_zutil_adler32_long
    (const MunitPlusParameter params[], void* data);

static MunitPlusTest tests_zutil[] = {
  {(char*)"adler32", test_zutil_adler32, nullptr,nullptr,
    MUNIT_PLUS_TEST_OPTION_NONE,nullptr},
  {(char*)"adler32/long", test_zutil_adler32_long, nullptr,nullptr,
    MUNIT_PLUS_TEST_OPTION_NONE,nullptr},
  {nullptr, nullptr, nullptr,nullptr, MUNIT_PLUS_TEST_OPTION_NONE,nullptr}
};

static MunitPlusSuite const suite_zutil = {
  (char*)"access/zutil/", tests_zutil, nullptr, 1, MUNIT_PLUS_SUITE_OPTION_NONE
};


static
unsigned long int test_adler32(unsigned char* b, std::size_t n);
  


unsigned long int test_adler32(unsigned char* b, std::size_t n) {
  unsigned long int s1 = 1u;
  unsigned long int s2 = 0u;
  size_t i;
  for (i = 0u; i < n; ++i) {
    s1 += b[i];
    s1 %= 65521;
    s2 += s1;
    s2 %= 65521;
  }
  return s1+(s2*65536);
}



MunitPlusResult test_zutil_adler32
  (const MunitPlusParameter params[], void* data)
{
  unsigned char buf[256];
  std::size_t const len = munit_plus_rand_int_range(0,256);
  unsigned long int checksums[2];
  (void)params;
  (void)data;
  munit_plus_rand_memory(len, static_cast<munit_plus_uint8_t*>(buf));
  checksums[0] = test_adler32(buf, len);
  checksums[1] = text_complex::access::zutil_adler32(len, buf);
  munit_plus_assert_ulong(checksums[1], ==, checksums[0]);
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_zutil_adler32_long
  (const MunitPlusParameter params[], void* data)
{
  std::vector<unsigned char> buf;
  std::size_t const len = munit_plus_rand_int_range(0,32767);
  unsigned long int checksums[3];
  (void)params;
  (void)data;
  buf.resize(len);
  munit_plus_rand_memory(len, static_cast<munit_plus_uint8_t*>(&buf[0]));
  checksums[0] = test_adler32(&buf[0], len);
  checksums[1] = text_complex::access::zutil_adler32(len, &buf[0]);
  /* compute by parts */{
    std::size_t i;
    text_complex::access::uint32 chk = 1;
    for (i = 0; i < len; i += 200) {
      std::size_t const part_len = (len-i > 200) ? 200 : len-i;
      chk = text_complex::access::zutil_adler32(part_len, &buf[i], chk);
    } 
    checksums[2] = chk;
  }
  munit_plus_assert_ulong(checksums[1], ==, checksums[0]);
  munit_plus_assert_ulong(checksums[2], ==, checksums[0]);
  return MUNIT_PLUS_OK;
}




int main(int argc, char **argv) {
  return munit_plus_suite_main(&suite_zutil, nullptr, argc, argv);
}
