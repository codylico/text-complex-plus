/**
 * @brief Test program for prefix list
 */
#include "testfont.hpp"
#include "text-complex-plus/access/fixlist.hpp"
#include "munit-plus/munit.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>


static MunitPlusResult test_fixlist_cycle
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_fixlist_item
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_fixlist_gen_codes
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_fixlist_gen_lengths
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_fixlist_preset
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_fixlist_hist_cycle
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_fixlist_hist_item
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_fixlist_codesort
    (const MunitPlusParameter params[], void* data);
static void* test_fixlist_setup
    (const MunitPlusParameter params[], void* user_data);
static void* test_fixlist_gen_setup
    (const MunitPlusParameter params[], void* user_data);
static void* test_fixlist_len_setup
    (const MunitPlusParameter params[], void* user_data);
static void test_fixlist_teardown(void* fixture);
static void* test_fixlist_hist_setup
    (const MunitPlusParameter params[], void* user_data);
static void test_fixlist_hist_teardown(void* fixture);


static MunitPlusParameterEnum test_fixlist_gen_params[] = {
  { (char*)"prefixes", nullptr },
  { nullptr, nullptr },
};

static MunitPlusParameterEnum test_fixlist_len_params[] = {
  { (char*)"frequencies", nullptr },
  { nullptr, nullptr },
};



static MunitPlusTest tests_fixlist[] = {
  {(char*)"cycle", test_fixlist_cycle,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_SINGLE_ITERATION,
      nullptr},
  {(char*)"item", test_fixlist_item,
      test_fixlist_setup,test_fixlist_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"gen_codes", test_fixlist_gen_codes,
      test_fixlist_gen_setup,test_fixlist_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      test_fixlist_gen_params},
  {(char*)"gen_lengths", test_fixlist_gen_lengths,
      test_fixlist_len_setup,test_fixlist_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      test_fixlist_len_params},
  {(char*)"preset", test_fixlist_preset,
      test_fixlist_setup,test_fixlist_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"codesort", test_fixlist_codesort,
      test_fixlist_gen_setup,test_fixlist_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      test_fixlist_gen_params},
  {(char*)"histogram/cycle", test_fixlist_hist_cycle,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_SINGLE_ITERATION,
      nullptr},
  {(char*)"histogram/item", test_fixlist_hist_item,
      test_fixlist_hist_setup,test_fixlist_hist_teardown,
      MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {nullptr, nullptr, nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,nullptr}
};

static MunitPlusSuite const suite_fixlist = {
  (char*)"access/fixlist/", tests_fixlist, nullptr,
      1, MUNIT_PLUS_SUITE_OPTION_NONE
};




MunitPlusResult test_fixlist_cycle
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::prefix_list* ptr[2];
  (void)params;
  (void)data;
  ptr[0] = text_complex::access::fixlist_new(288);
  ptr[1] = new text_complex::access::prefix_list(288);
  std::unique_ptr<text_complex::access::prefix_list> ptr2 =
      text_complex::access::fixlist_unique(288);
  munit_plus_assert_not_null(ptr[0]);
  munit_plus_assert_not_null(ptr[1]);
  munit_plus_assert_not_null(ptr2.get());
  munit_plus_assert_ptr_not_equal(ptr[0],ptr[1]);
  munit_plus_assert_ptr_not_equal(ptr[0],ptr2.get());
  text_complex::access::fixlist_destroy(ptr[0]);
  delete ptr[1];
  return MUNIT_PLUS_OK;
}

void* test_fixlist_setup(const MunitPlusParameter params[], void* user_data) {
  return text_complex::access::fixlist_new(
        static_cast<std::size_t>(munit_plus_rand_int_range(4,256))
      );
}


void* test_fixlist_gen_setup
    (const MunitPlusParameter params[], void* user_data)
{
  tcmplxAtest_fixlist_lex lex;
  text_complex::access::prefix_list* out;
  if (lex.start(munit_plus_parameters_get(params, "prefixes"))) {
    lex.start("3*5,2,4*2");
  }
  out = text_complex::access::fixlist_new(lex.size());
  if (out) {
    size_t i;
    for (i = 0; i < lex.size(); ++i) {
      (*out)[i].len = lex.next();
      (*out)[i].value = i;
    }
  }
  return out;
}

