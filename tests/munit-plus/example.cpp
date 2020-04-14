/* Example file for using µnit.
 *
 * µnit is MIT-licensed, but for this file and this file alone:
 *
 * To the extent possible under law, the author(s) of this file have
 * waived all copyright and related or neighboring rights to this
 * work.  See <https://creativecommons.org/publicdomain/zero/1.0/> for
 * details.
 *********************************************************************/

#include "munit.hpp"
#include <string>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <list>

/* This is just to disable an MSVC warning about conditional
 * expressions being constant, which you shouldn't have to do for your
 * code.  It's only here because we want to be able to do silly things
 * like assert that 0 != 1 for our demo. */
#if defined(_MSC_VER)
#pragma warning(disable: 4127)
#endif

/* Tests are functions that return void, and take a single void*
 * parameter.  We'll get to what that parameter is later. */
static MunitPlusResult
test_compare(const MunitPlusParameter params[], void* data) {
  /* We'll use these later */
  const unsigned char val_uchar = 'b';
  const short val_short = 1729;
  double pi = 3.141592654;
  char const* stewardesses = "stewardesses";
  char const* most_fun_word_to_type;

  /* These are just to silence compiler warnings about the parameters
   * being unused. */
  (void) params;
  (void) data;

  /* Let's start with the basics. */
  munit_plus_assert(0 != 1);

  /* There is also the more verbose, though slightly more descriptive
     munit_plus_assert_true/false: */
  munit_plus_assert_false(0);

  /* You can also call munit_plus_error and munit_plus_errorf yourself.  We
   * won't do it is used to indicate a failure, but here is what it
   * would look like: */
  /* munit_plus_error("FAIL"); */
  /* munit_plus_errorf("Goodbye, cruel %s", "world"); */

  /* There are macros for comparing lots of types. */
  munit_plus_assert_char('a', ==, 'a');

  /* Sure, you could just assert('a' == 'a'), but if you did that, a
   * failed assertion would just say something like "assertion failed:
   * val_uchar == 'b'".  µnit will tell you the actual values, so a
   * failure here would result in something like "assertion failed:
   * val_uchar == 'b' ('X' == 'b')." */
  munit_plus_assert_uchar(val_uchar, ==, 'b');

  /* Obviously we can handle values larger than 'char' and 'uchar'.
   * There are versions for char, short, int, long, long long,
   * int8/16/32/64_t, as well as the unsigned versions of them all. */
  munit_plus_assert_short(42, <, val_short);

  /* There is also support for size_t.
   *
   * The longest word in English without repeating any letters is
   * "uncopyrightables", which has uncopyrightable (and
   * dermatoglyphics, which is the study of fingerprints) beat by a
   * character */
  munit_plus_assert_size(std::strlen("uncopyrightables"), >, std::strlen("dermatoglyphics"));

  /* Of course there is also support for doubles and floats. */
  munit_plus_assert_double(pi, ==, 3.141592654);

  /* If you want to compare two doubles for equality, you might want
   * to consider using munit_plus_assert_double_equal.  It compares two
   * doubles for equality within a precison of 1.0 x 10^-(precision).
   * Note that precision (the third argument to the macro) needs to be
   * fully evaluated to an integer by the preprocessor so µnit doesn't
   * have to depend pow, which is often in libm not libc. */
  munit_plus_assert_double_equal(3.141592654, 3.141592653589793, 9);

  /* And if you want to check strings for equality (or inequality),
   * there is munit_plus_assert_string_equal/not_equal.
   *
   * "stewardesses" is the longest word you can type on a QWERTY
   * keyboard with only one hand, which makes it loads of fun to type.
   * If I'm going to have to type a string repeatedly, let's make it a
   * good one! */
  munit_plus_assert_string_equal(stewardesses, "stewardesses");

  /* A personal favorite macro which is fantastic if you're working
   * with binary data, is the one which naïvely checks two blobs of
   * memory for equality.  If this fails it will tell you the offset
   * of the first differing byte. */
  munit_plus_assert_memory_equal(7, stewardesses, "steward");

  /* You can also make sure that two blobs differ *somewhere*: */
  munit_plus_assert_memory_not_equal(8, stewardesses, "steward");

  /* There are equal/not_equal macros for pointers, too: */
  most_fun_word_to_type = stewardesses;
  munit_plus_assert_ptr_equal(most_fun_word_to_type, stewardesses);

  /* And null/not_null */
  munit_plus_assert_null(nullptr);
  munit_plus_assert_not_null(most_fun_word_to_type);

  /* Lets verify that the data parameter is what we expected.  We'll
   * see where this comes from in a bit.
   *
   * Note that the casting isn't usually required; if you give this
   * function a real pointer (instead of a number like 0xdeadbeef) it
   * would work as expected. */
  munit_plus_assert_ptr_equal(data, (void*)(uintptr_t)0xdeadbeef);

  return MUNIT_PLUS_OK;
}

