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
  (std::ostream &, text_complex::access::insert_copy_type t);

static MunitPlusResult test_inscopy_cycle
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_inscopy_item
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_inscopy_item_c
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
  {(char*)"item_const", test_inscopy_item_c,
      test_inscopy_setup,test_inscopy_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {nullptr, nullptr, nullptr,nullptr,MUNIT_PLUS_TEST_OPTION_NONE,nullptr}
};

static MunitPlusSuite const suite_inscopy = {
  (char*)"access/inscopy/", tests_inscopy, nullptr,
      1, MUNIT_PLUS_SUITE_OPTION_NONE
};



std::ostream& operator<<
  (std::ostream &o, text_complex::access::insert_copy_type t)
{
  std::ostream::sentry s(o);
  if (s) {
    switch (t) {
    case text_complex::access::insert_copy_type::Literal:
      o << "Literal";
      break;
    case text_complex::access::insert_copy_type::Stop:
      o << "Stop";
      break;
    case text_complex::access::insert_copy_type::Insert:
      o << "Insert";
      break;
    case text_complex::access::insert_copy_type::InsertCopy:
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
  std::size_t const t = testfont_rand_size_range(0u,256u);
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
  text_complex::access::insert_copy_preset const t =
      static_cast<text_complex::access::insert_copy_preset>(
          munit_plus_rand_int_range(0,2)
        );
  std::unique_ptr<text_complex::access::insert_copy_table> p =
    text_complex::access::inscopy_unique();
  if (p) {
    try {
      text_complex::access::inscopy_preset(*p, t);
    } catch (text_complex::access::api_exception const& ae) {
      munit_plus_logf(MUNIT_PLUS_LOG_WARNING, "setup api_exception: \"%s\"",
        ae.what());
      p.reset(nullptr);
    }
  }
  return p.release();
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
  /* consistency check */{
    std::size_t const i = testfont_rand_size_range(0,p->size());
    munit_plus_assert_ptr_equal(&(*p)[i], &(*p_c)[i]);
  }
  switch (p->size()) {
  case 286: /* Deflate */
    {
      /* test literals */{
        std::size_t const i = testfont_rand_size_range(0,255);
        struct text_complex::access::insert_copy_row& row = (*p)[i];
        munit_plus_assert_op
          (row.type, ==, text_complex::access::insert_copy_type::Literal);
        munit_plus_assert_size(row.code, ==, i);
      }
      /* test stop */{
        struct text_complex::access::insert_copy_row& row = (*p)[256];
        munit_plus_assert_op
          (row.type, ==, text_complex::access::insert_copy_type::Stop);
        munit_plus_assert_ushort(row.code, ==, 256u);
      }
      /* test length */{
        std::size_t i = testfont_rand_size_range(257,285);
        struct text_complex::access::insert_copy_row& row = (*p)[i];
        munit_plus_assert_op
          (row.type, ==, text_complex::access::insert_copy_type::Insert);
        munit_plus_assert_size(row.code, ==, i);
        munit_plus_assert_uint(row.insert_bits, <=, 5);
        munit_plus_logf(MUNIT_PLUS_LOG_DEBUG, "[%u] = {bits: %u, first: %u}",
          static_cast<unsigned int>(i),
          row.insert_bits, row.insert_first);
      }
    }break;
  case 704: /* Brotli Insert-Copy */
    {
      std::size_t i = testfont_rand_size_range(0,703);
      struct text_complex::access::insert_copy_row& row = (*p)[i];
      munit_plus_assert_size(row.code, ==, i);
      munit_plus_assert_int(row.zero_distance_tf, ==, i<128);
      munit_plus_assert_op
        (row.type, ==, text_complex::access::insert_copy_type::InsertCopy);
      munit_plus_logf(MUNIT_PLUS_LOG_DEBUG,
        "[%u] = {bits: %u, first: %u, copy_bits: %u, copy_first: %u}",
        static_cast<unsigned int>(i),
        row.insert_bits, row.insert_first,
        row.copy_bits, row.copy_first);
    }break;
  case 26: /* Brotli Block Count */
    {
      std::size_t i = testfont_rand_size_range(0,25);
      struct text_complex::access::insert_copy_row& row = (*p)[i];
      munit_plus_assert_op
        (row.type, ==, text_complex::access::insert_copy_type::BlockCount);
      munit_plus_assert_size(row.code, ==, i);
      munit_plus_assert_uint(row.insert_bits, <=, 24);
      munit_plus_logf(MUNIT_PLUS_LOG_DEBUG, "[%u] = {bits: %u, first: %u}",
        static_cast<unsigned int>(i),
        row.insert_bits, row.insert_first);
    }break;
  default:
    munit_plus_errorf(
      "Unexpected table length %" MUNIT_PLUS_SIZE_MODIFIER "u.",
      p->size()
      );
  }
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_inscopy_item_c
  (const MunitPlusParameter params[], void* data)
{
  text_complex::access::insert_copy_table const* const p =
    static_cast<text_complex::access::insert_copy_table*>(data);
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  switch (p->size()) {
  case 286: /* Deflate */
    {
      /* test literals */{
        struct text_complex::access::insert_copy_row const& row =
          (*p)[testfont_rand_size_range(0,255)];
        munit_plus_assert_op
          (row.type, ==, text_complex::access::insert_copy_type::Literal);
      }
      /* test stop */{
        struct text_complex::access::insert_copy_row const& row = (*p)[256];
        munit_plus_assert_op
          (row.type, ==, text_complex::access::insert_copy_type::Stop);
      }
      /* test length */{
        std::size_t i = testfont_rand_size_range(257,285);
        struct text_complex::access::insert_copy_row const& row = (*p)[i];
        munit_plus_assert_op
          (row.type, ==, text_complex::access::insert_copy_type::Insert);
        munit_plus_assert_uint(row.insert_bits, <=, 5);
        munit_plus_logf(MUNIT_PLUS_LOG_DEBUG, "[%u] = {bits: %u, first: %u}",
          static_cast<unsigned int>(i),
          row.insert_bits, row.insert_first);
      }
    }break;
  case 704: /* Brotli Insert-Copy */
    {
      std::size_t i = testfont_rand_size_range(0,703);
      struct text_complex::access::insert_copy_row const& row = (*p)[i];
      munit_plus_assert_int(row.zero_distance_tf, ==, i<128);
      munit_plus_assert_op
        (row.type, ==, text_complex::access::insert_copy_type::InsertCopy);
      munit_plus_logf(MUNIT_PLUS_LOG_DEBUG,
        "[%u] = {bits: %u, first: %u, copy_bits: %u, copy_first: %u}",
        static_cast<unsigned int>(i),
        row.insert_bits, row.insert_first,
        row.copy_bits, row.copy_first);
    }break;
  case 26: /* Brotli Block Count */
    {
      std::size_t i = testfont_rand_size_range(0,25);
      struct text_complex::access::insert_copy_row const& row = (*p)[i];
      munit_plus_assert_op
        (row.type, ==, text_complex::access::insert_copy_type::BlockCount);
      munit_plus_assert_size(row.code, ==, i);
      munit_plus_assert_uint(row.insert_bits, <=, 24);
      munit_plus_logf(MUNIT_PLUS_LOG_DEBUG, "[%u] = {bits: %u, first: %u}",
        static_cast<unsigned int>(i),
        row.insert_bits, row.insert_first);
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
