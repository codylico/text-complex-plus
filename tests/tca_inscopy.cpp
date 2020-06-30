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
static MunitPlusResult test_inscopy_lengthsort
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_inscopy_codesort
    (const MunitPlusParameter params[], void* data);
static MunitPlusResult test_inscopy_encode
    (const MunitPlusParameter params[], void* data);
static void* test_inscopy_setup
    (const MunitPlusParameter params[], void* user_data);
static void test_inscopy_teardown(void* fixture);
static unsigned long int test_inscopy_rand_length25(void);


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
  {(char*)"lengthsort", test_inscopy_lengthsort,
      test_inscopy_setup,test_inscopy_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"codesort", test_inscopy_codesort,
      test_inscopy_setup,test_inscopy_teardown,MUNIT_PLUS_TEST_OPTION_NONE,
      nullptr},
  {(char*)"encode", test_inscopy_encode,
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

unsigned long int test_inscopy_rand_length25(void) {
  /*
   * Non-uniform sampling of ranges [0,8192) | [8192,16801792)
   */
  unsigned long int const out = testfont_rand_uint_range(0u,16391u);
  constexpr unsigned long int remap_diff = 16777216lu - 8192lu;
  return (out >= 8192u)
    ? ((out<<11) + testfont_rand_uint_range(0u,2047u) - remap_diff)
    : out;
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

MunitPlusResult test_inscopy_codesort
    (const MunitPlusParameter params[], void* data)
{
  text_complex::access::insert_copy_table* const p =
    static_cast<text_complex::access::insert_copy_table*>(data);
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  /* run code-sort */{
#if !(defined TextComplexAccessP_NO_EXCEPT)
    text_complex::access::inscopy_codesort(*p);
#else
    text_complex::access::api_error ae;
    text_complex::access::inscopy_codesort(*p, ae);
    munit_plus_assert_int(ae,==,text_complex::access::api_error::Success);
#endif /*TextComplexAccessP_NO_EXCEPT*/
  }
  /* inspect */if (p->size() >= 2) {
    size_t i = testfont_rand_size_range(0,p->size()-2);
    text_complex::access::insert_copy_row const& first = (*p)[i];
    text_complex::access::insert_copy_row const& second = (*p)[i+1];
    munit_plus_assert_uint(first.code, <, second.code);
  }
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_inscopy_lengthsort
    (const MunitPlusParameter params[], void* data)
{
  text_complex::access::insert_copy_table* const p =
    static_cast<text_complex::access::insert_copy_table*>(data);
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  /* run length-sort */{
#if !(defined TextComplexAccessP_NO_EXCEPT)
    text_complex::access::inscopy_lengthsort(*p);
#else
    text_complex::access::api_error ae;
    text_complex::access::inscopy_lengthsort(*p, ae);
    munit_plus_assert_int(ae,==,text_complex::access::api_error::Success);
#endif /*TextComplexAccessP_NO_EXCEPT*/
  }
  /* inspect */if (p->size() >= 2) {
    size_t i = testfont_rand_size_range(0,p->size()-2);
    text_complex::access::insert_copy_row const& first = (*p)[i];
    text_complex::access::insert_copy_row const& second = (*p)[i+1];
    if (first.zero_distance_tf != second.zero_distance_tf) {
      munit_plus_assert_false(first.zero_distance_tf);
      munit_plus_assert_true(second.zero_distance_tf);
    } else if (first.insert_first != second.insert_first) {
      munit_plus_assert_uint(first.insert_first, <, second.insert_first);
    } else {
      munit_plus_assert_uint(first.copy_first, <=, second.copy_first);
    }
  }
  return MUNIT_PLUS_OK;
}

MunitPlusResult test_inscopy_encode
    (const MunitPlusParameter params[], void* data)
{
  /*
   * DEFLATE insert max: 258
   * Brotli insert max: 16799809
   * Brotli copy max: 16779333
   * Brotli block-count max: 16793840
   */
  constexpr std::size_t not_found = static_cast<std::size_t>(-1);
  text_complex::access::insert_copy_table* const p =
    static_cast<text_complex::access::insert_copy_table*>(data);
  if (p == nullptr)
    return MUNIT_PLUS_SKIP;
  (void)params;
  /* run length-sort */{
    text_complex::access::api_error ae;
    text_complex::access::inscopy_lengthsort(*p, ae);
    if (ae != text_complex::access::api_error::Success)
      return MUNIT_PLUS_SKIP;
  }
  switch (p->size()) {
  case 704: /* Brotli insert-copy pairs */{
      unsigned long int const ins_len = test_inscopy_rand_length25();
      unsigned long int const cpy_len = test_inscopy_rand_length25();
      bool const zero_dist = (munit_plus_rand_int_range(0,1) != 0);
      bool const expect_success =
        (ins_len <= 16799809ul && cpy_len >= 2 && cpy_len <= 16779333ul);
      std::size_t encode_index =
        text_complex::access::inscopy_encode(*p, ins_len, cpy_len, zero_dist);
      if ((encode_index == not_found) && zero_dist) {
        munit_plus_log
            (MUNIT_PLUS_LOG_DEBUG, "Retrying without zero distance.");
        encode_index =
          text_complex::access::inscopy_encode(*p, ins_len, cpy_len);
      }
      if ((encode_index != not_found) == expect_success) {
        if (expect_success) {
          text_complex::access::insert_copy_row const& row =
            (*p)[encode_index];
          unsigned long int const ins_extra = ins_len - row.insert_first;
          unsigned long int const cpy_extra = cpy_len - row.copy_first;
          munit_plus_logf(MUNIT_PLUS_LOG_DEBUG,
            "Encode (insert: %lu, copy: %lu, zero: %s) as "
            "<%u, %lu:%u, %lu:%u (zero: %s)>",
            ins_len, cpy_len, zero_dist?"true":"false",
            row.code, ins_extra, row.insert_bits, cpy_extra, row.copy_bits,
            row.zero_distance_tf?"true":"false");
        } else {
          munit_plus_logf(MUNIT_PLUS_LOG_DEBUG,
            "Encode (insert: %lu, copy: %lu, zero: %s) properly rejected.",
            ins_len, cpy_len, zero_dist?"true":"false");
        }
      } else {
        munit_plus_errorf(
          "Table row %" MUNIT_PLUS_SIZE_MODIFIER "u selected for "
          "%s configuration (insert: %lu, copy: %lu, zero: %s).",
          encode_index, (expect_success?"valid":"impossible"),
          ins_len, cpy_len, zero_dist?"true":"false");
      }
    }break;
  case 286: /* DEFLATE */{
      unsigned long int const cpy_len = testfont_rand_uint_range(0u,260u);
      if (cpy_len >= 259) {
        munit_plus_log(MUNIT_PLUS_LOG_DEBUG, "259 activated!");
      }
      bool const expect_success = (cpy_len >= 3 && cpy_len <= 258);
      std::size_t const encode_index = /* FIXME cpy_len should be second parameter */
        text_complex::access::inscopy_encode(*p, cpy_len, 0u);
      if ((encode_index != not_found) == expect_success) {
        if (expect_success) {
          text_complex::access::insert_copy_row const& row =
            (*p)[encode_index];
          unsigned long int const cpy_extra = /* FIXME should use copy_first */
            cpy_len - row.insert_first;
          munit_plus_logf(MUNIT_PLUS_LOG_DEBUG,
            "Encode (copy: %lu) as <%u, %lu:%u>",
            cpy_len, row.code, cpy_extra,
            row.insert_bits /* FIXME should use copy_bits */);
        } else {
          munit_plus_logf(MUNIT_PLUS_LOG_DEBUG,
            "Encode (copy: %lu) properly rejected.", cpy_len);
        }
      } else {
        munit_plus_errorf("Table row %" MUNIT_PLUS_SIZE_MODIFIER
          "u selected for "
          "%s configuration (copy: %lu).",
          encode_index, (expect_success?"valid":"impossible"), cpy_len);
      }
    }break;
  case 26: /* Brotli block counts */{
      unsigned long int const block_len = test_inscopy_rand_length25();
      bool const expect_success = (block_len >= 1 && block_len <= 16793840ul);
      std::size_t const encode_index =
        text_complex::access::inscopy_encode(*p, block_len, 0u);
      if ((encode_index != not_found) == expect_success) {
        if (expect_success) {
          text_complex::access::insert_copy_row const& row = (*p)[encode_index];
          unsigned long int const block_extra = block_len - row.insert_first;
          munit_plus_logf(MUNIT_PLUS_LOG_DEBUG,
            "Encode (block-count: %lu) as <%u, %lu:%u>",
            block_len, row.code, block_extra, row.insert_bits);
        } else {
          munit_plus_logf(MUNIT_PLUS_LOG_DEBUG,
            "Encode (block-count: %lu) properly rejected.", block_len);
        }
      } else {
        munit_plus_errorf("Table row %" MUNIT_PLUS_SIZE_MODIFIER
          "u selected for "
          "%s configuration (block-count: %lu).",
          encode_index, (expect_success?"valid":"impossible"), block_len);
      }
    }break;
  }
  return MUNIT_PLUS_OK;
}




int main(int argc, char **argv) {
  return munit_plus_suite_main(&suite_inscopy, nullptr, argc, argv);
}
