#include "test_utils.h"

TEST(PARSE_ARRAY, ARRAY)
{
    auto v           = tijson::Parse("[ ]");
    auto v_deep_copy = v;
    EXPECT_EQ(v_deep_copy.GetArray().size(), 0);
    v           = tijson::Parse("[ null , false , true , 124 , \"abc\" ]");
    v_deep_copy = v;
    EXPECT_VALUE_EQ_NULL(v_deep_copy.GetArray()[0]);
    EXPECT_VALUE_EQ_FALSE(v_deep_copy.GetArray()[1]);
    EXPECT_VALUE_EQ_TRUE(v_deep_copy.GetArray()[2]);
    EXPECT_VALUE_EQ_NUMBER(v_deep_copy.GetArray()[3], 124);
    EXPECT_VALUE_EQ_STRING(v_deep_copy.GetArray()[4], "abc");
    v           = tijson::Parse("[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]");
    v_deep_copy = v;
    for (auto& i : v_deep_copy.GetArray())
        for (int j = 0; j < i.GetArray().size(); j++)
            EXPECT_VALUE_EQ_NUMBER(i.GetArray()[j], j);
}

TEST(PARSE_ARRAY, MISS_COMMA_OR_SQUARE_BRACKET)
{
    EXPECT_PARSE_THROW_MESSAGE("[1", "MISS_COMMA_OR_SQUARE_BRACKET");
    EXPECT_PARSE_THROW_MESSAGE("[1}", "MISS_COMMA_OR_SQUARE_BRACKET");
    EXPECT_PARSE_THROW_MESSAGE("[1 2", "MISS_COMMA_OR_SQUARE_BRACKET");
    EXPECT_PARSE_THROW_MESSAGE("[[]", "MISS_COMMA_OR_SQUARE_BRACKET");
}