void* test_fixlist_len_setup
    (const MunitPlusParameter params[], void* user_data)
{
  tcmplxAtest_fixlist_lex lex;
  text_complex::access::prefix_list* out;
  int const start_res =
    lex.start(munit_plus_parameters_get(params, "frequencies"));
  out = text_complex::access::fixlist_new
    (start_res ==0? lex.size() : munit_plus_rand_int_range(2,384));
  if (!out) {
    return nullptr;
  } else if (start_res != 0) {
    size_t const len = out->size();
    size_t i;
    /* generate random */
    for (i = 0; i < len; ++i) {
      (*out)[i].len = munit_plus_rand_int_range(0,32);
      (*out)[i].value = i;
    }
  } else {
    size_t i;
    for (i = 0; i < lex.size(); ++i) {
      (*out)[i].len = lex.next();
      (*out)[i].value = i;
    }
  }
  return out;
}


void test_fixlist_teardown(void* fixture) {
  text_complex::access::fixlist_destroy(
      static_cast<text_complex::access::prefix_list*>(fixture)
    );
  return;
}

MunitPlusResult test_fixlist_item
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::prefix_list* const p =
    static_cast<text_complex::access::prefix_list*>(data);
  text_complex::access::prefix_list const* const p_c = p;
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  struct text_complex::access::prefix_line const* dsp[4];
  std::size_t sz = p->size();
  munit_plus_assert_size(sz,>=,4);
  munit_plus_assert_size(sz,<=,256);
  std::size_t i = static_cast<std::size_t>(
        munit_plus_rand_int_range(0,static_cast<int>(sz)-1)
      );
  dsp[0] = &(*p)[i];
  dsp[1] = &(*p_c)[i];
  dsp[2] = &p->at(i);
  dsp[3] = &p_c->at(i);
  munit_plus_assert_not_null(dsp[0]);
  munit_plus_assert_ptr(dsp[0],==,dsp[1]);
  munit_plus_assert_ptr(dsp[0],==,dsp[2]);
  munit_plus_assert_ptr(dsp[0],==,dsp[3]);
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_fixlist_gen_codes
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::prefix_list* const p =
    static_cast<text_complex::access::prefix_list*>(data);
  text_complex::access::prefix_list const* const p_c = p;
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
#if !(defined TextComplexAccessP_NO_EXCEPT)
  text_complex::access::fixlist_gen_codes(*p);
#else
  text_complex::access::api_error ae;
  text_complex::access::fixlist_gen_codes(*p, ae);
  munit_plus_assert_int(ae,==,text_complex::access::api_error::Success);
