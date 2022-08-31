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
template<class T>
class Exception : public std::exception
{
public:
    Exception() = default;
    Exception(std::string what) : what_(std::move(what)){};
    Exception(T type, std::string what) : what_(std::move(what)), type_(type){};

    Exception(Exception const&)            = default;
    Exception& operator=(Exception const&) = default;

    Exception(Exception&&) noexcept            = default;
    Exception& operator=(Exception&&) noexcept = default;

    ~Exception() noexcept override = default;

    [[nodiscard]] const char* what() const noexcept override
    {
        return what_.empty() ? "Unknown exception" : what_.c_str();
    }

    /* get error_code */
    T GetErrorCode() { return type_; }

    /* construct ParseError with string matched enum */
    template<T N>
    static Exception ConstructWithErrorCode()
    {
        std::string_view sv  = __PRETTY_FUNCTION__;
        auto             pos = sv.find_last_of("::") + 1;
        return {N, std::string(sv.substr(pos, sv.size() - pos - 1))};
    }

private:
    std::string what_;
    T           type_;
};

int main()
{
    using ab = Exception<ERROR_TYPE>;
    auto a   = ab::ConstructWithErrorCode<ERROR_TYPE::NO_ERROR>();
    std::cout << a.what();
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
    /* v.SetArray({"asda", "123", {213, false}, true}); */
    std::cout << v.Stringify();
    v.SetObject({
        {"asdasda", true},
        {"asdsda", false},
        {"asda", tijson::Array({"asdasd", true})},
    });
    std::cout << v.Stringify();
    /* if (v == "asdasd") {}; */
    /* std::cout << (a == ERROR_TYPE::NO_ERROR) << '\n'; */
    /* std::cout << GetStringFromTN<ERROR_TYPE, ERROR_TYPE::NO_ERROR>() << '\n'; */
    v                = tijson::Array({123, 123, tijson::Array({123, 123})});
    tijson::Value v4 = v.GetArray();

    std::cout << v.Stringify();
    v = 5;
    std::cout << v4.Stringify();
    v = tijson::Object({
        {"meow-2", "tijosn"},
        {"tijson", true},
        {"asd", tijson::Array({123, 123, 123})},
        {"asdasd", tijson::Object({
                       {"asdasadsas", true},
                       {"meow-2", "tijosn"},
                       {"tijson", true},
                   })},
    });
    std::cout << v.Stringify();
    v["meow-2"]           = "sadasdadsa";
    v["meow-2"]           = "sad";
    v["meow-3"]           = "sad";
    v["asd"][2]           = "sdasd";
    v["asdasd"]["meow-2"] = false;
    for (auto&& [key, val] : v.GetObject()) {
        std::cout << key << "'s homepage: " << val.Stringify() << std::endl;
    }
    std::cout << v.Stringify();
}
