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
#include <ios>
#include <vector>

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

template <typename t>
static t test_seq_wchar_cast(wchar_t v);

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
static MunitPlusResult test_seq_whence
    (const MunitPlusParameter params[], void* data);
#if  (!(defined TextComplexAccessP_NO_IOSTREAM)) \
  && (!(defined TextComplexAccessP_NO_LOCALE))
static MunitPlusResult test_seqbuf_cycle
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_seqstream_cycle
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_seqwbuf_cycle
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_seqwstream_cycle
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_seqstream_eof
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_seqstream_eof_const
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_seqstream_seek
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_seqstream_whence
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_seqstream_setbuf
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_seqwstream_eof
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_seqwstream_eof_const
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_seqwstream_seek
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_seqwstream_whence
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_seqwstream_setbuf
    (const MunitPlusParameter params[], void* data);
#endif //TextComplexAccessP_NO_IOSTREAM && TextComplexAccessP_NO_LOCALE

static void* test_seq_null_setup
    (const MunitPlusParameter params[], void* user_data);
static void* test_seq_setup
    (const MunitPlusParameter params[], void* user_data);
static void* test_seq_mapper_setup
    (const MunitPlusParameter params[], void* user_data);
static void test_seq_teardown(void* fixture);

template <typename t>
t test_seq_wchar_cast(wchar_t v) {
#if (defined _MSC_VER)
  return v >= 256u ? -1 : static_cast<t>(v);
#else
  return (v < 0 || v > 127) ? -1 : static_cast<t>(v);
#endif
}


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
  {(char*)"whence", test_seq_whence,
      test_seq_setup,test_seq_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"null_whence", test_seq_whence,
      test_seq_null_setup,test_seq_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
#if  (!(defined TextComplexAccessP_NO_IOSTREAM)) \
  && (!(defined TextComplexAccessP_NO_LOCALE))
  {(char*)"buf/cycle", test_seqbuf_cycle,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_SINGLE_ITERATION,
      nullptr},
  {(char*)"stream/cycle", test_seqstream_cycle,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_SINGLE_ITERATION,
      nullptr},
  {(char*)"wbuf/cycle", test_seqwbuf_cycle,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_SINGLE_ITERATION,
      nullptr},
  {(char*)"wstream/cycle", test_seqwstream_cycle,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_SINGLE_ITERATION,
      nullptr},
  {(char*)"stream/null_eof", test_seqstream_eof,
      test_seq_null_setup,test_seq_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"stream/eof", test_seqstream_eof,
      test_seq_mapper_setup,test_seq_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"stream/seek", test_seqstream_seek,
      test_seq_mapper_setup,test_seq_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"stream/null_seek", test_seqstream_seek,
      test_seq_null_setup,test_seq_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"stream/whence", test_seqstream_whence,
      test_seq_mapper_setup,test_seq_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"stream/null_whence", test_seqstream_whence,
      test_seq_null_setup,test_seq_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"stream/setbuf", test_seqstream_setbuf,
      test_seq_mapper_setup,test_seq_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"wstream/null_eof", test_seqwstream_eof,
      test_seq_null_setup,test_seq_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"wstream/eof", test_seqwstream_eof,
      test_seq_mapper_setup,test_seq_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"wstream/seek", test_seqwstream_seek,
      test_seq_mapper_setup,test_seq_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"wstream/null_seek", test_seqwstream_seek,
      test_seq_null_setup,test_seq_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"wstream/whence", test_seqwstream_whence,
      test_seq_mapper_setup,test_seq_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"wstream/null_whence", test_seqwstream_whence,
      test_seq_null_setup,test_seq_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"wstream/setbuf", test_seqwstream_setbuf,
      test_seq_mapper_setup,test_seq_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
#endif //TextComplexAccessP_NO_IOSTREAM && TextComplexAccessP_NO_LOCALE
  {nullptr, nullptr, nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,nullptr}
};


static MunitPlusSuite const suite_seq = {
  (char*)"access/seq/", tests_seq, nullptr,
      1, MUNIT_PLUS_SUITE_OPTION_NONE
};




MunitPlusResult test_seq_cycle
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::sequential* ptr[2];
  (void)params;
  (void)data;
  ptr[0] = text_complex::access::seq_new(nullptr);
  ptr[1] = new text_complex::access::sequential(nullptr);
  std::unique_ptr<text_complex::access::sequential> ptr2 =
      text_complex::access::seq_unique(nullptr);
  munit_plus_assert_not_null(ptr[0]);
  munit_plus_assert_not_null(ptr[1]);
  munit_plus_assert_not_null(ptr2.get());
  munit_plus_assert_ptr_not_equal(ptr[0],ptr[1]);
  munit_plus_assert_ptr_not_equal(ptr[0],ptr2.get());
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

void* test_seq_mapper_setup
    (const MunitPlusParameter params[], void* user_data)
{
  struct test_seq_fixture* fixt = munit_plus_new(struct test_seq_fixture);
  fixt->gen = munit_plus_rand_int_range(0,tcmplxAtest_MAX);
  fixt->len = (std::size_t)munit_plus_rand_int_range(0,32767);
  if (fixt->len > tcmplxAtest_gen_size(fixt->gen))
    fixt->len = tcmplxAtest_gen_size(fixt->gen);
  fixt->seed = (unsigned int)munit_plus_rand_uint32();
  fixt->mapper = tcmplxAtest_gen_maptwo(fixt->gen, fixt->len, fixt->seed);
  fixt->seq = nullptr;
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
  text_complex::access::sequential* const p = fixt->seq;
  text_complex::access::sequential const* const p_c = p;
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
  text_complex::access::sequential* const p = fixt->seq;
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
  text_complex::access::sequential* const p = fixt->seq;
  text_complex::access::sequential const* const p_c = fixt->seq;
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
  text_complex::access::sequential* const p = fixt->seq;
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

MunitPlusResult test_seq_whence
  (const MunitPlusParameter params[], void* data)
{
  struct test_seq_fixture* const fixt =
    static_cast<struct test_seq_fixture*>(data);
  text_complex::access::sequential* const p = fixt->seq;
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  munit_plus_logf(MUNIT_PLUS_LOG_INFO,
    "inspecting until %" MUNIT_PLUS_SIZE_MODIFIER "u", fixt->len);
  /* */{
    int j;
    std::size_t i = 0u;
    std::size_t const fail = ~(std::size_t)0u;
    for (j = 0; j < 20; ++j) {
      long int const dist = munit_plus_rand_int_range(-32767, 32767);
      text_complex::access::seq_whence whence =
        static_cast<text_complex::access::seq_whence>(
            munit_plus_rand_int_range(0,2)
          );
      std::size_t landing_i;
      /* predict where the read position should land */
      switch (whence) {
      case text_complex::access::seq_whence::Set:
        landing_i = (dist < 0) ? fail : static_cast<std::size_t>(dist);
        break;
      case text_complex::access::seq_whence::Cur:
        if (dist < 0) {
          std::size_t pdist = static_cast<std::size_t>(-dist);
          landing_i = (pdist > i) ? fail : i-pdist;
        } else {
          std::size_t pdist = static_cast<std::size_t>(dist);
          landing_i = (pdist > fixt->len-i) ? fail : i+pdist;
        }
        break;
      case text_complex::access::seq_whence::End:
        if (dist > 0)
          landing_i = fail;
        else {
          std::size_t pdist = static_cast<std::size_t>(-dist);
          landing_i = (pdist > fixt->len) ? fail : fixt->len-pdist;
        }
        break;
      }
      if (landing_i > fixt->len) {
        /* expect failure */
        long int res = p->seek(dist, whence);
        if (res >= -1L)
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "breaks past %" MUNIT_PLUS_SIZE_MODIFIER "u to "
            "%" MUNIT_PLUS_SIZE_MODIFIER "u (%li,%i)",
            i,landing_i,dist,static_cast<int>(whence));
        munit_plus_assert_long(res,<,-1L);
      } else if (landing_i == fixt->len) {
        int b;
        int const d = -1;
        long int res = p->seek(dist,whence);
        if (res != static_cast<long int>(landing_i))
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "breaks from %" MUNIT_PLUS_SIZE_MODIFIER "u to "
            "%" MUNIT_PLUS_SIZE_MODIFIER "u (%li,%i)",
            i,landing_i,dist,static_cast<int>(whence));
        munit_plus_assert_long(static_cast<long int>(landing_i),==,res);
        i = landing_i;
        b = p->get_byte();
        if (b!=d)
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "breaks at the end (%" MUNIT_PLUS_SIZE_MODIFIER "u)", i);
        munit_plus_assert_int(b,==,d);
      } else {
        /* expect success */
        int b;
        int const d = tcmplxAtest_gen_datum(fixt->gen, landing_i, fixt->seed);
        long int res = p->seek(dist,whence);
        if (res != static_cast<long int>(landing_i))
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "breaks from %" MUNIT_PLUS_SIZE_MODIFIER "u to "
            "%" MUNIT_PLUS_SIZE_MODIFIER "u (%li,%i)",
            i,landing_i,dist,static_cast<int>(whence));
        munit_plus_assert_long(static_cast<long int>(landing_i),==,res);
        i = landing_i;
        b = p->get_byte();
        if (b!=d)
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "breaks at %" MUNIT_PLUS_SIZE_MODIFIER "u", i);
        munit_plus_assert_int(b,==,d);
        i += 1u;
      }
    }
  }
  return MUNIT_PLUS_OK;
}


