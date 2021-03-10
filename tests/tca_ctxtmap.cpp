/**
 * @brief Test program for context map
 */
#include "testfont.hpp"
#include "text-complex-plus/access/ctxtmap.hpp"
#include "munit-plus/munit.hpp"
#include <memory>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>


static MunitPlusResult test_ctxtmap_cycle
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_ctxtmap_item
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_ctxtmap_distcontext
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_ctxtmap_litcontext
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_ctxtmap_imtf
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_ctxtmap_mtf
    (const MunitPlusParameter params[], void* data);
static void* test_ctxtmap_setup
    (const MunitPlusParameter params[], void* user_data);
static void test_ctxtmap_teardown(void* fixture);

/* NOTE from RFC7932 */
static void InverseMoveToFrontTransform
  (munit_plus_uint8_t* v, int v_len);


static MunitPlusTest tests_ctxtmap[] = {
  {(char*)"cycle", test_ctxtmap_cycle,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_SINGLE_ITERATION,
      nullptr},
  {(char*)"item", test_ctxtmap_item,
      test_ctxtmap_setup,test_ctxtmap_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"distance_context", test_ctxtmap_distcontext,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"literal_context", test_ctxtmap_litcontext,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"imtf", test_ctxtmap_imtf,
      test_ctxtmap_setup,test_ctxtmap_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"mtf", test_ctxtmap_mtf,
      test_ctxtmap_setup,test_ctxtmap_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {nullptr, nullptr, nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,nullptr}
};

static MunitPlusSuite const suite_ctxtmap = {
  (char*)"access/ctxtmap/", tests_ctxtmap, nullptr,
      1, MUNIT_PLUS_SUITE_OPTION_NONE
};


/* NOTE adapted from RFC7932 */
void InverseMoveToFrontTransform(munit_plus_uint8_t* v, int v_len) {
  munit_plus_uint8_t mtf[256];
  int i;
  for (i = 0; i < 256; ++i) {
     mtf[i] = static_cast<munit_plus_uint8_t>(i);
  }
  for (i = 0; i < v_len; ++i) {
     munit_plus_uint8_t index = v[i];
     munit_plus_uint8_t value = mtf[index];
     v[i] = value;
     for (; index; --index) {
        mtf[index] = mtf[index - 1];
     }
     mtf[0] = value;
  }
}


MunitPlusResult test_ctxtmap_cycle
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::context_map* ptr[2];
  (void)params;
  (void)data;
  ptr[0] = text_complex::access::ctxtmap_new();
  ptr[1] = new text_complex::access::context_map();
  std::unique_ptr<text_complex::access::context_map> ptr2 =
      text_complex::access::ctxtmap_unique();
  munit_plus_assert_not_null(ptr[0]);
  munit_plus_assert_not_null(ptr[1]);
  munit_plus_assert_not_null(ptr2.get());
  munit_plus_assert_ptr_not_equal(ptr[0],ptr[1]);
  munit_plus_assert_ptr_not_equal(ptr[0],ptr2.get());
  munit_plus_assert_ptr_not_equal(ptr[1],ptr2.get());
  text_complex::access::ctxtmap_destroy(ptr[0]);
  delete ptr[1];
  return MUNIT_PLUS_OK;
}

void* test_ctxtmap_setup(const MunitPlusParameter params[], void* user_data) {
  return text_complex::access::ctxtmap_new(
      testfont_rand_size_range(1,256), munit_plus_rand_int_range(0,1)?4:64
    );
}

void test_ctxtmap_teardown(void* fixture) {
  text_complex::access::ctxtmap_destroy(
      static_cast<struct text_complex::access::context_map*>(fixture)
    );
  return;
}

