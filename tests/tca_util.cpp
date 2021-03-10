
#include "text-complex-plus/access/util.hpp"
#include "munit-plus/munit.hpp"
#include <new>
#include <memory>
#include <utility>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifndef NDEBUG
#  define test_sharp2(s) #s
#  define test_sharp(s) test_sharp2(s)
#  define test_break(s) \
     { munit_error(s); return MUNIT_FAIL; }
#else
#  define test_break(s)
#endif /*NDEBUG*/

typedef int (*test_fn)(void);

struct x_with_a_dtor {
  int *x;
  int o;
  x_with_a_dtor(void) : x(nullptr), o(7) {}
  x_with_a_dtor(int *y, int v) : x(y), o(v) {}
  ~x_with_a_dtor() {
    if (x){ (*x)+=o;}
  }
};

static MunitPlusResult test_util_alloc
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_util_unique_cycle
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_util_unique_stdptr
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_util_unique_access
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_util_unique_a_cycle
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_util_unique_a_stdptr
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_util_unique_a_index
    (const MunitPlusParameter params[], void* data);

static MunitPlusTest tests_util[] = {
  {(char*)"alloc", test_util_alloc, nullptr,nullptr,
    MUNIT_PLUS_TEST_OPTION_NONE,nullptr},
  {(char*)"unique/cycle", test_util_unique_cycle, nullptr,nullptr,
    MUNIT_PLUS_TEST_OPTION_NONE,nullptr},
  {(char*)"unique/stdptr", test_util_unique_stdptr, nullptr,nullptr,
    MUNIT_PLUS_TEST_OPTION_NONE,nullptr},
  {(char*)"unique/access", test_util_unique_access, nullptr,nullptr,
    MUNIT_PLUS_TEST_OPTION_NONE,nullptr},
  {(char*)"unique_a/cycle", test_util_unique_a_cycle, nullptr,nullptr,
    MUNIT_PLUS_TEST_OPTION_NONE,nullptr},
  {(char*)"unique_a/stdptr", test_util_unique_a_stdptr, nullptr,nullptr,
    MUNIT_PLUS_TEST_OPTION_NONE,nullptr},
  {(char*)"unique_a/index", test_util_unique_a_index, nullptr,nullptr,
    MUNIT_PLUS_TEST_OPTION_NONE,nullptr},
  {nullptr, nullptr, nullptr,nullptr, MUNIT_PLUS_TEST_OPTION_NONE,nullptr}
};

static MunitPlusSuite const suite_util = {
  (char*)"access/util/", tests_util, nullptr, 1, MUNIT_PLUS_SUITE_OPTION_NONE
};



