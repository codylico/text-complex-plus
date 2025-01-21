/**
 * @brief Test program for prefix list
 */
#include "testfont.hpp"
#include "text-complex-plus/access/gaspvec.hpp"
#include "munit-plus/munit.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>


static MunitPlusResult test_gaspvec_cycle
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_gaspvec_item
    (const MunitPlusParameter params[], void* data);
static void* test_gaspvec_setup
    (const MunitPlusParameter params[], void* user_data);
static void test_gaspvec_teardown(void* fixture);

namespace tca = text_complex::access;


static MunitPlusTest tests_fixlist[] = {
  {(char*)"cycle", test_gaspvec_cycle,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_SINGLE_ITERATION,
      nullptr},
  {(char*)"item", test_gaspvec_item,
      test_gaspvec_setup,test_gaspvec_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {nullptr, nullptr, nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,nullptr}
};

static MunitPlusSuite const suite_fixlist = {
  (char*)"access/fixlist/", tests_fixlist, nullptr,
      1, MUNIT_PLUS_SUITE_OPTION_NONE
};




MunitPlusResult test_gaspvec_cycle
  (const MunitPlusParameter params[], void* data)
{
  tca::gasp_vector* ptr[2];
  (void)params;
  (void)data;
  ptr[0] = tca::gaspvec_new(128);
  ptr[1] = new tca::gasp_vector(128);
  std::unique_ptr<tca::gasp_vector> ptr2 = tca::gaspvec_unique(128);
  munit_plus_assert(ptr[0] != nullptr);
  munit_plus_assert(ptr[1] != nullptr);
  munit_plus_assert(ptr2.get() != nullptr);
  munit_plus_assert(ptr[0] != ptr[1]);
  munit_plus_assert(ptr[0] != ptr2.get());
  tca::gaspvec_destroy(ptr[0]);
  delete ptr[1];
  return MUNIT_PLUS_OK;
}

void* test_gaspvec_setup(const MunitPlusParameter params[], void* user_data) {
  return tca::gaspvec_new(
        static_cast<std::size_t>(munit_plus_rand_int_range(4,256))
      );
}

void test_gaspvec_teardown(void* fixture) {
  tca::gaspvec_destroy(
      static_cast<tca::gasp_vector*>(fixture)
    );
  return;
}

MunitPlusResult test_gaspvec_item
  (const MunitPlusParameter params[], void* data)
{
  tca::gasp_vector* const p =
    static_cast<tca::gasp_vector*>(data);
  tca::gasp_vector const* const p_c = p;
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  tca::prefix_list const* dsp[4];
  std::size_t sz = p->size();
  munit_plus_assert(sz >= 4);
  munit_plus_assert(sz <= 256);
  std::size_t i = static_cast<std::size_t>(
        munit_plus_rand_int_range(0,static_cast<int>(sz)-1)
      );
  dsp[0] = &(*p)[i];
  dsp[1] = &(*p_c)[i];
  dsp[2] = &p->at(i);
  dsp[3] = &p_c->at(i);
  munit_plus_assert(dsp[0] != nullptr);
  munit_plus_assert(dsp[0] == dsp[1]);
  munit_plus_assert(dsp[0] == dsp[2]);
  munit_plus_assert(dsp[0] == dsp[3]);
  return MUNIT_PLUS_OK;
}



int main(int argc, char **argv) {
  return munit_plus_suite_main(&suite_fixlist, nullptr, argc, argv);
}