MunitPlusResult test_ctxtmap_item
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::context_map* const p =
    static_cast<text_complex::access::context_map*>(data);
  text_complex::access::context_map const* const p_c = p;
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  /* */{
    unsigned char* data_ptr = p->data();
    std::size_t j;
    munit_plus_assert_ptr_equal(data_ptr, p_c->data());
    for (j = 0; j < p->block_types(); ++j) {
      std::size_t i;
      for (i = 0; i < p->contexts(); ++i, ++data_ptr) {
        unsigned char const x =
          static_cast<unsigned char>(testfont_rand_int_range(0,255));
        p->at(j,i) = x;
        munit_plus_assert_ptr_equal(data_ptr, &(*p_c)(j,i));
        munit_plus_assert_ptr_equal(data_ptr, &(*p)(j,i));
        munit_plus_assert_ptr_equal(data_ptr, &p_c->at(j,i));
        munit_plus_assert_ptr_equal(data_ptr, &p->at(j,i));
        munit_plus_assert_uchar(x, ==, p_c->at(j,i));
      }
    }
  }
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_ctxtmap_distcontext
  (const MunitPlusParameter params[], void* data)
{
  unsigned long int len = munit_plus_rand_int_range(0,10);
  size_t ctxt;
  /* query the context */{
#if !(defined TextComplexAccessP_NO_EXCEPT)
    bool err_caught = false;
    try {
      ctxt = text_complex::access::ctxtmap_distance_context(len);
    } catch (text_complex::access::api_exception const& ae) {
      err_caught = true;
    }
    if (len < 2) {
      munit_plus_assert_true(err_caught);
    }
#else
    text_complex::access::api_error ae;
    ctxt = text_complex::access::ctxtmap_distance_context(len, ae);
    if (len < 2) {
      munit_plus_assert_int(ae,!=,text_complex::access::api_error::Success);
    }
#endif /*TextComplexAccessP_NO_EXCEPT*/
  }
  /* inspect the result */if (len >= 2) {
    switch (len) {
    case 2: munit_plus_assert_size(ctxt,==,0); break;
    case 3: munit_plus_assert_size(ctxt,==,1); break;
    case 4: munit_plus_assert_size(ctxt,==,2); break;
    default: munit_plus_assert_size(ctxt,==,3); break;
    }
  }
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_ctxtmap_litcontext
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::context_map_mode const m =
     static_cast<text_complex::access::context_map_mode>(
        munit_plus_rand_int_range(0,4)
      );
  munit_plus_uint8_t hist[2];
  size_t ctxt;
  munit_plus_rand_memory(sizeof(hist), hist);
  /* query the context */{
#if !(defined TextComplexAccessP_NO_EXCEPT)
    bool err_caught = false;
    try {
      ctxt = text_complex::access::ctxtmap_literal_context
          (m, hist[0], hist[1]);
    } catch (text_complex::access::api_exception const& ae) {
      err_caught = true;
    }
    if (static_cast<int>(m) >= 4) {
      munit_plus_assert_true(err_caught);
    }
#else
    text_complex::access::api_error ae;
    ctxt = text_complex::access::ctxtmap_literal_context
        (m, hist[0], hist[1], ae);
    if (static_cast<int>(m) >= 4) {
      munit_plus_assert_int(ae,!=,text_complex::access::api_error::Success);
    }
#endif /*TextComplexAccessP_NO_EXCEPT*/
  }
  /* inspect the result */if (static_cast<int>(m) < 4) {
    switch (m) {
    case text_complex::access::context_map_mode::LSB6:
      munit_plus_assert_size(ctxt,==,hist[0]&0x3f);
      break;
    case text_complex::access::context_map_mode::MSB6:
      munit_plus_assert_size(ctxt,==,(hist[0]>>2));
      break;
    case text_complex::access::context_map_mode::UTF8:
    case text_complex::access::context_map_mode::Signed:
      /* ? */
      break;
    }
  }
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_ctxtmap_imtf
    (const MunitPlusParameter params[], void* data)
{
  text_complex::access::context_map* const p =
    static_cast<text_complex::access::context_map*>(data);
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  /* */{
    std::size_t const isize = p->contexts() * p->block_types();
    unsigned char* const data = p->data();
    std::vector<munit_plus_uint8_t> mem(isize*sizeof(unsigned char));
    munit_plus_rand_memory(isize, data);
    std::memcpy(&mem[0], data, isize*sizeof(unsigned char));
    /* apply the inverse transform */{
      text_complex::access::ctxtmap_revert_movetofront(*p);
      InverseMoveToFrontTransform(&mem[0], isize);
    }
    munit_plus_assert_memory_equal(isize, data, &mem[0]);
  }
  return MUNIT_PLUS_OK;
}
MunitPlusResult test_ctxtmap_mtf
    (const MunitPlusParameter params[], void* data)
{
  text_complex::access::context_map* const p =
    static_cast<text_complex::access::context_map*>(data);
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  /* */{
    std::size_t const isize = p->contexts() * p->block_types();
    unsigned char* const data = p->data();
    std::vector<munit_plus_uint8_t> mem(isize*sizeof(unsigned char));
    munit_plus_rand_memory(isize, data);
    std::memcpy(&mem[0], data, isize*sizeof(unsigned char));
    /* apply the inverse transform */{
      text_complex::access::ctxtmap_apply_movetofront(*p);
      InverseMoveToFrontTransform(data, isize);
    }
    munit_plus_assert_memory_equal(isize, data, &mem[0]);
  }
  return MUNIT_PLUS_OK;
}


int main(int argc, char **argv) {
  return munit_plus_suite_main(&suite_ctxtmap, nullptr, argc, argv);
}
