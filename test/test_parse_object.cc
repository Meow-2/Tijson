#include "test_utils.h"
#include "tijson.h"

TEST(PARSE_OBJECT, OBJECT)
{
    auto v = tijson::parse(" { } ");
    auto v_deep_copy(v);
    EXPECT_EQ(v_deep_copy.get_object().size(), 0);
    v = tijson::parse(" { "
                      "\"n\" : null , "
                      "\"f\" : false , "
                      "\"t\" : true , "
                      "\"i\" : 123 , "
                      "\"s\" : \"abc\", "
                      "\"a\" : [ 1, 2, 3 ],"
                      "\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"
                      " } "

    );
    auto v_deep_move(std::move(v));
    auto object0 = v_deep_move.get_object();
    EXPECT_VALUE_EQ_NULL(object0["n"]);
    EXPECT_VALUE_EQ_FALSE(object0["f"]);
    EXPECT_VALUE_EQ_TRUE(object0["t"]);
    EXPECT_VALUE_EQ_NUMBER(object0["i"], 123);
    EXPECT_VALUE_EQ_STRING(object0["s"], "abc");
    auto array = object0["a"].get_array();
    for (int i = 0; i < array.size(); i++)
        EXPECT_VALUE_EQ_NUMBER(array[i], i + 1);
    auto object1 = object0["o"].get_object();
    for (int i = 0; i < object1.size(); i++)
        EXPECT_VALUE_EQ_NUMBER(object1[std::to_string(i + 1)], i + 1);
}

TEST(PARSE_OBJECT, MISS_KEY)
{
    EXPECT_PARSE_THROW_MESSAGE("{:1,", "MISS_KEY");
    EXPECT_PARSE_THROW_MESSAGE("{1:1,", "MISS_KEY");
    EXPECT_PARSE_THROW_MESSAGE("{true:1,", "MISS_KEY");
    EXPECT_PARSE_THROW_MESSAGE("{false:1,", "MISS_KEY");
    EXPECT_PARSE_THROW_MESSAGE("{null:1,", "MISS_KEY");
    EXPECT_PARSE_THROW_MESSAGE("{[]:1,", "MISS_KEY");
    EXPECT_PARSE_THROW_MESSAGE("{{}:1,", "MISS_KEY");
    EXPECT_PARSE_THROW_MESSAGE("{\"a\":1,", "MISS_KEY");
}
TEST(PARSE_OBJECT, MISS_COLON)
{
    EXPECT_PARSE_THROW_MESSAGE("{\"a\"}", "MISS_COLON");
    EXPECT_PARSE_THROW_MESSAGE("{\"a\",\"b\"}", "MISS_COLON");
}

TEST(PARSE_OBJECT, MISS_COMMA_OR_CURLY_BRACKET)
{
    EXPECT_PARSE_THROW_MESSAGE("{\"a\":1", "MISS_COMMA_OR_CURLY_BRACKET");
    EXPECT_PARSE_THROW_MESSAGE("{\"a\":1]", "MISS_COMMA_OR_CURLY_BRACKET");
    EXPECT_PARSE_THROW_MESSAGE("{\"a\":1 \"b\"", "MISS_COMMA_OR_CURLY_BRACKET");
    EXPECT_PARSE_THROW_MESSAGE("{\"a\":{}", "MISS_COMMA_OR_CURLY_BRACKET");
}