static MunitPlusResult
test_rand(const MunitPlusParameter params[], void* user_data) {
  int random_int;
  double random_dbl;
  munit_plus_uint8_t data[5];

  (void) params;
  (void) user_data;

  /* One thing missing from a lot of unit testing frameworks is a
   * random number generator.  You can't just use srand/rand because
   * the implementation varies across different platforms, and it's
   * important to be able to look at the seed used in a failing test
   * to see if you can reproduce it.  Some randomness is a fantastic
   * thing to have in your tests, I don't know why more people don't
   * do it...
   *
   * µnit's PRNG is re-seeded with the same value for each iteration
   * of each test.  The seed is retrieved from the MUNIT_SEED
   * envirnment variable or, if none is provided, one will be
   * (pseudo-)randomly generated. */

  /* If you need an integer in a given range */
  random_int = munit_plus_rand_int_range(128, 4096);
  munit_plus_assert_int(random_int, >=, 128);
  munit_plus_assert_int(random_int, <=, 4096);

  /* Or maybe you want a double, between 0 and 1: */
  random_dbl = munit_plus_rand_double();
  munit_plus_assert_double(random_dbl, >=, 0.0);
  munit_plus_assert_double(random_dbl, <=, 1.0);

  /* Of course, you want to be able to reproduce bugs discovered
   * during testing, so every time the tests are run they print the
   * random seed used.  When you want to reproduce a result, just put
   * that random seed in the MUNIT_SEED environment variable; it even
   * works on different platforms.
   *
   * If you want this to pass, use 0xdeadbeef as the random seed and
   * uncomment the next line of code.  Note that the PRNG is not
   * re-seeded between iterations of the same test, so this will only
   * work on the first iteration. */
  /* munit_plus_assert_uint32(munit_plus_rand_uint32(), ==, 1306447409); */

  /* You can also get blobs of random memory: */
  munit_plus_rand_memory(sizeof(data), data);

  return MUNIT_PLUS_OK;
}

/* This test case shows how to accept parameters.  We'll see how to
 * specify them soon.
 *
 * By default, every possible variation of a parameterized test is
 * run, but you can specify parameters manually if you want to only
 * run specific test(s), or you can pass the --single argument to the
 * CLI to have the harness simply choose one variation at random
 * instead of running them all. */
static MunitPlusResult
test_parameters(const MunitPlusParameter params[], void* user_data) {
  const char* foo;
  const char* bar;

  (void) user_data;

  /* The "foo" parameter is specified as one of the following values:
   * "one", "two", or "three". */
  foo = munit_plus_parameters_get(params, "foo");
  /* Similarly, "bar" is one of "four", "five", or "six". */
  bar = munit_plus_parameters_get(params, "bar");
  /* "baz" is a bit more complicated.  We don't actually specify a
   * list of valid values, so by default nullptr is passed.  However, the
   * CLI will accept any value.  This is a good way to have a value
   * that is usually selected randomly by the test, but can be
   * overridden on the command line if desired. */
  /* const char* baz = munit_plus_parameters_get(params, "baz"); */

  /* Notice that we're returning MUNIT_PLUS_FAIL instead of writing an
   * error message.  Error messages are generally preferable, since
   * they make it easier to diagnose the issue, but this is an
   * option.
   *
   * Possible values are:
   *  - MUNIT_PLUS_OK: Sucess
   *  - MUNIT_PLUS_FAIL: Failure
   *  - MUNIT_PLUS_SKIP: The test was skipped; usually this happens when a
   *    particular feature isn't in use.  For example, if you're
   *    writing a test which uses a Wayland-only feature, but your
   *    application is running on X11.
   *  - MUNIT_PLUS_ERROR: The test failed, but not because of anything you
   *    wanted to test.  For example, maybe your test downloads a
   *    remote resource and tries to parse it, but the network was
   *    down.
   */

  if (strcmp(foo, "one") != 0 &&
      strcmp(foo, "two") != 0 &&
      strcmp(foo, "three") != 0)
    return MUNIT_PLUS_FAIL;

  if (strcmp(bar, "red") != 0 &&
      strcmp(bar, "green") != 0 &&
      strcmp(bar, "blue") != 0)
    return MUNIT_PLUS_FAIL;

  return MUNIT_PLUS_OK;
}