MunitPlusResult test_util_alloc
  (const MunitPlusParameter params[], void* data)
{
  (void)params;
  (void)data;
  void* ptr[3];
  unsigned long int lg = munit_plus_rand_uint32();
  ptr[0] = text_complex::access::util_op_new(0u);
  munit_plus_assert_ptr_null(ptr[0]);
  ptr[0] = text_complex::access::util_op_new(sizeof(unsigned long int));
  munit_plus_assert_ptr_not_null(ptr[0]);
  /* store and retrieve number */{
    unsigned long int* const lgptr = static_cast<unsigned long int*>(ptr[0]);
    *lgptr = lg;
    munit_plus_assert_ulong(*lgptr,==,lg);
  }
  text_complex::access::util_op_delete(ptr[0]);
  text_complex::access::util_op_delete(nullptr);
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_util_unique_cycle
    (const MunitPlusParameter params[], void* data)
{
  int x = 2;
  (void)params;
  (void)data;
  /* scope */{
    text_complex::access::util_unique_ptr<x_with_a_dtor> p;
    /* make a new x */try {
      x_with_a_dtor* xd;
      xd = new x_with_a_dtor(&x, 5);
      p.reset(xd);
    } catch (std::bad_alloc const&) {
      return MUNIT_PLUS_SKIP;
    }
  }
  munit_plus_assert_int(x,==,7);
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_util_unique_stdptr
    (const MunitPlusParameter params[], void* data)
{
  int x = 3;
  /* scope */{
    x_with_a_dtor* xd;
    std::unique_ptr<x_with_a_dtor> s1;
    /* make a new x */try {
      xd = new x_with_a_dtor(&x,8);
      s1.reset(xd);
    } catch (std::bad_alloc const&) {
      return MUNIT_PLUS_SKIP;
    }
    munit_plus_assert_ptr_equal(s1.get(), xd);
    munit_plus_assert_int(x,==,3);
    text_complex::access::util_unique_ptr<x_with_a_dtor> p1(std::move(s1));
    munit_plus_assert_ptr_equal(p1.get(), xd);
    munit_plus_assert_int(x,==,3);
    s1 = std::move(p1);
    munit_plus_assert_ptr_equal(s1.get(), xd);
    munit_plus_assert_ptr_equal(p1.get(), nullptr);
    munit_plus_assert_int(x,==,3);
    p1 = std::move(s1);
    munit_plus_assert_ptr_equal(p1.get(), xd);
    munit_plus_assert_int(x,==,3);
    switch (munit_plus_rand_int_range(0,3)) {
    case 0:
      break;
    case 1:
      {
      std::unique_ptr<x_with_a_dtor> s2(std::move(p1));
      munit_plus_assert_ptr_equal(s2.get(), xd);
      munit_plus_assert_ptr_equal(p1.get(), nullptr);
      munit_plus_assert_int(x,==,3);
      } break;
    case 2:
      {
        text_complex::access::util_unique_ptr<
            x_with_a_dtor
          > p2(std::move(p1));
        munit_plus_assert_ptr_equal(p2.get(), xd);
        munit_plus_assert_ptr_equal(p1.get(), nullptr);
        munit_plus_assert_int(x,==,3);
      }break;
    case 3:
      {
        text_complex::access::util_unique_ptr<
            x_with_a_dtor
          > p2;
        p2 = std::move(p1);
        munit_plus_assert_ptr_equal(p2.get(), xd);
        munit_plus_assert_ptr_equal(p1.get(), nullptr);
        munit_plus_assert_int(x,==,3);
      }break;
    }
  }
  munit_plus_assert_int(x,==,11);
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_util_unique_access
    (const MunitPlusParameter params[], void* data)
{
  int y = munit_plus_rand_int_range(1,1024);
  int x = 2;
  (void)params;
  (void)data;
  /* scope */{
    text_complex::access::util_unique_ptr<x_with_a_dtor> p;
    /* make a new x */try {
      x_with_a_dtor* xd;
      xd = new x_with_a_dtor(&x, y);
      p.reset(xd);
    } catch (std::bad_alloc const&) {
      return MUNIT_PLUS_SKIP;
    }
    munit_plus_assert_int(y,==,p->o);
    munit_plus_assert_int(y,==,(*p).o);
  }
  munit_plus_assert_int(x,==,2+y);
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_util_unique_a_cycle
    (const MunitPlusParameter params[], void* data)
{
  int x = 2;
  (void)params;
  (void)data;
  /* scope */{
    text_complex::access::util_unique_ptr<x_with_a_dtor[]> p;
    /* make new exes */try {
      int i;
      x_with_a_dtor* xd;
      xd = new x_with_a_dtor[5];
      for (i = 0; i < 5; ++i) {
        xd[i].x = &x;
        xd[i].o = i+1;
      }
      p.reset(xd);
    } catch (std::bad_alloc const&) {
      return MUNIT_PLUS_SKIP;
    }
  }
  munit_plus_assert_int(x,==,17);
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_util_unique_a_stdptr
    (const MunitPlusParameter params[], void* data)
{
  int x = 3;
  /* scope */{
    x_with_a_dtor* xd;
    std::unique_ptr<x_with_a_dtor[]> s1;
    /* make a new x */try {
      int i;
      xd = new x_with_a_dtor[5];
      for (i = 0; i < 5; ++i) {
        xd[i].x = &x;
        xd[i].o = (i+1)*(i+1);
      }
      s1.reset(xd);
    } catch (std::bad_alloc const&) {
      return MUNIT_PLUS_SKIP;
    }
    munit_plus_assert_ptr_equal(s1.get(), xd);
    munit_plus_assert_int(x,==,3);
    text_complex::access::util_unique_ptr<x_with_a_dtor[]> p1(std::move(s1));
    munit_plus_assert_ptr_equal(p1.get(), xd);
    munit_plus_assert_int(x,==,3);
    s1 = std::move(p1);
    munit_plus_assert_ptr_equal(s1.get(), xd);
    munit_plus_assert_ptr_equal(p1.get(), nullptr);
    munit_plus_assert_int(x,==,3);
    p1 = std::move(s1);
    munit_plus_assert_ptr_equal(p1.get(), xd);
    munit_plus_assert_int(x,==,3);
    switch (munit_plus_rand_int_range(0,3)) {
    case 0:
      break;
    case 1:
      {
        std::unique_ptr<x_with_a_dtor[]> s2(std::move(p1));
        munit_plus_assert_ptr_equal(s2.get(), xd);
        munit_plus_assert_ptr_equal(p1.get(), nullptr);
        munit_plus_assert_int(x,==,3);
      } break;
    case 2:
      {
        text_complex::access::util_unique_ptr<
            x_with_a_dtor[]
          > p2(std::move(p1));
        munit_plus_assert_ptr_equal(p2.get(), xd);
        munit_plus_assert_ptr_equal(p1.get(), nullptr);
        munit_plus_assert_int(x,==,3);
      }break;
    case 3:
      {
        text_complex::access::util_unique_ptr<
            x_with_a_dtor[]
          > p2;
        p2 = std::move(p1);
        munit_plus_assert_ptr_equal(p2.get(), xd);
        munit_plus_assert_ptr_equal(p1.get(), nullptr);
        munit_plus_assert_int(x,==,3);
      }break;
    }
  }
  munit_plus_assert_int(x,==,58);
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_util_unique_a_index
    (const MunitPlusParameter params[], void* data)
{
  unsigned int nums[20];
  int sz = munit_plus_rand_int_range(1,20);
  unsigned int *arr;
  munit_plus_rand_memory_ex(nums);
  text_complex::access::util_unique_ptr<unsigned int[]> p;
  try {
    int i;
    arr = new unsigned int[sz];
    for (i = 0; i < sz; ++i) {
      arr[i] = nums[i];
    }
    p.reset(arr);
  } catch (std::bad_alloc const& ) {
    return MUNIT_PLUS_SKIP;
  }
  munit_plus_assert_ptr_equal(p.get(), arr);
  munit_plus_assert_ptr_not_equal(p.get(), nums);
  munit_plus_assert_ptr_equal(&p[0], arr);
  /* compare array */{
    int i;
    for (i = 0; i < sz; ++i) {
      munit_plus_assert_uint(p[i],==,nums[i]);
    }
  }
  return MUNIT_PLUS_OK;
}



int main(int argc, char **argv) {
  return munit_plus_suite_main(&suite_util, nullptr, argc, argv);
}
