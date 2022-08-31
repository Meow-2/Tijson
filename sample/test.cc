#include "../include/tijson.h"

#include <iostream>
#include <ostream>
using namespace std;
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

enum class ERROR_TYPE : char
{
    NO_ERROR = 'N',
    EXPECT_VALUE,
};

ostream& operator<<(ostream& x, ERROR_TYPE const& y)
{
    if (y == ERROR_TYPE::NO_ERROR)
        return x << "NO_ERROR";
    if (y == ERROR_TYPE::EXPECT_VALUE)
        return x << "ERROR_TYPE";
    return x;
}
int main()
{
    /* ERROR_TYPE a = ERROR_TYPE::EXPECT_VALUE; */
    /* std::cout << a; */
    auto v  = tijson::Parse("[ null , false , true , 124 , \"abc\" ]");
    auto v2 = v;
    std::cout << v2.Stringify();
    auto& arr = v2.GetArray();
    v         = 5;
    std::cout << v2.Stringify();
    std::cout << v.Stringify();
    /* v = {123, 213, 123}; */
    /* v                = {"asda", "123", 213, false, true}; */
    tijson::Value v3 = {
        {"asdasda", true},
        {"asdsda", false},
        {"asda", {3123, 2133, 12312}},
    };
    std::cout << v3.Stringify();
    /* if (v == "asdasd") {}; */
    /* std::cout << (a == ERROR_TYPE::NO_ERROR) << '\n'; */
    /* std::cout << GetStringFromTN<ERROR_TYPE, ERROR_TYPE::NO_ERROR>() << '\n'; */
}