/* The setup function, if you provide one, for a test will be run
 * before the test, and the return value will be passed as the sole
 * parameter to the test function. */
static void*
test_compare_setup(const MunitPlusParameter params[], void* user_data) {
  (void) params;

  munit_plus_assert_string_equal(static_cast<char*>(user_data), "µnit");
  return reinterpret_cast<void*>( static_cast<uintptr_t>(0xdeadbeef) );
}

/* To clean up after a test, you can use a tear down function.  The
 * fixture argument is the value returned by the setup function
 * above. */
static void
test_compare_tear_down(void* fixture) {
  munit_plus_assert_ptr_equal(fixture, (void*)(uintptr_t)0xdeadbeef);
}

#include <string>
#include <iostream>

class thing_with_a_dtor {
public:
  ~thing_with_a_dtor(void) {
    munit_plus_log(MUNIT_PLUS_LOG_INFO, "yay! destructor was called.");
  }
};

/* Let's see what C++ can do. */
static MunitPlusResult
test_compare_cxx(const MunitPlusParameter params[], void* data) {
  /* We'll use these later */
  const unsigned char val_uchar = 'b';
  const short val_short = 1729;
  double pi = 3.141592654;
  std::string stewardesses = "stewardesses";
  std::string most_fun_word_to_type;
  thing_with_a_dtor thingie;

  /* These are just to silence compiler warnings about the parameters
   * being unused. */
  (void) params;
  (void) data;

  /* Let's start with the basics. */
  munit_plus_assert(0 != 1);

  /* There is also the more verbose, though slightly more descriptive
     munit_plus_assert_true/false: */
  munit_plus_assert_false(false);

  /* You can also call munit_plus_error and munit_plus_errorf yourself.  We
   * won't do it is used to indicate a failure, but here is what it
   * would look like: */
  /* munit_plus_error("FAIL"); */
  /* munit_plus_errorf("Goodbye, cruel %s", "world"); */

  /* There are macros for comparing lots of types. */
  munit_plus_assert_op('a', ==, 'a');

  /* Sure, you could just assert('a' == 'a'), but if you did that, a
   * failed assertion would just say something like "assertion failed:
   * val_uchar == 'b'".  µnit will tell you the actual values, so a
   * failure here would result in something like "assertion failed:
   * val_uchar == 'b' ('X' == 'b')." */
  munit_plus_assert_op(val_uchar, ==, 'b');

  /* Obviously we can handle values larger than 'char' and 'uchar'.
   * There are versions for char, short, int, long, long long,
   * int8/16/32/64_t, as well as the unsigned versions of them all. */
  munit_plus_assert_op(42, <, val_short);

  /* There is also support for size_t.
   *
   * The longest word in English without repeating any letters is
   * "uncopyrightables", which has uncopyrightable (and
   * dermatoglyphics, which is the study of fingerprints) beat by a
   * character */
  munit_plus_assert_op(strlen("uncopyrightables"), >, strlen("dermatoglyphics"));

  /* Of course there is also support for doubles and floats. */
  munit_plus_assert_op(pi, ==, 3.141592654);

  /* If you want to compare two doubles for equality, you might want
   * to consider using munit_plus_assert_near_equal.  It compares two
   * doubles for equality within a precison of 1.0 x 10^-(precision).
   * Note that precision (the third argument to the macro) needs to be
   * fully evaluated to an integer by the preprocessor so µnit doesn't
   * have to depend pow, which is often in libm not libc. */
  munit_plus_assert_near_equal(double, 3.141592654, 3.141592653589793, 9);

  /* And if you want to check strings for equality (or inequality),
   * the munit_plus_assert_op should just work. For the char* variables,
   * munit_plus_assert_string_equal remains available.
   *
   * "stewardesses" is the longest word you can type on a QWERTY
   * keyboard with only one hand, which makes it loads of fun to type.
   * If I'm going to have to type a string repeatedly, let's make it a
   * good one! */
  munit_plus_assert_op(stewardesses, ==, "stewardesses");
  munit_plus_assert_string_equal(stewardesses.c_str(), "stewardesses");

  /* A personal favorite macro which is fantastic if you're working
   * with binary data, is the one which naïvely checks two blobs of
   * memory for equality.  If this fails it will tell you the offset
   * of the first differing byte. */
  munit_plus_assert_memory_equal(7, stewardesses.c_str(), "steward");

  /* You can also make sure that two blobs differ *somewhere*: */
  munit_plus_assert_memory_not_equal(8, stewardesses.c_str(), "steward");

  /* There are equal/not_equal macros for pointers, too: */
  most_fun_word_to_type = stewardesses;
  munit_plus_assert_ptr_not_equal(&most_fun_word_to_type, &stewardesses);

  /* And null/not_null */
  munit_plus_assert_null(nullptr);
  munit_plus_assert_not_null(most_fun_word_to_type.c_str());

  /* Lets verify that the data parameter is what we expected.  We'll
   * see where this comes from in a bit.
   *
   * Note that the casting isn't usually required; if you give this
   * function a real pointer (instead of a number like 0xdeadbeef) it
   * would work as expected. */
  munit_plus_assert_ptr_equal(data, reinterpret_cast<void*>(static_cast<uintptr_t>(0xdeadbeef)));

  return MUNIT_PLUS_OK;
}

