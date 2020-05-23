/**
 * @brief Test program for woff2
 */
#include "../tcmplx-access-plus/woff2.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "munit-plus/munit.hpp"
#include <memory>
#include "../mmaptwo-plus/mmaptwo.hpp"
#include "testfont.hpp"
#include "../tcmplx-access-plus/offtable.hpp"

static MunitPlusResult test_woff2_cycle
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_woff2_item
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_woff2_tag_toi
    (const MunitPlusParameter params[], void* data);
static void* test_woff2_null_setup
    (const MunitPlusParameter params[], void* user_data);
static void* test_woff2_setup
    (const MunitPlusParameter params[], void* user_data);
static void test_woff2_teardown(void* fixture);


static MunitPlusTest tests_woff2[] = {
  {(char*)"cycle", test_woff2_cycle,
      nullptr,nullptr,
      static_cast<MunitPlusTestOptions>(
          MUNIT_PLUS_TEST_OPTION_SINGLE_ITERATION | MUNIT_PLUS_TEST_OPTION_TODO
        ),
      nullptr},
  {(char*)"item", test_woff2_item,
      test_woff2_setup,test_woff2_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"tag_toi", test_woff2_tag_toi,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {nullptr, nullptr, nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,nullptr}
};

static MunitPlusSuite const suite_woff2 = {
  (char*)"access/woff2/", tests_woff2, nullptr,
      1, MUNIT_PLUS_SUITE_OPTION_NONE
};




MunitPlusResult test_woff2_cycle
  (const MunitPlusParameter params[], void* data)
{
  struct text_complex::access::woff2* ptr[2];
  (void)params;
  (void)data;
  ptr[0] = text_complex::access::woff2_new(nullptr);
  ptr[1] = new text_complex::access::woff2(nullptr);
  std::unique_ptr<text_complex::access::woff2> ptr2 =
      text_complex::access::woff2_unique(nullptr);
  munit_plus_assert_not_null(ptr[0]);
  munit_plus_assert_not_null(ptr[1]);
  munit_plus_assert_not_null(ptr[2]);
  munit_plus_assert_ptr_not_equal(ptr[0],ptr[1]);
  munit_plus_assert_ptr_not_equal(ptr[0],ptr[2]);
  text_complex::access::woff2_destroy(ptr[0]);
  delete ptr[1];
  return MUNIT_PLUS_OK;
}

void* test_woff2_null_setup
    (const MunitPlusParameter params[], void* user_data)
{
  return text_complex::access::woff2_new(
      nullptr
    );
}

void* test_woff2_setup(const MunitPlusParameter params[], void* user_data) {
  struct tcmplxAtest_arg& tfa =
    *static_cast<struct tcmplxAtest_arg *>(user_data);
  mmaptwo::mmaptwo_i *const m2i =
      (tfa.font_path.size() > 0)
    ? mmaptwo::open(tfa.font_path.c_str(), "re", 0,0)
    : tcmplxAtest_gen_maptwo
        ( tcmplxAtest_Zero, 256,
          static_cast<unsigned int>(munit_plus_rand_uint32()) );
  text_complex::access::woff2* const w2 =
    text_complex::access::woff2_new(m2i);
  if (!w2) {
    delete m2i;
  }
  return w2;
}

void test_woff2_teardown(void* fixture) {
  text_complex::access::woff2_destroy(
      static_cast<struct text_complex::access::woff2*>(fixture)
    );
  return;
}

MunitPlusResult test_woff2_item
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::woff2* const p =
    static_cast<struct text_complex::access::woff2*>(data);
  text_complex::access::woff2 const* const p_c = p;
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  /* inspect offset table items */{
    text_complex::access::offset_table const& offt =
      p_c->get_offsets();
    std::size_t const l = offt.size();
    std::size_t i;
    for (i = 0; i < l; ++i) {
      text_complex::access::offset_line const& line = offt[i];
      munit_plus_logf(MUNIT_PLUS_LOG_INFO,
        "[%" MUNIT_PLUS_SIZE_MODIFIER "u] = {\"%.4s\", %lu from %lu}",
        i, line.tag, static_cast<long unsigned int>(line.length),
        static_cast<long unsigned int>(line.offset)
        );
    }
  }
  return MUNIT_PLUS_OK;
}
MunitPlusResult test_woff2_tag_toi
  (const MunitPlusParameter params[], void* data)
{
  (void)params;
  (void)data;
  unsigned int i = static_cast<unsigned int>(munit_plus_rand_uint32()&63);
  if (i == 63) {
    unsigned char const* value = text_complex::access::woff2_tag_fromi(i);
    munit_plus_assert_ptr_null(value);
    unsigned char buf[4] = {1,2,3,4};
    unsigned int j = text_complex::access::woff2_tag_toi(buf);
    munit_plus_assert_uint(j,==,i);
  } else {
    unsigned char const* value = text_complex::access::woff2_tag_fromi(i);
    munit_plus_assert_ptr_not_null(value);
    unsigned char buf[4];
    std::memcpy(buf,value,sizeof(buf));
    unsigned int j = text_complex::access::woff2_tag_toi(buf);
    munit_plus_assert_uint(j,==,i);
  }
  return MUNIT_PLUS_OK;
}


int main(int argc, char **argv) {
  struct tcmplxAtest_arg tfa;
  return munit_plus_suite_main_custom
    (&suite_woff2, &tfa, argc, argv, tcmplxAtest_get_args());
}
