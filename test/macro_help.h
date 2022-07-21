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

#define EXPECT_EQ_NUMBER(TEST_CONTENT, TEST_NUMBER)                       \
    do {                                                                  \
        auto v = tijson::parse(TEST_CONTENT);                             \
        EXPECT_EQ(v.get_number(), TEST_NUMBER);                           \
        EXPECT_STREQ(get_str_of_enum(v.get_type()),                       \
                     get_str_of_enum(tijson::value::VALUE_TYPE::NUMBER)); \
    } while (0)
