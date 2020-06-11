/**
 * @brief Test program for insert copy table
 */
#include "../tcmplx-access-plus/inscopy.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "munit-plus/munit.hpp"
#include <memory>
#include "testfont.hpp"



static std::ostream& operator<<
  (std::ostream &, text_complex::access::inscopy_type t);

static MunitPlusResult test_inscopy_cycle
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_inscopy_item
    (const MunitPlusParameter params[], void* data);
static void* test_inscopy_setup
    (const MunitPlusParameter params[], void* user_data);
static void test_inscopy_teardown(void* fixture);


static MunitPlusTest tests_inscopy[] = {
  {(char*)"cycle", test_inscopy_cycle,
      nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"item", test_inscopy_item,
      test_inscopy_setup,test_inscopy_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {nullptr, nullptr, nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,nullptr}
};

static MunitPlusSuite const suite_inscopy = {
  (char*)"access/inscopy/", tests_inscopy, nullptr,
      1, MUNIT_PLUS_SUITE_OPTION_NONE
};



std::ostream& operator<<
  (std::ostream &o, text_complex::access::inscopy_type t)
{
  std::ostream::sentry s(o);
  if (s) {
    switch (t) {
    case text_complex::access::inscopy_type::Literal:
      o << "Literal";
      break;
    case text_complex::access::inscopy_type::Stop:
      o << "Stop";
      break;
    case text_complex::access::inscopy_type::Insert:
      o << "Insert";
      break;
    case text_complex::access::inscopy_type::InsertCopy:
      o << "InsertCopy";
      break;
    default:
      o << static_cast<unsigned int>(static_cast<unsigned char>(t));
      break;
    }
  }
  return o;
}

MunitPlusResult test_inscopy_cycle
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::insert_copy_table* ptr[2];
  (void)params;
  (void)data;
  text_complex::access::inscopy_preset const t =
      munit_plus_rand_int_range(0,1)
    ? text_complex::access::inscopy_preset::Deflate
    : text_complex::access::inscopy_preset::Brotli;
  ptr[0] = text_complex::access::inscopy_new(t);
  ptr[1] = new text_complex::access::insert_copy_table(t);
  std::unique_ptr<text_complex::access::insert_copy_table> ptr2 =
      text_complex::access::inscopy_unique(t);
  munit_plus_assert_not_null(ptr[0]);
  munit_plus_assert_not_null(ptr[1]);
  munit_plus_assert_not_null(ptr2.get());
  munit_plus_assert_ptr_not_equal(ptr[0],ptr[1]);
  munit_plus_assert_ptr_not_equal(ptr[0],ptr2.get());
  munit_plus_assert_ptr_not_equal(ptr[1],ptr2.get());
  text_complex::access::inscopy_destroy(ptr[0]);
  delete ptr[1];
  return MUNIT_PLUS_OK;
}

void* test_inscopy_setup(const MunitPlusParameter params[], void* user_data) {
  text_complex::access::inscopy_preset const t =
      munit_plus_rand_int_range(0,1)
    ? text_complex::access::inscopy_preset::Deflate
    : text_complex::access::inscopy_preset::Brotli;
  return text_complex::access::inscopy_new(t);
}

void test_inscopy_teardown(void* fixture) {
  text_complex::access::inscopy_destroy(
      static_cast<struct text_complex::access::insert_copy_table*>(fixture)
    );
  return;
}

MunitPlusResult test_inscopy_item
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::insert_copy_table* const p =
    static_cast<text_complex::access::insert_copy_table*>(data);
  text_complex::access::insert_copy_table const* const p_c = p;
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  switch (p->size()) {
  case 286: /* Deflate */
    {
      /* test literals */{
        struct text_complex::access::inscopy_row const& row =
          (*p)[testfont_rand_size_range(0,255)];
        munit_plus_assert_op
          (row.type, ==, text_complex::access::inscopy_type::Literal);
      }
      /* test stop */{
        struct text_complex::access::inscopy_row const& row = (*p)[256];
        munit_plus_assert_op
          (row.type, ==, text_complex::access::inscopy_type::Stop);
      }
      /* test length */{
        size_t i = testfont_rand_size_range(257,285);
        struct text_complex::access::inscopy_row const& row = (*p)[i];
        munit_plus_assert_op
          (row.type, ==, text_complex::access::inscopy_type::Insert);
        munit_plus_assert_uint(row.insert_bits, <=, 5);
        munit_plus_logf(MUNIT_PLUS_LOG_DEBUG, "[%u] = {bits: %u, first: %u}",
          static_cast<unsigned int>(i),
          row.insert_bits, row.insert_first);
      }
    }break;
  case 704: /* Brotli */
    {
      size_t i = testfont_rand_size_range(0,703);
      struct text_complex::access::inscopy_row const& row = (*p)[i];
      munit_plus_assert_int(row.zero_distance_tf, ==, i<128);
      munit_plus_assert_op
        (row.type, ==, text_complex::access::inscopy_type::InsertCopy);
      munit_plus_logf(MUNIT_PLUS_LOG_DEBUG,
        "[%u] = {bits: %u, first: %u, copy_bits: %u, copy_first: %u}",
        static_cast<unsigned int>(i),
        row.insert_bits, row.insert_first,
        row.copy_bits, row.copy_first);
    }break;
  default:
    munit_plus_errorf(
      "Unexpected table length %" MUNIT_PLUS_SIZE_MODIFIER "u.",
      p->size()
      );
  }
  return MUNIT_PLUS_OK;
}


int main(int argc, char **argv) {
  return munit_plus_suite_main(&suite_inscopy, nullptr, argc, argv);
}