#endif /*TextComplexAccessP_NO_EXCEPT*/
  /* inspect the new codes */{
    size_t i;
    size_t const len = p->size();
    munit_plus_logf(MUNIT_PLUS_LOG_DEBUG,
      "total %" MUNIT_PLUS_SIZE_MODIFIER "u", len);
    for (i = 0; i < len; ++i) {
      struct text_complex::access::prefix_line const& line = (*p)[i];
      munit_plus_logf(MUNIT_PLUS_LOG_DEBUG,
        "  [%" MUNIT_PLUS_SIZE_MODIFIER "u] = {%#x, l %u, v %lu}",
        i, line.code, line.len, line.value);
      munit_plus_assert_uint((line.code>>(line.len)), ==, 0u);
    }
  }
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_fixlist_codesort
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::prefix_list* const p =
    static_cast<text_complex::access::prefix_list*>(data);
  text_complex::access::prefix_list const* const p_c = p;
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  /* */{
#if !(defined TextComplexAccessP_NO_EXCEPT)
    text_complex::access::fixlist_gen_codes(*p);
#else
    text_complex::access::api_error ae;
    text_complex::access::fixlist_gen_codes(*p, ae);
    munit_plus_assert_int(ae,==,text_complex::access::api_error::Success);
#endif /*TextComplexAccessP_NO_EXCEPT*/
  }
  /* sort the codes */{
#if !(defined TextComplexAccessP_NO_EXCEPT)
    text_complex::access::fixlist_codesort(*p);
#else
    text_complex::access::api_error ae;
    text_complex::access::fixlist_codesort(*p, ae);
    munit_plus_assert_int(ae,==,text_complex::access::api_error::Success);
#endif /*TextComplexAccessP_NO_EXCEPT*/
  }
  /* inspect the new codes */if (p_c->size() > 1) {
    size_t i;
    size_t const len = p_c->size()-1;
    for (i = 0; i < len; ++i) {
      struct text_complex::access::prefix_line const& line = (*p_c)[i];
      struct text_complex::access::prefix_line const& b_line = (*p_c)[i+1];
      munit_plus_assert_uint(line.len, <=, b_line.len);
      munit_plus_assert_uint(
          (line.code<<(15-line.len)), <=, (b_line.code<<(15-b_line.len))
        );
    }
  }
  /* binary search */{
    std::size_t i;
    std::size_t len = p_c->size();
    for (i = 0u; i < len; ++i) {
      unsigned int n;
      unsigned int bits;
      size_t j;
      /* */{
        struct text_complex::access::prefix_line const& line = (*p_c)[i];
        n = line.len;
        bits = line.code;
      }
      if (n == 0)
        continue;
      j = text_complex::access::fixlist_codebsearch(*p_c, n, bits);
      munit_plus_assert_size(j, ==, i);
    }
  }
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_fixlist_preset
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::prefix_list* const p =
    static_cast<text_complex::access::prefix_list*>(data);
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  text_complex::access::prefix_preset x =
    static_cast<text_complex::access::prefix_preset>(
          testfont_rand_int_range(0,7)
        );
  struct text_complex::access::prefix_line dsp[2];
  /* generate once */{
#if !(defined TextComplexAccessP_NO_EXCEPT)
    text_complex::access::fixlist_preset(*p, x);
#else
    text_complex::access::api_error ae;
    text_complex::access::fixlist_preset(*p, x, ae);
    munit_assert_int(ae,==,text_complex::access::api_error::Success);
#endif /*TextComplexAccessP_NO_EXCEPT*/
  }
  std::size_t sz = p->size();
  munit_plus_assert_size(sz,>,0);
  std::size_t i = testfont_rand_size_range(0,sz-1);
  dsp[0] = (*p)[i];
  /* generate again */{
#if !(defined TextComplexAccessP_NO_EXCEPT)
    text_complex::access::fixlist_preset(*p, x);
#else
    text_complex::access::api_error ae;
    text_complex::access::fixlist_preset(*p, x, ae);
    munit_plus_assert_int(ae,==,text_complex::access::api_error::Success);
#endif /*TextComplexAccessP_NO_EXCEPT*/
    munit_plus_assert_size(p->size(),==,sz);
  }
  dsp[1] = (*p)[i];
  munit_plus_assert_memory_equal(sizeof(dsp[0]), &dsp[0],&dsp[1]);
  return MUNIT_PLUS_OK;
}


MunitPlusResult test_fixlist_gen_lengths
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::prefix_list* const p =
    static_cast<text_complex::access::prefix_list*>(data);
  text_complex::access::prefix_list const* const p_c = p;
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  text_complex::access::prefix_histogram ph(p_c->size());
  (void)params;
  /* extract the histogram */{
    std::size_t i;
    std::size_t sz = p_c->size();
    for (i = 0; i < sz; ++i) {
      ph[i] = (*p_c)[i].len;
    }
  }
#if !(defined TextComplexAccessP_NO_EXCEPT)
  text_complex::access::fixlist_gen_lengths(*p, ph, 15);
#else
  text_complex::access::api_error ae;
  text_complex::access::fixlist_gen_lengths(*p, ph, 15, ae);
  munit_plus_assert_op(ae,==,text_complex::access::api_error::Success);
