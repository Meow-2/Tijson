#include "test_utils.h"


#include <gtest/gtest_pred_impl.h>
#include <stdexcept>
#include <tijson.h>

TEST(PARSE_LITERAL, NUL)
{
    EXPECT_EQ_NULL("null");
    EXPECT_EQ_NULL(" null ");
    EXPECT_EQ_NULL("null ");
    EXPECT_EQ_NULL(" null");
    EXPECT_EQ_NULL("\rnull\n\t");
}

TEST(PARSE_LITERAL, TRUE)
{
    EXPECT_EQ_TRUE("true");
    EXPECT_EQ_TRUE(" true ");
    EXPECT_EQ_TRUE("true ");
    EXPECT_EQ_TRUE(" true");
    EXPECT_EQ_TRUE("\rtrue\n\t");
}

TEST(PARSE_LITERAL, FALSE)
{
    EXPECT_EQ_FALSE("false");
    EXPECT_EQ_FALSE(" false ");
    EXPECT_EQ_FALSE("false ");
    EXPECT_EQ_FALSE(" false");
    EXPECT_EQ_FALSE("\rfalse\n\t");
}

TEST(PARSE_LITERAL, EXPECT_VALUE)
{
    EXPECT_PARSE_THROW_MESSAGE(" ", "EXPECT_VALUE");
    EXPECT_PARSE_THROW_MESSAGE("   ", "EXPECT_VALUE");
    EXPECT_PARSE_THROW_MESSAGE("", "EXPECT_VALUE");
    EXPECT_PARSE_THROW_MESSAGE("\r\t\n  \r \n \t", "EXPECT_VALUE");
}

TEST(PARSE_LITERAL, ROOT_NOT_SINGULAR)
{
    EXPECT_PARSE_THROW_MESSAGE(" null a", "ROOT_NOT_SINGULAR");
    EXPECT_PARSE_THROW_MESSAGE(" nullasda ", "ROOT_NOT_SINGULAR");
    EXPECT_PARSE_THROW_MESSAGE(" truea", "ROOT_NOT_SINGULAR");
    EXPECT_PARSE_THROW_MESSAGE(" true \tsda ", "ROOT_NOT_SINGULAR");
    EXPECT_PARSE_THROW_MESSAGE(" falsea", "ROOT_NOT_SINGULAR");
    EXPECT_PARSE_THROW_MESSAGE(" false \tsda ", "ROOT_NOT_SINGULAR");
}
TEST(PARSE_LITERAL, INVALID_VALUE)
{
    EXPECT_PARSE_THROW_MESSAGE("nul", "INVALID_VALUE");
    EXPECT_PARSE_THROW_MESSAGE("fa\r", "INVALID_VALUE");
    EXPECT_PARSE_THROW_MESSAGE("tr\b", "INVALID_VALUE");
    EXPECT_PARSE_THROW_MESSAGE("nw\awer", "INVALID_VALUE");
    EXPECT_PARSE_THROW_MESSAGE("fqer", "INVALID_VALUE");
    EXPECT_PARSE_THROW_MESSAGE("ndsaf", "INVALID_VALUE");
}
