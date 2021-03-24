
#include "text-complex-plus/access/bdict.hpp"
#include "munit-plus/munit.hpp"
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <cstring>


static MunitPlusResult test_bdict_crc
    (const MunitPlusParameter params[], void* data);

/**
 * @brief Resumable CRC32 calculator.
 * @param v data to check
 * @param len length of data
 * @param oldcrc last CRC value; initialize with `0`
 * @return the new CRC value
 */
static
unsigned long int test_bdict_CRC32_resume
    ( unsigned char const* v, std::size_t len,
      unsigned long int oldcrc);


static MunitPlusTest tests_bdict[] = {
  {(char*)"crc", test_bdict_crc,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_SINGLE_ITERATION,
      nullptr},
  {nullptr, nullptr, nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,nullptr}
};

static MunitPlusSuite const suite_bdict = {
  (char*)"access/bdict/", tests_bdict, nullptr,
      1, MUNIT_PLUS_SUITE_OPTION_NONE
};


/* NOTE: adapted from RFC 7932, Appendix C */
unsigned long int test_bdict_CRC32_resume
    ( unsigned char const* v, std::size_t len,
      unsigned long int oldcrc)
{
  unsigned long int const poly = 0xedb88320UL;
  unsigned long int i;
  unsigned long int crc = oldcrc ^ 0xffffffffUL;
  for (i = 0; i < len; ++i) {
    unsigned int k;
    unsigned long int c;
    c = (crc ^ v[i]) & 0xff;
    for (k = 0; k < 8; k++) {
      c = c & 1 ? poly ^ (c >> 1) : c >> 1;
    }
    crc = c ^ (crc >> 8);
  }
  return crc ^ 0xffffffffUL;
}



MunitPlusResult test_bdict_crc
  (const MunitPlusParameter params[], void* data)
{
  unsigned int const max_wordlen = 25;
  unsigned int j;
  unsigned long int crc = 0;
  for (j = 0; j < max_wordlen; ++j) {
    unsigned int const wordcount =
      text_complex::access::bdict_word_count(j);
    unsigned int i;
    for (i = 0; i < wordcount; ++i) {
      text_complex::access::bdict_word const text =
        text_complex::access::bdict_get_word(j,i);
      munit_plus_assert_size(text.size(), >, 0u);
      crc = test_bdict_CRC32_resume(&text[0], text.size(), crc);
    }
  }
  munit_plus_assert_ulong(crc, ==, 0x5136cb04);
  return MUNIT_PLUS_OK;
}



int main(int argc, char **argv) {
  return munit_plus_suite_main(&suite_bdict, nullptr, argc, argv);
}
