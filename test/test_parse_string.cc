#include "enum_help.h"
#include "macro_help.h"

#include <gtest/gtest.h>
#include <stdexcept>
#include <tijson.h>

//     TEST_STRING("", "\"\"");
//     TEST_STRING("Hello", "\"Hello\"");
// #if 1
//     TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
//     TEST_STRING("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
// TEST_ERROR(LEPT_PARSE_MISS_QUOTATION_MARK, "\"");
// TEST_ERROR(LEPT_PARSE_MISS_QUOTATION_MARK, "\"abc");
//
//
// TEST_ERROR(LEPT_PARSE_INVALID_STRING_ESCAPE, "\"\\v\"");
// TEST_ERROR(LEPT_PARSE_INVALID_STRING_ESCAPE, "\"\\'\"");
// TEST_ERROR(LEPT_PARSE_INVALID_STRING_ESCAPE, "\"\\0\"");
// TEST_ERROR(LEPT_PARSE_INVALID_STRING_ESCAPE, "\"\\x12\"");
//
// TEST_ERROR(LEPT_PARSE_INVALID_STRING_CHAR, "\"\x01\"");
// TEST_ERROR(LEPT_PARSE_INVALID_STRING_CHAR, "\"\x1F\"");
