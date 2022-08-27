#include "test_utils.h"

TEST(PARSE_ARRAY, ARRAY)
{
    // auto v = tijson::parse("[ ]");
    // v      = tijson::parse("[ null , false , true , 123 , \"abc\" ]");
    // v      = tijson::parse("[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]");
}

TEST(PARSE_ARRAY, MISS_COMMA_OR_SQUARE_BRACKET)
{
    EXPECT_PARSE_THROW_MESSAGE("[1", "MISS_COMMA_OR_SQUARE_BRACKET");
    EXPECT_PARSE_THROW_MESSAGE("[1}", "MISS_COMMA_OR_SQUARE_BRACKET");
    EXPECT_PARSE_THROW_MESSAGE("[1 2", "MISS_COMMA_OR_SQUARE_BRACKET");
    EXPECT_PARSE_THROW_MESSAGE("[[]", "MISS_COMMA_OR_SQUARE_BRACKET");
}