/* One-off tests. */
class trivial_thing {
public:
  int yay;
  float hah;
  void* nowhere;
};
class not_so_trivial {
private:
  int x;
public:
  not_so_trivial() : x(50) { }
  not_so_trivial(int i) : x(i) { }
  int get_x(void) { return x; }
};

static MunitPlusResult
test_compare_cxx_oneoff(const MunitPlusParameter params[], void* data) {
  /* These are just to silence compiler warnings about the parameters
   * being unused. */
  (void) params;
  (void) data;
  /* Use munit_plus_new to create instances of a class through
   * operator new. Use munit_plus_newp to add constructor parameters. */
  int one_value = munit_plus_rand_int_range(-127, 127);
  not_so_trivial *one = munit_plus_newp(not_so_trivial, one_value);
  munit_plus_assert_op(one->get_x(), ==, one_value);
  delete one;
  not_so_trivial *another = munit_plus_new(not_so_trivial);
  munit_plus_assert_op(one->get_x(), ==, 50);
  delete another;
  std::list<long> seven;
  /* assign */{
    int i;
    for (i = 0; i < 7; ++i) { seven.push_back(0); }
  }
  munit_plus_rand_memory_ex(seven.begin(), seven.end());
  std::vector<trivial_thing> five;
  five.resize(5);
  munit_plus_rand_memory_ex(five.begin(), five.end());
  std::vector<std::string> four;
  four.resize(4);
  //munit_plus_rand_memory_ex(four.begin(), four.end());
  //std::string three[3] = { "a", "b", "c" };
  //munit_plus_rand_memory_ex(three);
  int six[6];
  munit_plus_rand_memory_ex(six);
  //thing_with_a_dtor two[2];
  //munit_plus_rand_memory_ex(two);
  /* read */{
    for (std::list<long>::iterator it7 = seven.begin();
      it7 != seven.end(); ++it7)
    {
      long x = *it7;
      munit_plus_logf(MUNIT_PLUS_LOG_INFO, "seven: %li", x);
    }
    for (std::vector<trivial_thing>::iterator it5 = five.begin();
      it5 != five.end(); ++it5)
    {
      trivial_thing const& x = *it5;
      munit_plus_logf(MUNIT_PLUS_LOG_INFO, "five: %i,%f,%p", x.yay,x.hah,x.nowhere);
    }
    for (int i = 0; i < 6; ++i) {
      int x = six[i];
      munit_plus_logf(MUNIT_PLUS_LOG_INFO, "six: %i", x);
    }
  }
  return MUNIT_PLUS_OK;
}

static char* foo_params[] = {
  (char*) "one", (char*) "two", (char*) "three", nullptr
};

static char* bar_params[] = {
  (char*) "red", (char*) "green", (char*) "blue", nullptr
};

static MunitPlusParameterEnum test_params[] = {
  { (char*) "foo", foo_params },
  { (char*) "bar", bar_params },
  { (char*) "baz", nullptr },
  { nullptr, nullptr },
};

/* Creating a test suite is pretty simple.  First, you'll need an
 * array of tests: */