#if  (!(defined TextComplexAccessP_NO_IOSTREAM)) \
  && (!(defined TextComplexAccessP_NO_LOCALE))
MunitPlusResult test_seqbuf_cycle
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::sequentialbuf v;
  text_complex::access::sequentialbuf* ptr;
  (void)params;
  (void)data;
  ptr = new text_complex::access::sequentialbuf;
  std::unique_ptr<text_complex::access::sequentialbuf> ptr2 (
        new text_complex::access::sequentialbuf
      );
  munit_plus_assert_not_null(ptr);
  munit_plus_assert_not_null(ptr2.get());
  munit_plus_assert_ptr_not_equal(ptr,&v);
  munit_plus_assert_ptr_not_equal(ptr,ptr2.get());
  /* swap test */{
    text_complex::access::sequentialbuf w;
    w.swap(v);
    w.swap(*ptr);
    w.swap(*ptr2.get());
  }
  /* move test */{
    text_complex::access::sequentialbuf w = std::move(v);
    v = std::move(w);
  }
  delete ptr;
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_seqwbuf_cycle
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::wsequentialbuf v;
  text_complex::access::wsequentialbuf* ptr;
  (void)params;
  (void)data;
  ptr = new text_complex::access::wsequentialbuf();
  std::unique_ptr<text_complex::access::wsequentialbuf> ptr2 (
        new text_complex::access::wsequentialbuf
      );
  munit_plus_assert_not_null(ptr);
  munit_plus_assert_not_null(ptr2.get());
  munit_plus_assert_ptr_not_equal(ptr,&v);
  munit_plus_assert_ptr_not_equal(ptr,ptr2.get());
  /* swap test */{
    text_complex::access::wsequentialbuf w;
    w.swap(v);
    w.swap(*ptr);
    w.swap(*ptr2.get());
  }
  /* move test */{
    text_complex::access::wsequentialbuf w = std::move(v);
    v = std::move(w);
  }
  delete ptr;
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_seqstream_cycle
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::isequentialstream v;
  text_complex::access::isequentialstream* ptr;
  (void)params;
  (void)data;
  ptr = new text_complex::access::isequentialstream;
  std::unique_ptr<text_complex::access::isequentialstream> ptr2 (
      new text_complex::access::isequentialstream
    );
  munit_plus_assert_not_null(ptr);
  munit_plus_assert_not_null(ptr2.get());
  munit_plus_assert_ptr_not_equal(ptr,ptr2.get());
  munit_plus_assert_ptr_not_equal(ptr,&v);
  /* swap test */{
    text_complex::access::isequentialstream w;
    w.swap(v);
    w.swap(*ptr);
    w.swap(*ptr2.get());
  }
  /* move test */{
    text_complex::access::isequentialstream w = std::move(v);
    v = std::move(w);
  }
  delete ptr;
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_seqwstream_cycle
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::wisequentialstream v;
  text_complex::access::wisequentialstream* ptr;
  (void)params;
  (void)data;
  ptr = new text_complex::access::wisequentialstream;
  std::unique_ptr<text_complex::access::wisequentialstream> ptr2 (
      new text_complex::access::wisequentialstream
    );
  munit_plus_assert_not_null(ptr);
  munit_plus_assert_not_null(ptr2.get());
  munit_plus_assert_ptr_not_equal(ptr,ptr2.get());
  munit_plus_assert_ptr_not_equal(ptr,&v);
  /* swap test */{
    text_complex::access::wisequentialstream w;
    w.swap(v);
    w.swap(*ptr);
    w.swap(*ptr2.get());
  }
  /* move test */{
    text_complex::access::wisequentialstream w = std::move(v);
    v = std::move(w);
  }
  delete ptr;
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_seqstream_eof
    (const MunitPlusParameter params[], void* data)
{
  struct test_seq_fixture* const fixt =
    static_cast<struct test_seq_fixture*>(data);
  mmaptwo::mmaptwo_i* const m = fixt->mapper;
  text_complex::access::isequentialstream v(m);
  /* TODO randomly test
   * ...sequential_buf<...>::pubsetbuf(char*, std::streamsize) */
  munit_plus_logf(MUNIT_PLUS_LOG_INFO,
    "inspecting until %" MUNIT_PLUS_SIZE_MODIFIER "u", fixt->len);
  /* */{
    size_t i;
    char pre_b;
    for (i = 0u; i < fixt->len; ++i) {
      int b;
      int const d = tcmplxAtest_gen_datum(fixt->gen, i, fixt->seed);
      munit_plus_assert_llong(v.tellg(),==,i);
      if (!v.get(pre_b)) {
        b = -1;
      } else {
        b = ((unsigned char)pre_b)&255;
      }
      if (b!=d)
        munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
          "breaks at %" MUNIT_PLUS_SIZE_MODIFIER "u", i);
      munit_plus_assert_int(b,==,d);
      if (b == -1) {
        return /*early*/ MUNIT_PLUS_OK;
      }
    }
    munit_plus_assert_llong(v.tellg(),==,i);
    munit_plus_assert_true(!v.get(pre_b));
    munit_plus_assert_true(v.eof());
    munit_plus_assert_llong(v.tellg(),==,-1);
  }
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_seqstream_seek
    (const MunitPlusParameter params[], void* data)
{
  struct test_seq_fixture* const fixt =
    static_cast<struct test_seq_fixture*>(data);
  mmaptwo::mmaptwo_i* const m = fixt->mapper;
  text_complex::access::isequentialstream v(m);
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
      v.clear();
      if (i > fixt->len) {
        /* expect failure */
        v.seekg(i);
        if (!v.fail())
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "breaks past %" MUNIT_PLUS_SIZE_MODIFIER "u", i);
        munit_plus_assert_false(static_cast<bool>(v));
      } else if (i == fixt->len) {
        int b;
        int const d = -1;
        v.seekg(i);
        bool const bool_v = static_cast<bool>(v);
        if (!bool_v)
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "fails at the end (%" MUNIT_PLUS_SIZE_MODIFIER "u)", i);
        munit_plus_assert_llong(i,==,v.tellg());
        munit_plus_assert_true(bool_v);
        b = v.get();
        if (b!=d)
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "breaks at the end (%" MUNIT_PLUS_SIZE_MODIFIER "u)", i);
        munit_plus_assert_int(b,==,d);
      } else {
        /* expect success */
        int b;
        int const d = tcmplxAtest_gen_datum(fixt->gen, i, fixt->seed);
        v.seekg(i);
        bool const bool_v = static_cast<bool>(v);
        if (!bool_v)
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "fails at %" MUNIT_PLUS_SIZE_MODIFIER "u", i);
        munit_plus_assert_llong(i,==,v.tellg());
        munit_plus_assert_true(bool_v);
        b = v.get();
        if (b!=d)
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "breaks at %" MUNIT_PLUS_SIZE_MODIFIER "u", i);
        munit_plus_assert_int(b,==,d);
      }
    }
  }
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_seqstream_whence
    (const MunitPlusParameter params[], void* data)
{
  /* test with 0x1227ce1f */
  struct test_seq_fixture* const fixt =
    static_cast<struct test_seq_fixture*>(data);
  mmaptwo::mmaptwo_i* const m = fixt->mapper;
  text_complex::access::isequentialstream v(m);
  (void)params;
  munit_plus_logf(MUNIT_PLUS_LOG_INFO,
    "inspecting until %" MUNIT_PLUS_SIZE_MODIFIER "u", fixt->len);
  /* */{
    int j;
    std::size_t i = 0u;
    std::size_t const fail = ~(std::size_t)0u;
    for (j = 0; j < 20; ++j) {
      long int const dist = munit_plus_rand_int_range(-32767, 32767);
      std::ios_base::seekdir dir;
      std::size_t landing_i;
      int const rir = munit_plus_rand_int_range(0,2);
      v.clear();
      /* predict where the read position should land */
      switch (rir) {
      case 0:
        dir = std::ios_base::beg;
        landing_i = (dist < 0) ? fail : static_cast<std::size_t>(dist);
        break;
      case 1:
        dir = std::ios_base::cur;
        if (dist < 0) {
          std::size_t pdist = static_cast<std::size_t>(-dist);
          landing_i = (pdist > i) ? fail : i-pdist;
        } else {
          std::size_t pdist = static_cast<std::size_t>(dist);
          landing_i = (pdist > fixt->len-i) ? fail : i+pdist;
        }
        break;
      default:
        dir = std::ios_base::end;
        if (dist > 0)
          landing_i = fail;
        else {
          std::size_t pdist = static_cast<std::size_t>(-dist);
          landing_i = (pdist > fixt->len) ? fail : fixt->len-pdist;
        }
        break;
      }
      if (landing_i > fixt->len) {
        /* expect failure */
        v.seekg(dist, dir);
        if (!v.fail())
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "breaks past %" MUNIT_PLUS_SIZE_MODIFIER "u to "
            "%" MUNIT_PLUS_SIZE_MODIFIER "u (%li,%i)",
            i,landing_i,dist,rir);
        munit_plus_assert_false(static_cast<bool>(v));
      } else if (landing_i == fixt->len) {
        int b;
        int const d = -1;
        v.seekg(dist,dir);
        bool const bool_v = static_cast<bool>(v);
        if (!bool_v)
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "fails from %" MUNIT_PLUS_SIZE_MODIFIER "u to "
            "%" MUNIT_PLUS_SIZE_MODIFIER "u (%li,%i)",
            i,landing_i,dist,rir);
        munit_plus_assert_llong(landing_i,==,v.tellg());
        munit_plus_assert_true(bool_v);
        i = landing_i;
        b = v.get();
        if (b!=d)
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "breaks at the end (%" MUNIT_PLUS_SIZE_MODIFIER "u)", i);
        munit_plus_assert_int(b,==,d);
      } else {
        /* expect success */
        int b;
        int const d = tcmplxAtest_gen_datum(fixt->gen, landing_i, fixt->seed);
        v.seekg(dist,dir);
        bool const bool_v = static_cast<bool>(v);
        if (!bool_v)
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "fails from %" MUNIT_PLUS_SIZE_MODIFIER "u to "
            "%" MUNIT_PLUS_SIZE_MODIFIER "u (%li,%i)",
            i,landing_i,dist,rir);
        munit_plus_assert_llong(landing_i,==,v.tellg());
        munit_plus_assert_true(bool_v);
        i = landing_i;
        b = v.get();
        if (b!=d)
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "breaks at %" MUNIT_PLUS_SIZE_MODIFIER "u", i);
        munit_plus_assert_int(b,==,d);
        i += 1u;
      }
    }
  }
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_seqstream_setbuf
    (const MunitPlusParameter params[], void* data)
{
  struct test_seq_fixture* const fixt =
    static_cast<struct test_seq_fixture*>(data);
  mmaptwo::mmaptwo_i* const m = fixt->mapper;
  text_complex::access::isequentialstream v(m);
  std::streamsize const buff_size = munit_plus_rand_int_range(1, 8192);
  std::vector<char> buff(static_cast<std::size_t>(buff_size));
  v.rdbuf()->pubsetbuf(buff.data(), static_cast<std::size_t>(buff_size));
  munit_plus_logf(MUNIT_PLUS_LOG_INFO,
    "inspecting until %" MUNIT_PLUS_SIZE_MODIFIER "u", fixt->len);
  /* */{
    std::size_t tell_next = testfont_rand_size_range(0,fixt->len);
    std::size_t seek_next = testfont_rand_size_range(0,fixt->len);
    bool seek_check = true;
    std::size_t buff_point = 0u;
    size_t i;
    char pre_b;
    std::fpos<std::mbstate_t> seek_pos = std::fpos<std::mbstate_t>(-1);
    for (i = 0u; i < fixt->len; ++i) {
      if (seek_check) {
        if (tell_next == i) {
          seek_pos = v.tellg();
        }
        if (seek_next == i) {
          if (seek_pos == std::fpos<std::mbstate_t>(-1)) {
            /* relative seek */
            v.seekg(static_cast<int>(tell_next), std::ios_base::beg);
            if (tell_next == fixt->len)
              break;
            else i = tell_next;
          } else {
            v.seekg(seek_pos);
            i = tell_next;
          }
          buff_point = 0u;
          seek_check = false;
        }
      }
      int b;
      int const d = tcmplxAtest_gen_datum(fixt->gen, i, fixt->seed);
      munit_plus_assert_llong(v.tellg(),==,i);
      if (!v.get(pre_b)) {
        b = -1;
      } else {
        b = ((unsigned char)pre_b)&255;
      }
      if (b!=d)
        munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
          "breaks at %" MUNIT_PLUS_SIZE_MODIFIER "u", i);
      munit_plus_assert_int(b,==,d);
      if (b == -1) {
        return /*early*/ MUNIT_PLUS_OK;
      } else {
        munit_plus_assert_int
          (d, ==, static_cast<unsigned char>(buff[buff_point])&255);
        buff_point = (buff_point+1)%buff_size;
      }
    }
    munit_plus_assert_llong(v.tellg(),==,i);
    munit_plus_assert_true(!v.get(pre_b));
    munit_plus_assert_true(v.eof());
    munit_plus_assert_llong(v.tellg(),==,-1);
  }
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_seqwstream_eof
    (const MunitPlusParameter params[], void* data)
{
  struct test_seq_fixture* const fixt =
    static_cast<struct test_seq_fixture*>(data);
  mmaptwo::mmaptwo_i* const m = fixt->mapper;
  text_complex::access::wisequentialstream v(m);
  /* TODO randomly test
   * ...sequential_buf<...>::pubsetbuf(wchar_t*, std::streamsize) */
  v.imbue(std::locale::classic());
  munit_plus_logf(MUNIT_PLUS_LOG_INFO,
    "inspecting until %" MUNIT_PLUS_SIZE_MODIFIER "u", fixt->len);
  /* */{
    size_t i;
    wchar_t pre_b;
    for (i = 0u; i < fixt->len; ++i) {
      std::wint_t b;
      int const pre_d = tcmplxAtest_gen_datum(fixt->gen, i, fixt->seed);
      std::wint_t const d = test_seq_wchar_cast<std::wint_t>(pre_d);
      munit_plus_assert_llong(v.tellg(),==,i);
      if (!v.get(pre_b)) {
        b = -1;
      } else {
        if (pre_b > 255)
          b = 256;
        else
          b = ((unsigned char)pre_b)&255;
      }
      if (b!=d)
        munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
          "breaks at %" MUNIT_PLUS_SIZE_MODIFIER "u", i);
      munit_plus_assert_int(b,==,d);
      if (b == -1) {
        return /*early*/ MUNIT_PLUS_OK;
      }
    }
    munit_plus_assert_llong(v.tellg(),==,i);
    munit_plus_assert_true(!v.get(pre_b));
    munit_plus_assert_true(v.eof());
    munit_plus_assert_llong(v.tellg(),==,-1);
  }
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_seqwstream_seek
    (const MunitPlusParameter params[], void* data)
{
  struct test_seq_fixture* const fixt =
    static_cast<struct test_seq_fixture*>(data);
  mmaptwo::mmaptwo_i* const m = fixt->mapper;
  text_complex::access::wisequentialstream v(m);
  (void)params;
  v.imbue(std::locale::classic());
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
      v.clear();
      if (i > fixt->len) {
        /* expect failure */
        v.seekg(i);
        if (!v.fail())
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "breaks past %" MUNIT_PLUS_SIZE_MODIFIER "u", i);
        munit_plus_assert_false(static_cast<bool>(v));
      } else if (i == fixt->len) {
        std::wint_t b;
        std::wint_t const d = -1;
        v.seekg(i);
        bool const bool_v = static_cast<bool>(v);
        if (!bool_v)
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "fails at the end (%" MUNIT_PLUS_SIZE_MODIFIER "u)", i);
        munit_plus_assert_llong(i,==,v.tellg());
        munit_plus_assert_true(bool_v);
        b = v.get();
        if (b!=d)
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "breaks at the end (%" MUNIT_PLUS_SIZE_MODIFIER "u)", i);
        munit_plus_assert_int(b,==,d);
      } else {
        /* expect success (sort of) */
        std::wint_t b;
        int const pre_d = tcmplxAtest_gen_datum(fixt->gen, i, fixt->seed);
        std::wint_t const d = test_seq_wchar_cast<std::wint_t>(pre_d);
        v.seekg(i);
        bool const bool_v = static_cast<bool>(v);
        if (!bool_v)
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "fails at %" MUNIT_PLUS_SIZE_MODIFIER "u", i);
        munit_plus_assert_llong(i,==,v.tellg());
        munit_plus_assert_true(bool_v);
        b = v.get();
        if (b!=d)
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "breaks at %" MUNIT_PLUS_SIZE_MODIFIER "u", i);
        munit_plus_assert_int(b,==,d);
      }
    }
  }
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_seqwstream_whence
    (const MunitPlusParameter params[], void* data)
{
  /* test with 0x1227ce1f */
  struct test_seq_fixture* const fixt =
    static_cast<struct test_seq_fixture*>(data);
  mmaptwo::mmaptwo_i* const m = fixt->mapper;
  if (fixt->len == 32528) {
    munit_plus_log(MUNIT_PLUS_LOG_INFO, "edge case.");
  }
  text_complex::access::wisequentialstream v(m);
  (void)params;
  munit_plus_logf(MUNIT_PLUS_LOG_INFO,
    "inspecting until %" MUNIT_PLUS_SIZE_MODIFIER "u", fixt->len);
  /* */{
    int j;
    std::size_t i = 0u;
    std::size_t const fail = ~(std::size_t)0u;
    for (j = 0; j < 20; ++j) {
      long int const dist = munit_plus_rand_int_range(-32767, 32767);
      std::ios_base::seekdir dir;
      std::size_t landing_i;
      int const rir = munit_plus_rand_int_range(0,2);
      v.clear();
      /* predict where the read position should land */
      switch (rir) {
      case 0:
        dir = std::ios_base::beg;
        landing_i = (dist < 0) ? fail : static_cast<std::size_t>(dist);
        break;
      case 1:
        dir = std::ios_base::cur;
        if (dist < 0) {
          std::size_t pdist = static_cast<std::size_t>(-dist);
          landing_i = (pdist > i) ? fail : i-pdist;
        } else {
          std::size_t pdist = static_cast<std::size_t>(dist);
          landing_i = (pdist > fixt->len-i) ? fail : i+pdist;
        }
        break;
      default:
        dir = std::ios_base::end;
        if (dist > 0)
          landing_i = fail;
        else {
          std::size_t pdist = static_cast<std::size_t>(-dist);
          landing_i = (pdist > fixt->len) ? fail : fixt->len-pdist;
        }
        break;
      }
      munit_plus_logf(MUNIT_PLUS_LOG_INFO,
        "whence %2u (%li,%i) -> %" MUNIT_PLUS_SIZE_MODIFIER "u",
        j,dist,rir,landing_i);
      if (landing_i > fixt->len) {
        /* expect failure */
        v.seekg(dist, dir);
        if (!v.fail())
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "breaks past %" MUNIT_PLUS_SIZE_MODIFIER "u to "
            "%" MUNIT_PLUS_SIZE_MODIFIER "u (%li,%i)",
            i,landing_i,dist,rir);
        munit_plus_assert_false(static_cast<bool>(v));
      } else if (landing_i == fixt->len) {
        std::wint_t b;
        std::wint_t const d = -1;
        v.seekg(dist,dir);
        bool const bool_v = static_cast<bool>(v);
        if (!bool_v)
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "fails from %" MUNIT_PLUS_SIZE_MODIFIER "u to "
            "%" MUNIT_PLUS_SIZE_MODIFIER "u (%li,%i)",
            i,landing_i,dist,rir);
        munit_plus_assert_llong(landing_i,==,v.tellg());
        munit_plus_assert_true(bool_v);
        i = landing_i;
        b = v.get();
        if (b!=d)
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "breaks at the end (%" MUNIT_PLUS_SIZE_MODIFIER "u)", i);
        munit_plus_assert_int(b,==,d);
      } else {
        /* expect success (sort of) */
        std::wint_t b;
        int const pre_d = tcmplxAtest_gen_datum
          (fixt->gen, landing_i, fixt->seed);
        std::wint_t const d = test_seq_wchar_cast<std::wint_t>(pre_d);
        v.seekg(dist,dir);
        bool const bool_v = static_cast<bool>(v);
        if (!bool_v)
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "fails from %" MUNIT_PLUS_SIZE_MODIFIER "u to "
            "%" MUNIT_PLUS_SIZE_MODIFIER "u (%li,%i)",
            i,landing_i,dist,rir);
        long long int tellg_res = v.tellg();
        if (landing_i != tellg_res)
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "bad landing from %" MUNIT_PLUS_SIZE_MODIFIER "u to "
            "%" MUNIT_PLUS_SIZE_MODIFIER "u (%li,%i)",
            i,landing_i,dist,rir);
        munit_plus_assert_llong(landing_i,==,tellg_res);
        munit_plus_assert_true(bool_v);
        i = landing_i;
        b = v.get();
        if (b!=d)
          munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
            "breaks at %" MUNIT_PLUS_SIZE_MODIFIER "u", i);
        munit_plus_assert_int(b,==,d);
        if (d != -1)
          i += 1u;
      }
    }
  }
  return MUNIT_PLUS_OK;
}


