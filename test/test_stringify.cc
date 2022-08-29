#include "test_utils.h"

TEST(STRINGIFY, NUL)
{
    EXPECT_STRINGIFY_EQ("null");
    EXPECT_STRINGIFY_EQ(" null ");
    EXPECT_STRINGIFY_EQ("null ");
    EXPECT_STRINGIFY_EQ(" null");
    EXPECT_STRINGIFY_EQ("\rnull\n\t");
}
TEST(STRINGIFY, TRUE)
{
    EXPECT_STRINGIFY_EQ("true");
    EXPECT_STRINGIFY_EQ(" true ");
    EXPECT_STRINGIFY_EQ("true ");
    EXPECT_STRINGIFY_EQ(" true");
    EXPECT_STRINGIFY_EQ("\rtrue\n\t");
}
TEST(STRINGIFY, FALSE)
{
    EXPECT_STRINGIFY_EQ("false");
    EXPECT_STRINGIFY_EQ(" false ");
    EXPECT_STRINGIFY_EQ("false ");
    EXPECT_STRINGIFY_EQ(" false");
    EXPECT_STRINGIFY_EQ("\rfalse\n\t");
}
TEST(STRINGIFY, NUMBER)
{
    EXPECT_STRINGIFY_EQ("0");
    EXPECT_STRINGIFY_EQ("-0");
    EXPECT_STRINGIFY_EQ("-0.0");
    EXPECT_STRINGIFY_EQ("1");
    EXPECT_STRINGIFY_EQ("-1");
    EXPECT_STRINGIFY_EQ("1.5");
    EXPECT_STRINGIFY_EQ("-1.5");
    EXPECT_STRINGIFY_EQ("3.1416");
    EXPECT_STRINGIFY_EQ("1E10");
    EXPECT_STRINGIFY_EQ("1e10");
    EXPECT_STRINGIFY_EQ("1E+10");
    EXPECT_STRINGIFY_EQ("1E-10");
    EXPECT_STRINGIFY_EQ("-1E10");
    EXPECT_STRINGIFY_EQ("-1e10");
    EXPECT_STRINGIFY_EQ("-1E+10");
    EXPECT_STRINGIFY_EQ("-1E-10");
    EXPECT_STRINGIFY_EQ("1.234E+10");
    EXPECT_STRINGIFY_EQ("1.234E-10");
    EXPECT_STRINGIFY_EQ("1.0000000000000002");
    EXPECT_STRINGIFY_EQ("4.9406564584124654E-324");
    EXPECT_STRINGIFY_EQ("2.2250738585072009e-308");
    EXPECT_STRINGIFY_EQ("2.2250738585072014E-308");
    EXPECT_STRINGIFY_EQ("1.7976931348623157e+308");
    EXPECT_STRINGIFY_EQ("1e-10000"); /* must underflow */
}
TEST(STRINGIFY, STRING)
{
    EXPECT_STRINGIFY_EQ("\"\"");
    EXPECT_STRINGIFY_EQ("\"Hello\"");
    EXPECT_STRINGIFY_EQ("\"Hello\\nWorld\"");
    EXPECT_STRINGIFY_EQ("\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
    EXPECT_STRINGIFY_EQ("\"Hello\\u0000World\"");
    EXPECT_STRINGIFY_EQ("\"\\u0024\"");        /* Dollar sign U+0024 */
    EXPECT_STRINGIFY_EQ("\"\\u00A2\"");        /* Cents sign U+00A2 */
    EXPECT_STRINGIFY_EQ("\"\\u20AC\"");        /* Euro sign U+20AC */
    EXPECT_STRINGIFY_EQ("\"\\uD834\\uDD1E\""); /* G clef sign U+1D11E */
    EXPECT_STRINGIFY_EQ("\"\\ud834\\udd1e\""); /* G clef sign U+1D11E */
}
TEST(STRINGIFY, ARRAY)
{
    EXPECT_STRINGIFY_EQ("[ ]");
    EXPECT_STRINGIFY_EQ("[ null , false , true , 124 , \"abc\" ]");
    EXPECT_STRINGIFY_EQ("[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]");
}
TEST(STRINGIFY, OBJECT)
{
    EXPECT_STRINGIFY_EQ(" { } ");
    EXPECT_STRINGIFY_EQ(" { "
                        "\"n\" : null , "
                        "\"f\" : false , "
                        "\"t\" : true , "
                        "\"i\" : 123 , "
                        "\"s\" : \"abc\", "
                        "\"a\" : [ 1, 2, 3 ],"
                        "\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"
                        " } ");
}
