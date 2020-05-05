/**
 * @brief Test program for sequential
 */
#include "../tcmplx-access-plus/seq.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "munit-plus/munit.hpp"
#include <memory>
#include "testfont.hpp"
#include "../mmaptwo-plus/mmaptwo.hpp"

#ifndef NDEBUG
#  define test_sharp2(s) #s
#  define test_sharp(s) test_sharp2(s)
#  define test_break(s) \
     { munit_error(s); return MUNIT_FAIL; }
#else
#  define test_break(s)
#endif /*NDEBUG*/

struct test_seq_fixture {
  text_complex::access::sequential* seq;
  mmaptwo::mmaptwo_i* mapper;
  int gen;
  unsigned int seed;
  size_t len;
};


static MunitPlusResult test_seq_cycle
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_seq_null_eof
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_seq_eof
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_seq_eof_const
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_seq_seek
    (const MunitPlusParameter params[], void* data);

static void* test_seq_null_setup
    (const MunitPlusParameter params[], void* user_data);
static void* test_seq_setup
    (const MunitPlusParameter params[], void* user_data);
static void test_seq_teardown(void* fixture);


static MunitPlusTest tests_seq[] = {
  {(char*)"cycle", test_seq_cycle,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_SINGLE_ITERATION,
      nullptr},
  {(char*)"null_eof", test_seq_null_eof,
      test_seq_null_setup,test_seq_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"eof", test_seq_eof,
      test_seq_setup,test_seq_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"eof_const", test_seq_eof_const,
      test_seq_setup,test_seq_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"seek", test_seq_seek,
      test_seq_setup,test_seq_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"null_seek", test_seq_seek,
      test_seq_null_setup,test_seq_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {nullptr, nullptr, nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,nullptr}
};

/* TODO test isequentialstream, wisequentialstream,  */

static MunitPlusSuite const suite_seq = {
  (char*)"access/seq/", tests_seq, nullptr,
      1, MUNIT_PLUS_SUITE_OPTION_NONE
};




MunitPlusResult test_seq_cycle
  (const MunitPlusParameter params[], void* data)
{
  struct text_complex::access::sequential* ptr[2];
  (void)params;
  (void)data;
  ptr[0] = text_complex::access::seq_new(nullptr);
  ptr[1] = new text_complex::access::sequential(nullptr);
  std::unique_ptr<text_complex::access::sequential> ptr2 =
      text_complex::access::seq_unique(nullptr);
  munit_plus_assert_not_null(ptr[0]);
  munit_plus_assert_not_null(ptr[1]);
  munit_plus_assert_not_null(ptr[2]);
  munit_plus_assert_ptr_not_equal(ptr[0],ptr[1]);
  munit_plus_assert_ptr_not_equal(ptr[0],ptr[2]);
  text_complex::access::seq_destroy(ptr[0]);
  delete ptr[1];
  return MUNIT_PLUS_OK;
}

void* test_seq_null_setup(const MunitPlusParameter params[], void* user_data) {
  struct test_seq_fixture* fixt = munit_plus_new(struct test_seq_fixture);
  fixt->seq = text_complex::access::seq_new(nullptr);
  fixt->mapper = nullptr;
  fixt->len = 0u;
  fixt->gen = -1;
  return fixt;
}

void* test_seq_setup(const MunitPlusParameter params[], void* user_data) {
  struct test_seq_fixture* fixt = munit_plus_new(struct test_seq_fixture);
  fixt->gen = munit_plus_rand_int_range(0,tcmplxAtest_MAX);
  fixt->len = (std::size_t)munit_plus_rand_int_range(0,32767);
  if (fixt->len > tcmplxAtest_gen_size(fixt->gen))
    fixt->len = tcmplxAtest_gen_size(fixt->gen);
  fixt->seed = (unsigned int)munit_plus_rand_uint32();
  fixt->mapper = tcmplxAtest_gen_maptwo(fixt->gen, fixt->len, fixt->seed);
  if (fixt->mapper) {
    fixt->seq = text_complex::access::seq_new(fixt->mapper);
  } else fixt->seq = nullptr;
  return fixt;
}


void test_seq_teardown(void* fixture) {
  struct test_seq_fixture* const fixt =
    static_cast<struct test_seq_fixture*>(fixture);
  text_complex::access::seq_destroy(fixt->seq);
  if (fixt->mapper)
    delete fixt->mapper;
  delete fixt;
  return;
}

MunitPlusResult test_seq_null_eof
  (const MunitPlusParameter params[], void* data)
{
  struct test_seq_fixture* const fixt =
    static_cast<struct test_seq_fixture*>(data);
  struct text_complex::access::sequential* const p = fixt->seq;
  struct text_complex::access::sequential const* const p_c = p;
  bool use_const = (munit_plus_rand_int_range(0,1) != 0);
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  if (use_const) {
    munit_plus_assert_size(p_c->get_pos(),==,0u);
  } else {
    munit_plus_assert_size(p->get_pos(),==,0u);
  }
  munit_plus_assert_int(p->get_byte(),==,-1);
  if (use_const) {
    munit_plus_assert_size(p_c->get_pos(),==,0u);
  } else {
    munit_plus_assert_size(p->get_pos(),==,0u);
  }
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_seq_eof
  (const MunitPlusParameter params[], void* data)
{
  struct test_seq_fixture* const fixt =
    static_cast<struct test_seq_fixture*>(data);
  struct text_complex::access::sequential* const p = fixt->seq;
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  munit_plus_logf(MUNIT_PLUS_LOG_INFO,
    "inspecting until %" MUNIT_PLUS_SIZE_MODIFIER "u", fixt->len);
  if (fixt->len > 0u) {
    size_t i;
    for (i = 0u; i < fixt->len; ++i) {
      int b;
      int const d = tcmplxAtest_gen_datum(fixt->gen, i, fixt->seed);
      munit_plus_assert_size(p->get_pos(),==,i);
      b = p->get_byte();
      if (b!=d)
        munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
          "breaks at %" MUNIT_PLUS_SIZE_MODIFIER "u", i);
      munit_plus_assert_int(b,==,d);
    }
    munit_plus_assert_size(p->get_pos(),==,i);
    munit_plus_assert_int(p->get_byte(),==,-1);
  } else {
    munit_plus_assert_size(p->get_pos(),==,0u);
    munit_plus_assert_int(p->get_byte(),==,-1);
    munit_plus_assert_size(p->get_pos(),==,0u);
  }
  return MUNIT_PLUS_OK;
}


MunitPlusResult test_seq_eof_const
  (const MunitPlusParameter params[], void* data)
{
  struct test_seq_fixture* const fixt =
    static_cast<struct test_seq_fixture*>(data);
  struct text_complex::access::sequential* const p = fixt->seq;
  struct text_complex::access::sequential const* const p_c = fixt->seq;
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  munit_plus_logf(MUNIT_PLUS_LOG_INFO,
    "inspecting until %" MUNIT_PLUS_SIZE_MODIFIER "u", fixt->len);
  if (fixt->len > 0u) {
    size_t i;
    for (i = 0u; i < fixt->len; ++i) {
      int b;
      int const d = tcmplxAtest_gen_datum(fixt->gen, i, fixt->seed);
      munit_plus_assert_size(p_c->get_pos(),==,i);
      b = p->get_byte();
      if (b!=d)
        munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
          "breaks at %" MUNIT_PLUS_SIZE_MODIFIER "u", i);
      munit_plus_assert_int(b,==,d);
    }
    munit_plus_assert_size(p_c->get_pos(),==,i);
    munit_plus_assert_int(p->get_byte(),==,-1);
  } else {
    munit_plus_assert_size(p_c->get_pos(),==,0u);
    munit_plus_assert_int(p->get_byte(),==,-1);
    munit_plus_assert_size(p_c->get_pos(),==,0u);
  }
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_seq_seek
  (const MunitPlusParameter params[], void* data)
{
  struct test_seq_fixture* const fixt =
    static_cast<struct test_seq_fixture*>(data);
  struct text_complex::access::sequential* const p = fixt->seq;
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  munit_plus_logf(MUNIT_PLUS_LOG_INFO,
    "inspecting until %" MUNIT_PLUS_SIZE_MODIFIER "u", fixt->len);
  /* */{
    int j;
    for (j = 0; j < 20; ++j) {
      std::size_t i;
      if (j == 0u)
        i = fixt->len;
      else if (j == 1u)
        i = fixt->len-1u;
      else if (j == 19)
        i = 0u;
      else
        i = (std::size_t)munit_plus_rand_int_range(0, 32767);
      if (i > fixt->len) {
        /* expect failure */
        std::size_t res = p->set_pos(i);
        if (res != ~(std::size_t)0u)
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "breaks past %" MUNIT_PLUS_SIZE_MODIFIER "u", i);
        munit_plus_assert_size(~(std::size_t)0u,==,res);
      } else if (i == fixt->len) {
        int b;
        int const d = -1;
        munit_plus_assert_size(i,==,p->set_pos(i));
        b = p->get_byte();
        if (b!=d)
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "breaks at the end (%" MUNIT_PLUS_SIZE_MODIFIER "u)", i);
        munit_plus_assert_int(b,==,d);
      } else {
        /* expect success */
        int b;
        int const d = tcmplxAtest_gen_datum(fixt->gen, i, fixt->seed);
        munit_plus_assert_size(i,==,p->set_pos(i));
        b = p->get_byte();
        if (b!=d)
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "breaks at %" MUNIT_PLUS_SIZE_MODIFIER "u", i);
        munit_plus_assert_int(b,==,d);
      }
    }
  }
  return MUNIT_PLUS_OK;
}




int main(int argc, char **argv) {
  return munit_plus_suite_main(&suite_seq, nullptr, argc, argv);
}
