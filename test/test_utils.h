#include <gtest/gtest.h>
#include <magic_enum.hpp>
#include <stdexcept>
#include <tijson.h>

// EXPECT_STREQ is used to compare char const * p
#define EXPECT_EQ_VALUE_TYPE(TEST_VALUE, TEST_VALUE_TYPE)                             \
    do {                                                                              \
        EXPECT_EQ(magic_enum::enum_name(TEST_VALUE.get_type()),                       \
                  magic_enum::enum_name(tijson::value::VALUE_TYPE::TEST_VALUE_TYPE)); \
    } while (0)

#define EXPECT_PARSE_THROW_MESSAGE(TEST_CONTENT, TEST_MESSAGE) \
    do {                                                       \
        std::invalid_argument temp("");                        \
        try {                                                  \
            tijson::parse(TEST_CONTENT);                       \
        }                                                      \
        catch (std::invalid_argument & e) {                    \
            temp = e;                                          \
        }                                                      \
        EXPECT_STREQ(temp.what(), TEST_MESSAGE);               \
    } while (0)

// what does EXPECT_EQ_XXX do ?
// 1. test parse -> should be right value
// 2. test access -> should get right type and right value

#define EXPECT_EQ_TRUE(TEST_CONTENT)          \
    do {                                      \
        auto v = tijson::parse(TEST_CONTENT); \
        EXPECT_EQ(v.get_bool(), true);        \
        EXPECT_EQ_VALUE_TYPE(v, TRUE);        \
    } while (0)

#define EXPECT_EQ_FALSE(TEST_CONTENT)         \
    do {                                      \
        auto v = tijson::parse(TEST_CONTENT); \
        EXPECT_EQ(v.get_bool(), false);       \
        EXPECT_EQ_VALUE_TYPE(v, FALSE);       \
    } while (0)

#define EXPECT_EQ_NULL(TEST_CONTENT)          \
    do {                                      \
        auto v = tijson::parse(TEST_CONTENT); \
        EXPECT_EQ_VALUE_TYPE(v, NUL);         \
    } while (0)

#define EXPECT_EQ_NUMBER(TEST_CONTENT, TEST_NUMBER) \
    do {                                            \
        auto v = tijson::parse(TEST_CONTENT);       \
        EXPECT_EQ(v.get_number(), TEST_NUMBER);     \
        EXPECT_EQ_VALUE_TYPE(v, NUMBER);            \
    } while (0)

#define EXPECT_EQ_STRING(TEST_CONTENT, TEST_STRING) \
    do {                                            \
        auto v = tijson::parse(TEST_CONTENT);       \
        EXPECT_EQ(v.get_string(), TEST_STRING);     \
        EXPECT_EQ_VALUE_TYPE(v, STRING);            \
    } while (0)

#define EXPECT_EQ_ARRAY(TEST_CONTENT, TEST_STRING) \
    do {                                           \
        auto v   = tijson::parse(TEST_CONTENT);    \
        auto arr = v.get_array();                  \
        EXPECT_EQ(v.get_array(), TEST_STRING);     \
        EXPECT_EQ_VALUE_TYPE(v, STRING);           \
    } while (0)