static MunitPlusTest test_suite_tests[] = {
  {
    /* The name is just a unique human-readable way to identify the
     * test. You can use it to run a specific test if you want, but
     * usually it's mostly decorative. */
    (char*) "/example/compare",
    /* You probably won't be surprised to learn that the tests are
     * functions. */
    test_compare,
    /* If you want, you can supply a function to set up a fixture.  If
     * you supply nullptr, the user_data parameter from munit_plus_suite_main
     * will be used directly.  If, however, you provide a callback
     * here the user_data parameter will be passed to this callback,
     * and the return value from this callback will be passed to the
     * test function.
     *
     * For our example we don't really need a fixture, but lets
     * provide one anyways. */
    test_compare_setup,
    /* If you passed a callback for the fixture setup function, you
     * may want to pass a corresponding callback here to reverse the
     * operation. */
    test_compare_tear_down,
    /* Finally, there is a bitmask for options you can pass here.  You
     * can provide either MUNIT_PLUS_TEST_OPTION_NONE or 0 here to use the
     * defaults. */
    MUNIT_PLUS_TEST_OPTION_NONE,
    nullptr
  },
  /* Usually this is written in a much more compact format; all these
   * comments kind of ruin that, though.  Here is how you'll usually
   * see entries written: */
  { (char*) "/example/rand", test_rand, nullptr, nullptr, MUNIT_PLUS_TEST_OPTION_NONE, nullptr },
  { (char*) "/example/parameters", test_parameters, nullptr, nullptr, MUNIT_PLUS_TEST_OPTION_NONE, test_params },
  { (char*) "/example/cxx", test_compare_cxx, test_compare_setup, test_compare_tear_down, MUNIT_PLUS_TEST_OPTION_NONE },
  { (char*) "/example/cxx_oneoff", test_compare_cxx_oneoff, nullptr, nullptr, MUNIT_PLUS_TEST_OPTION_NONE },
  /* To tell the test runner when the array is over, just add a nullptr
   * entry at the end. */
  { nullptr, nullptr, nullptr, nullptr, MUNIT_PLUS_TEST_OPTION_NONE, nullptr }
};

/* If you wanted to have your test suite run other test suites you
 * could declare an array of them.  Of course each sub-suite can
 * contain more suites, etc. */
/* static const MunitPlusSuite other_suites[] = { */
/*   { "/second", test_suite_tests, nullptr, 1, MUNIT_PLUS_SUITE_OPTION_NONE }, */
/*   { nullptr, nullptr, nullptr, 0, MUNIT_PLUS_SUITE_OPTION_NONE } */
/* }; */

/* Now we'll actually declare the test suite.  You could do this in
 * the main function, or on the heap, or whatever you want. */
static const MunitPlusSuite test_suite = {
  /* This string will be prepended to all test names in this suite;
   * for example, "/example/rand" will become "/µnit/example/rand".
   * Note that, while it doesn't really matter for the top-level
   * suite, nullptr signal the end of an array of tests; you should use
   * an empty string ("") instead. */
  (char*) "",
  /* The first parameter is the array of test suites. */
  test_suite_tests,
  /* In addition to containing test cases, suites can contain other
   * test suites.  This isn't necessary in this example, but it can be
   * a great help to projects with lots of tests by making it easier
   * to spread the tests across many files.  This is where you would
   * put "other_suites" (which is commented out above). */
  nullptr,
  /* An interesting feature of µnit is that it supports automatically
   * running multiple iterations of the tests.  This is usually only
   * interesting if you make use of the PRNG to randomize your tests
   * cases a bit, or if you are doing performance testing and want to
   * average multiple runs.  0 is an alias for 1. */
  1,
  /* Just like MUNIT_PLUS_TEST_OPTION_NONE, you can provide
   * MUNIT_PLUS_SUITE_OPTION_NONE or 0 to use the default settings. */
  MUNIT_PLUS_SUITE_OPTION_NONE
};

/* This is only necessary for EXIT_SUCCESS and EXIT_FAILURE, which you
 * *should* be using but probably aren't (no, zero and non-zero don't
 * always mean success and failure).  I guess my point is that nothing
 * about µnit requires it. */
#include <cstdlib>

int main(int argc, char* argv[MUNIT_PLUS_ARRAY_PARAM(argc + 1)]) {
  /* Finally, we'll actually run our test suite!  That second argument
   * is the user_data parameter which will be passed either to the
   * test or (if provided) the fixture setup function. */
  return munit_plus_suite_main(&test_suite, (void*) "µnit", argc, argv);
}
