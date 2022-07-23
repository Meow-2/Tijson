#include "enum_help.h"
#include "macro_help.h"

#include <gtest/gtest.h>
#include <stdexcept>
#include <tijson.h>


// TEST(PARSE_STRING, STRING)
// {
//     EXPECT_EQ_STRING("\"\"", "");
//     EXPECT_EQ_STRING("\"Hello\"", "Hello");
//     EXPECT_EQ_STRING("\"Hello\\nWorld\"", "Hello\nWorld");
//     EXPECT_EQ_STRING("\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"", "\" \\ / \b \f \n \r \t");
// }
//
// TEST(PARSE_STRING, MISS_QUOTATION_MARK)
// {
//     EXPECT_PARSE_THROW_MESSAGE("\"", "MISS_QUOTATION_MARK");
//     EXPECT_PARSE_THROW_MESSAGE("\"abc", "MISS_QUOTATION_MARK");
// }
// TEST(PARSE_STRING, INVALID_STRING_ESCAPE)
// {
//     EXPECT_PARSE_THROW_MESSAGE("\"\\v\"", "INVALID_STRING_ESCAPE");
//     EXPECT_PARSE_THROW_MESSAGE("\"\\'\"", "INVALID_STRING_ESCAPE");
//     EXPECT_PARSE_THROW_MESSAGE("\"\\0\"", "INVALID_STRING_ESCAPE");
//     EXPECT_PARSE_THROW_MESSAGE("\"\\x12\"", "INVALID_STRING_ESCAPE");
// }
// TEST(PARSE_STRING, INVALID_STRING_CHAR)
// {
//
//     EXPECT_PARSE_THROW_MESSAGE("\"\x01\"", "INVALID_STRING_CHAR");
//     EXPECT_PARSE_THROW_MESSAGE("\"\x1F\"", "INVALID_STRING_CHAR");
// }
