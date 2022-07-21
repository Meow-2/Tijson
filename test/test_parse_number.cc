#include "enum_help.h"
#include "macro_help.h"

#include <gtest/gtest.h>
#include <stdexcept>
#include <tijson.h>

TEST(PARSE_NUMBER, NUMBER)
{
    EXPECT_EQ_NUMBER("0", 0.0);
    EXPECT_EQ_NUMBER("-0", 0.0);
    EXPECT_EQ_NUMBER("-0.0", 0.0);
    EXPECT_EQ_NUMBER("1", 1.0);
    EXPECT_EQ_NUMBER("-1", -1.0);
    EXPECT_EQ_NUMBER("1.5", 1.5);
    EXPECT_EQ_NUMBER("-1.5", -1.5);
    EXPECT_EQ_NUMBER("3.1416", 3.1416);
    EXPECT_EQ_NUMBER("1E10", 1E10);
    EXPECT_EQ_NUMBER("1e10", 1e10);
    EXPECT_EQ_NUMBER("1E+10", 1E+10);
    EXPECT_EQ_NUMBER("1E-10", 1E-10);
    EXPECT_EQ_NUMBER("-1E10", -1E10);
    EXPECT_EQ_NUMBER("-1e10", -1e10);
    EXPECT_EQ_NUMBER("-1E+10", -1E+10);
    EXPECT_EQ_NUMBER("-1E-10", -1E-10);
    EXPECT_EQ_NUMBER("1.234E+10", 1.234E+10);
    EXPECT_EQ_NUMBER("1.234E-10", 1.234E-10);
    EXPECT_EQ_NUMBER("1.0000000000000002", 1.0000000000000002);
    EXPECT_EQ_NUMBER("4.9406564584124654E-324", 4.9406564584124654E-324);
    EXPECT_EQ_NUMBER("2.2250738585072009e-308", 2.2250738585072009E-308);
    EXPECT_EQ_NUMBER("2.2250738585072014E-308", 2.2250738585072014E-308);
    EXPECT_EQ_NUMBER("1.7976931348623157e+308", 1.7976931348623157E+308);
    EXPECT_EQ_NUMBER("1e-10000", 0.0); /* must underflow */
}

TEST(PARSE_NUMBER, INVALID_VALUE)
{
    EXPECT_PARSE_THROW_MESSAGE("+0", "INVALID_VALUE");
    EXPECT_PARSE_THROW_MESSAGE("+1", "INVALID_VALUE");
    EXPECT_PARSE_THROW_MESSAGE(".123", "INVALID_VALUE"); /* at least one digit before '.' */
    EXPECT_PARSE_THROW_MESSAGE("1.", "INVALID_VALUE");   /* at least one digit after '.' */
    EXPECT_PARSE_THROW_MESSAGE("INF", "INVALID_VALUE");
    EXPECT_PARSE_THROW_MESSAGE("inf", "INVALID_VALUE");
    EXPECT_PARSE_THROW_MESSAGE("NAN", "INVALID_VALUE");
    EXPECT_PARSE_THROW_MESSAGE("nan", "INVALID_VALUE");
}

TEST(PARSE_NUMBER, NUMBER_TOO_BIG)
{
    EXPECT_PARSE_THROW_MESSAGE("1e309", "NUMBER_TOO_BIG");
    EXPECT_PARSE_THROW_MESSAGE("-1e309", "NUMBER_TOO_BIG");
}

TEST(PARSE_NUMBER, ROOT_NOT_SINGULAR)
{
    EXPECT_PARSE_THROW_MESSAGE(
        "0123", "ROOT_NOT_SINGULAR"); /* after zero should be '.' , 'E' , 'e' or nothing */
    EXPECT_PARSE_THROW_MESSAGE("0x0", "ROOT_NOT_SINGULAR");
    EXPECT_PARSE_THROW_MESSAGE("0x123", "ROOT_NOT_SINGULAR");
}
