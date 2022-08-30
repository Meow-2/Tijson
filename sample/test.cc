#include "../include/tijson.h"

#include <iostream>

template<class T, T N>
std::string GetStringFromTN()
{
    std::string str = __PRETTY_FUNCTION__;
    auto        pos = str.find_last_of("::") + 1;
    return str.substr(pos, str.size() - pos - 1);
};
template<class T>
std::string GetStringFromERROR(T n)
{
    if (n == 1)
        return GetStringFromTN<T, (T)1>();
};


enum class ERROR_TYPE : size_t
{
    NO_ERROR = 0,
    EXPECT_VALUE,
    INVALID_VALUE,
    ROOT_NOT_SINGULAR,
    NUMBER_TOO_BIG,
    MISS_QUOTATION_MARK,
    INVALID_STRING_ESCAPE,
    INVALID_STRING_CHAR,
    INVALID_UNICODE_HEX,
    INVALID_UNICODE_SURROGATE,
    MISS_COMMA_OR_SQUARE_BRACKET,
    MISS_KEY,
    MISS_COLON,
    MISS_COMMA_OR_CURLY_BRACKET
};

int main()
{
    std::cout << GetStringFromTN<ERROR_TYPE, ERROR_TYPE::NO_ERROR>() << '\n';
    /* std::cout << fun<ERROR_TYPE::NO_ERROR>() << '\n'; */
}
/* std::string content = R"( */
/* { */
/* "repo": "meojson", */
/* "author": { */
/*     "MistEO": "https://github.com/MistEO", */
/*     "ChingCdesu": "https://github.com/ChingCdesu" */
/* }, */
/* "list": [ */
/*     1, */
/*     2, */
/*     3 */
/* ], */
/* "str": "abc", */
/* "num": 3.1416, */
/* "A_obj": { */
/*     "B_arr": [ */
/*         { */
/*             "C_str": "you found me!" */
/*         } */
/*     ] */
/* } */
/* } */
/* )"; */