#endif /*TextComplexAccessP_NO_EXCEPT*/
  /* inspect the new code lengths */{
    size_t i;
    size_t const len = p->size();
    size_t nonzero_count = 0u;
    unsigned long int sum = 0u;
    munit_plus_logf(MUNIT_PLUS_LOG_DEBUG,
      "total %" MUNIT_PLUS_SIZE_MODIFIER "u", len);
    for (i = 0; i < len; ++i) {
      struct text_complex::access::prefix_line const& line = (*p)[i];
      munit_plus_logf(MUNIT_PLUS_LOG_DEBUG,
        "  [%" MUNIT_PLUS_SIZE_MODIFIER "u] = {-, l %u, v %lu}",
        i, line.len, line.value);
      if (ph[i] > 0) {
        munit_plus_assert_uint(line.len, >, 0u);
        munit_plus_assert_uint(line.len, <=, 15u);
        sum += (32768u>>line.len);
        nonzero_count += 1u;
      } else {
        munit_plus_assert_uint(line.len, ==, 0u);
      }
    }
    if (nonzero_count == 0) {
      munit_plus_assert_ulong(sum, ==, 0);
    } else if (nonzero_count == 1) {
      munit_plus_assert_ulong(sum, ==, 16384);
    } else munit_plus_assert_ulong(sum, ==, 32768);
  }
  return MUNIT_PLUS_OK;
}




MunitPlusResult test_fixlist_hist_cycle
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::prefix_histogram* ptr[2];
  (void)params;
  (void)data;
  ptr[0] = text_complex::access::fixlist_histogram_new(288);
  ptr[1] = new text_complex::access::prefix_histogram(288);
  std::unique_ptr<text_complex::access::prefix_histogram> ptr2 =
      text_complex::access::fixlist_histogram_unique(288);
  munit_plus_assert_not_null(ptr[0]);
  munit_plus_assert_not_null(ptr[1]);
  munit_plus_assert_not_null(ptr2.get());
  munit_plus_assert_ptr_not_equal(ptr[0],ptr[1]);
  munit_plus_assert_ptr_not_equal(ptr[0],ptr2.get());
  text_complex::access::fixlist_histogram_destroy(ptr[0]);
  delete ptr[1];
  return MUNIT_PLUS_OK;
}

void* test_fixlist_hist_setup
    (const MunitPlusParameter params[], void* user_data)
{
  return text_complex::access::fixlist_histogram_new(
        static_cast<std::size_t>(munit_plus_rand_int_range(4,256))
      );
}




void test_fixlist_hist_teardown(void* fixture) {
  text_complex::access::fixlist_histogram_destroy(
      static_cast<text_complex::access::prefix_histogram*>(fixture)
    );
  return;
}

MunitPlusResult test_fixlist_hist_item
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::prefix_histogram* const p =
    static_cast<text_complex::access::prefix_histogram*>(data);
  text_complex::access::prefix_histogram const* const p_c = p;
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  text_complex::access::uint32 const* dsp[4];
  std::size_t sz = p->size();
  munit_plus_assert_size(sz,>=,4);
  munit_plus_assert_size(sz,<=,256);
  std::size_t i = static_cast<std::size_t>(
        munit_plus_rand_int_range(0,static_cast<int>(sz)-1)
      );
  dsp[0] = &(*p)[i];
  dsp[1] = &(*p_c)[i];
  dsp[2] = &p->at(i);
  dsp[3] = &p_c->at(i);
  munit_plus_assert_not_null(dsp[0]);
  munit_plus_assert_ptr(dsp[0],==,dsp[1]);
  munit_plus_assert_ptr(dsp[0],==,dsp[2]);
  munit_plus_assert_ptr(dsp[0],==,dsp[3]);
  return MUNIT_PLUS_OK;
}





int main(int argc, char **argv) {
  return munit_plus_suite_main(&suite_fixlist, nullptr, argc, argv);
}