MunitPlusResult test_seqwstream_setbuf
    (const MunitPlusParameter params[], void* data)
{
  struct test_seq_fixture* const fixt =
    static_cast<struct test_seq_fixture*>(data);
  mmaptwo::mmaptwo_i* const m = fixt->mapper;
  text_complex::access::wisequentialstream v(m);
  std::streamsize const buff_size = munit_plus_rand_int_range(1, 8192);
  std::vector<wchar_t> buff(static_cast<std::size_t>(buff_size));
  v.rdbuf()->pubsetbuf(buff.data(), static_cast<std::size_t>(buff_size));
  munit_plus_logf(MUNIT_PLUS_LOG_INFO,
    "inspecting until %" MUNIT_PLUS_SIZE_MODIFIER "u", fixt->len);
  /* */{
    std::size_t tell_next = testfont_rand_size_range(0,fixt->len);
    std::size_t seek_next = testfont_rand_size_range(0,fixt->len);
    bool seek_check = true;
    std::size_t buff_point = 0u;
    size_t i;
    wchar_t pre_b;
    std::fpos<std::mbstate_t> seek_pos = std::fpos<std::mbstate_t>(-1);
    for (i = 0u; i < fixt->len; ++i) {
      if (seek_check) {
        if (tell_next == i) {
          seek_pos = v.tellg();
        }
        if (seek_next == i) {
          if (seek_pos == std::fpos<std::mbstate_t>(-1)) {
            /* relative seek */
            v.seekg(static_cast<int>(tell_next), std::ios_base::beg);
            if (tell_next == fixt->len)
              break;
            else i = tell_next;
          } else {
            v.seekg(seek_pos);
            i = tell_next;
          }
          buff_point = 0u;
          seek_check = false;
        }
      }
      std::wint_t b;
      int const pre_d = tcmplxAtest_gen_datum(fixt->gen, i, fixt->seed);
      std::wint_t const d = test_seq_wchar_cast<std::wint_t>(pre_d);
      munit_plus_assert_llong(v.tellg(),==,i);
      if (!v.get(pre_b)) {
        b = -1;
      } else {
        if (pre_b > 255)
          b = 256;
        else
          b = ((unsigned char)pre_b)&255;
      }
      if (b!=d)
        munit_plus_logf(MUNIT_PLUS_LOG_WARNING,
          "breaks at %" MUNIT_PLUS_SIZE_MODIFIER "u", i);
      munit_plus_assert_int(b,==,d);
      if (b == -1) {
        return /*early*/ MUNIT_PLUS_OK;
      } else {
        munit_plus_assert_int
          (d, ==, static_cast<unsigned char>(buff[buff_point])&255);
        buff_point = (buff_point+1)%buff_size;
      }
    }
    munit_plus_assert_llong(v.tellg(),==,i);
    munit_plus_assert_true(!v.get(pre_b));
    munit_plus_assert_true(v.eof());
    munit_plus_assert_llong(v.tellg(),==,-1);
  }
  return MUNIT_PLUS_OK;
}

#endif //TextComplexAccessP_NO_IOSTREAM && TextComplexAccessP_NO_LOCALE


int main(int argc, char **argv) {
  return munit_plus_suite_main(&suite_seq, nullptr, argc, argv);
}
