/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/string.h>

#include <fatal/test/driver.h>

#include <type_traits>

namespace fatal {

/////////////////
// type_string //
/////////////////

#define A_STR "a"
#define Z_STR "a\0b\0c"
#define EMPTY_STR ""
#define HELLO_STR "hello"
#define TEST_STR "this is a test"
#define UTF8_STR u8"UTF-8 String"
#define UTF16_STR u"UTF-16 String"
#define UTF32_STR U"UTF-32 String"

FATAL_STR(a_str, A_STR);
FATAL_STR(z_str, Z_STR);
FATAL_STR(empty_str, EMPTY_STR);
FATAL_STR(hello_str, HELLO_STR);
FATAL_STR(test_str, TEST_STR);
FATAL_STR(utf8_str, UTF8_STR);
FATAL_STR(utf16_str, UTF16_STR);
FATAL_STR(utf32_str, UTF32_STR);

// TODO: ADD TESTS FOR OTHER OPERATIONS INHERITED FROM CONSTANT_SEQUENCE

template <typename TString, std::size_t Expected>
void check_size() {
  FATAL_EXPECT_EQ(TString::size, Expected);
}

FATAL_TEST(type_string, size) {
  check_size<a_str, 1>();
  check_size<z_str, 5>();
  check_size<empty_str, 0>();
  check_size<hello_str, 5>();
  check_size<test_str, 14>();
  check_size<utf8_str, 12>();
  check_size<utf16_str, 13>();
  check_size<utf32_str, 13>();
}

#define CREATE_TEST_CALLS(Fn) \
  Fn<a_str>(A_STR); \
  Fn<z_str>(Z_STR); \
  Fn<empty_str>(EMPTY_STR); \
  Fn<hello_str>(HELLO_STR); \
  Fn<test_str>(TEST_STR); \
  Fn<utf8_str>(UTF8_STR); \
  Fn<utf16_str>(UTF16_STR); \
  Fn<utf32_str>(UTF32_STR)

FATAL_STR(u0, "0");
FATAL_STR(u1, "1");
FATAL_STR(u42, "42");
FATAL_STR(s_56, "-56");

FATAL_STR(wu0, L"0");
FATAL_STR(wu1, L"1");
FATAL_STR(wu42, L"42");
FATAL_STR(ws_56, L"-56");

// TODO: ADD EDGE CASES
#define CREATE_PARSE_TEST_CALLS(Fn) \
  Fn(u0, unsigned, 0); \
  Fn(u1, short, 1); \
  Fn(u42, std::size_t, 42); \
  Fn(s_56, int, -56); \
  \
  Fn(wu0, unsigned, 0); \
  Fn(wu1, short, 1); \
  Fn(wu42, std::size_t, 42); \
  Fn(ws_56, int, -56)

///////////////
// char_type //
///////////////

template <typename TCSTR, typename TChar, std::size_t Size>
void check_char_type(TChar const (&s)[Size]) {
  FATAL_EXPECT_SAME<TChar, typename TCSTR::char_type>();
}

FATAL_TEST(type_string, char_type) {
  CREATE_TEST_CALLS(check_char_type);
}

////////////
// string //
////////////

template <typename TCSTR, typename TChar, std::size_t Size>
void check_string(TChar const (&s)[Size]) {
  static_assert(Size > 0, "expected a valid null-terminated string");
  constexpr auto size = Size - 1;

  typedef std::basic_string<TChar> string_t;

  FATAL_EXPECT_SAME<string_t, typename TCSTR::template string_type<>>();

  auto const string = TCSTR::string();

  FATAL_EXPECT_EQ(size, string.size());
  FATAL_EXPECT_EQ(string_t(s, size), string);
}

FATAL_TEST(type_string, string) {
  CREATE_TEST_CALLS(check_string);
}

////////////////////
// to_type_string //
////////////////////

#define TEST_IMPL(Str, T, Value) \
  do { \
    FATAL_EXPECT_SAME< \
      Str, \
      to_type_string<T, Value, Str::char_type> \
    >(); \
  } while (false)

FATAL_TEST(to_type_string, sanity_check) {
  CREATE_PARSE_TEST_CALLS(TEST_IMPL);
}

#undef TEST_IMPL

///////////////////////
// parse_type_string //
///////////////////////

#define TEST_IMPL(Str, T, Value) \
  do { \
    using expected = std::integral_constant<T, Value>; \
    \
    FATAL_EXPECT_SAME< \
      expected, \
      Str::apply<parse_type_string<T>::bind<Str::char_type>::apply> \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      expected, \
      Str::typed_apply<parse_type_string<T>::apply> \
    >(); \
    \
    FATAL_EXPECT_SAME< \
      expected, \
      parse_type_string<T>::from<Str> \
    >(); \
  } while (false)

FATAL_TEST(parse_type_string, sanity_check) {
  CREATE_PARSE_TEST_CALLS(TEST_IMPL);
}

#undef TEST_IMPL

} // namespace fatal {
