#include "enum_help.h"

#include <gtest/gtest.h>
#include <stdexcept>
#include <tijson.h>

#define EXPECT_EQ_VALUE_TYPE(TEST_CONTENT, TEST_VALUE_TYPE)                        \
    do {                                                                           \
        EXPECT_STREQ(get_str_of_enum(tijson::parse(TEST_CONTENT).get_type()),      \
                     get_str_of_enum(tijson::value::VALUE_TYPE::TEST_VALUE_TYPE)); \
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

TEST(PARSE_VALUE, NULL)
{
    EXPECT_EQ_VALUE_TYPE("null", NUL);
    EXPECT_EQ_VALUE_TYPE(" null ", NUL);
    EXPECT_EQ_VALUE_TYPE("null ", NUL);
    EXPECT_EQ_VALUE_TYPE(" null", NUL);
    EXPECT_EQ_VALUE_TYPE("\rnull\n\t", NUL);
}

TEST(PARSE_VALUE, TRUE)
{
    EXPECT_EQ_VALUE_TYPE("true", TRUE);
    EXPECT_EQ_VALUE_TYPE(" true ", TRUE);
    EXPECT_EQ_VALUE_TYPE("true ", TRUE);
    EXPECT_EQ_VALUE_TYPE(" true", TRUE);
    EXPECT_EQ_VALUE_TYPE("\rtrue\n\t", TRUE);
}
TEST(PARSE_VALUE, FALSE)
{
    EXPECT_EQ_VALUE_TYPE("false", FALSE);
    EXPECT_EQ_VALUE_TYPE(" false ", FALSE);
    EXPECT_EQ_VALUE_TYPE("false ", FALSE);
    EXPECT_EQ_VALUE_TYPE(" false", FALSE);
    EXPECT_EQ_VALUE_TYPE("\rfalse\n\t", FALSE);
}

TEST(PARSE_VALUE, EXPECT_VALUE)
{
    EXPECT_PARSE_THROW_MESSAGE(" ", "EXPECT_VALUE");
    EXPECT_PARSE_THROW_MESSAGE("   ", "EXPECT_VALUE");
    EXPECT_PARSE_THROW_MESSAGE("", "EXPECT_VALUE");
    EXPECT_PARSE_THROW_MESSAGE("\r\t\n  \r \n \t", "EXPECT_VALUE");
}

TEST(PARSE_VALUE, ROOT_NOT_SINGULAR)
{
    EXPECT_PARSE_THROW_MESSAGE(" null a", "ROOT_NOT_SINGULAR");
    EXPECT_PARSE_THROW_MESSAGE(" nullasda ", "ROOT_NOT_SINGULAR");
    EXPECT_PARSE_THROW_MESSAGE(" truea", "ROOT_NOT_SINGULAR");
    EXPECT_PARSE_THROW_MESSAGE(" true \tsda ", "ROOT_NOT_SINGULAR");
    EXPECT_PARSE_THROW_MESSAGE(" falsea", "ROOT_NOT_SINGULAR");
    EXPECT_PARSE_THROW_MESSAGE(" false \tsda ", "ROOT_NOT_SINGULAR");
}
TEST(PARSE_VALUE, INVALID_VALUE)
{
    EXPECT_PARSE_THROW_MESSAGE("nul", "INVALID_VALUE");
    EXPECT_PARSE_THROW_MESSAGE("fa\r", "INVALID_VALUE");
    EXPECT_PARSE_THROW_MESSAGE("tr\b", "INVALID_VALUE");
    EXPECT_PARSE_THROW_MESSAGE("nw\awer", "INVALID_VALUE");
    EXPECT_PARSE_THROW_MESSAGE("fqer", "INVALID_VALUE");
    EXPECT_PARSE_THROW_MESSAGE("ndsaf", "INVALID_VALUE");
}
