#include "test_utils.h"

TEST(PARSE_ARRAY, ARRAY0)
{
    auto v           = tijson::parse("[ ]");
    auto v_deep_copy = v;
    EXPECT_EQ(v_deep_copy.get_array().size(), 0);
    v           = tijson::parse("[ null , false , true , 124 , \"abc\" ]");
    v_deep_copy = v;
    EXPECT_VALUE_EQ_NULL(v_deep_copy.get_array()[0]);
    EXPECT_VALUE_EQ_FALSE(v_deep_copy.get_array()[1]);
    EXPECT_VALUE_EQ_TRUE(v_deep_copy.get_array()[2]);
    EXPECT_VALUE_EQ_NUMBER(v_deep_copy.get_array()[3], 124);
    EXPECT_VALUE_EQ_STRING(v_deep_copy.get_array()[4], "abc");
    v           = tijson::parse("[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]");
    v_deep_copy = v;
    for (int i = 0; i < v_deep_copy.get_array().size(); i++)
        for (int j = 0; j < v_deep_copy.get_array()[i].get_array().size(); j++)
            EXPECT_VALUE_EQ_NUMBER(v_deep_copy.get_array()[i].get_array()[j], j);
}

TEST(PARSE_ARRAY, MISS_COMMA_OR_SQUARE_BRACKET)
{
    EXPECT_PARSE_THROW_MESSAGE("[1", "MISS_COMMA_OR_SQUARE_BRACKET");
    EXPECT_PARSE_THROW_MESSAGE("[1}", "MISS_COMMA_OR_SQUARE_BRACKET");
    EXPECT_PARSE_THROW_MESSAGE("[1 2", "MISS_COMMA_OR_SQUARE_BRACKET");
    EXPECT_PARSE_THROW_MESSAGE("[[]", "MISS_COMMA_OR_SQUARE_BRACKET");
}
