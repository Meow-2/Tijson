#include <gtest/gtest.h>
#include <magic_enum.hpp>
#include <stdexcept>
#include <tijson.h>

// EXPECT_STREQ is used to compare char const * p
#define EXPECT_EQ_VALUE_TYPE(TEST_VALUE, TEST_VALUE_TYPE)                             \
    do {                                                                              \
        EXPECT_EQ(magic_enum::enum_name(TEST_VALUE.GetType()),                        \
                  magic_enum::enum_name(tijson::Value::VALUE_TYPE::TEST_VALUE_TYPE)); \
    } while (0)

#define EXPECT_PARSE_THROW_MESSAGE(TEST_CONTENT, TEST_MESSAGE) \
    do {                                                       \
        std::invalid_argument temp("");                        \
        try {                                                  \
            tijson::Parser::Parse(TEST_CONTENT);               \
        }                                                      \
        catch (std::invalid_argument & e) {                    \
            temp = e;                                          \
        }                                                      \
        EXPECT_STREQ(temp.what(), TEST_MESSAGE);               \
    } while (0)

// what does EXPECT_EQ_XXX do ?
// 1. test Parse -> should be right value
// 2. test access -> should get right type and right value

#define EXPECT_EQ_TRUE(TEST_CONTENT)                  \
    do {                                              \
        auto v = tijson::Parser::Parse(TEST_CONTENT); \
        EXPECT_EQ(v.GetBool(), true);                 \
        EXPECT_EQ_VALUE_TYPE(v, TRUE);                \
    } while (0)

#define EXPECT_EQ_FALSE(TEST_CONTENT)                 \
    do {                                              \
        auto v = tijson::Parser::Parse(TEST_CONTENT); \
        EXPECT_EQ(v.GetBool(), false);                \
        EXPECT_EQ_VALUE_TYPE(v, FALSE);               \
    } while (0)

#define EXPECT_EQ_NULL(TEST_CONTENT)                  \
    do {                                              \
        auto v = tijson::Parser::Parse(TEST_CONTENT); \
        EXPECT_EQ_VALUE_TYPE(v, NUL);                 \
    } while (0)

#define EXPECT_EQ_NUMBER(TEST_CONTENT, TEST_NUMBER)   \
    do {                                              \
        auto v = tijson::Parser::Parse(TEST_CONTENT); \
        EXPECT_EQ(v.GetNumber(), TEST_NUMBER);        \
        EXPECT_EQ_VALUE_TYPE(v, NUMBER);              \
    } while (0)

#define EXPECT_EQ_STRING(TEST_CONTENT, TEST_STRING)   \
    do {                                              \
        auto v = tijson::Parser::Parse(TEST_CONTENT); \
        EXPECT_EQ(v.GetString(), TEST_STRING);        \
        EXPECT_EQ_VALUE_TYPE(v, STRING);              \
    } while (0)


#define EXPECT_VALUE_EQ_TRUE(v)        \
    do {                               \
        EXPECT_EQ(v.GetBool(), true);  \
        EXPECT_EQ_VALUE_TYPE(v, TRUE); \
    } while (0)

#define EXPECT_VALUE_EQ_FALSE(v)        \
    do {                                \
        EXPECT_EQ(v.GetBool(), false);  \
        EXPECT_EQ_VALUE_TYPE(v, FALSE); \
    } while (0)

#define EXPECT_VALUE_EQ_NULL(v)       \
    do {                              \
        EXPECT_EQ_VALUE_TYPE(v, NUL); \
    } while (0)

#define EXPECT_VALUE_EQ_NUMBER(v, TEST_NUMBER) \
    do {                                       \
        EXPECT_EQ(v.GetNumber(), TEST_NUMBER); \
        EXPECT_EQ_VALUE_TYPE(v, NUMBER);       \
    } while (0)

#define EXPECT_VALUE_EQ_STRING(v, TEST_STRING) \
    do {                                       \
        EXPECT_EQ(v.GetString(), TEST_STRING); \
        EXPECT_EQ_VALUE_TYPE(v, STRING);       \
    } while (0)

#define EXPECT_STRINGIFY_EQ(TEST_CONTENT)                         \
    do {                                                          \
        auto v           = tijson::Parser::Parse(TEST_CONTENT);   \
        auto v_deep_copy = v;                                     \
        auto str         = v_deep_copy.Stringify();               \
        v                = std::move(tijson::Parser::Parse(str)); \
        auto v2          = tijson::Parser::Parse(str);            \
        EXPECT_EQ(v2, v);                                         \
    } while (0)\
