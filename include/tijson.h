#ifndef INCLUDE_TIJSON_H
#define INCLUDE_TIJSON_H

#include <cassert>
#include <cmath>
#include <codecvt>
#include <exception>
#include <iomanip>
#include <locale>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>
// --------------------------------------------------------------
// -                        DECLARE                             -
// --------------------------------------------------------------
namespace tijson {

class value final
{
    // alias
    using array       = std::vector<value>;
    using object      = std::unordered_map<std::string, value>;
    using array_uptr  = std::unique_ptr<array>;
    using object_uptr = std::unique_ptr<object>;

public:
    // the type of json value
    enum class VALUE_TYPE : char
    {
        NUL     = 'n',
        TRUE    = 'T',
        FALSE   = 'F',
        STRING  = 'S',
        NUMBER  = 'N',
        ARRAY   = 'A',
        OBJECT  = 'O',
        INVALID = 'I',
    };

public:
    // defualt constructor
    value() = default;

    // copy constructor(custom)
    value(value const& rhs)
    {
        this->type = rhs.type;
        if (rhs.type == VALUE_TYPE::ARRAY)
            this->data = std::make_unique<array>(*std::get<array_uptr>(rhs.data));
        else if (rhs.type == VALUE_TYPE::OBJECT)
            this->data = std::make_unique<object>(*std::get<object_uptr>(rhs.data));
        else if (rhs.type == VALUE_TYPE::NUMBER)
            this->data = std::get<double>(rhs.data);
        else if (rhs.type == VALUE_TYPE::STRING)
            this->data = std::get<std::string>(rhs.data);
    }
    value& operator=(value const& rhs)
    {
        this->~value();
        return *(new (this) value(rhs));
    }

    // move constructor
    value(value&&) noexcept            = default;
    value& operator=(value&&) noexcept = default;

    // type judgment
    bool is_invalid() { return type == VALUE_TYPE::INVALID ? true : false; }
    bool is_null() { return type == VALUE_TYPE::NUL ? true : false; }
    bool is_true() { return type == VALUE_TYPE::TRUE ? true : false; }
    bool is_false() { return type == VALUE_TYPE::FALSE ? true : false; }
    bool is_number() { return type == VALUE_TYPE::NUMBER ? true : false; }
    bool is_string() { return type == VALUE_TYPE::STRING ? true : false; }
    bool is_array() { return type == VALUE_TYPE::ARRAY ? true : false; }
    bool is_object() { return type == VALUE_TYPE::OBJECT ? true : false; }

    // getter && setter method
    [[nodiscard]] VALUE_TYPE get_type() const { return type; }
    void                     set_type(VALUE_TYPE t) { type = t; }

    [[nodiscard]] bool get_bool() const
    {
        assert((type == VALUE_TYPE::TRUE || type == VALUE_TYPE::FALSE) && "json value is not bool");
        return type == VALUE_TYPE::TRUE ? true : false;
    }
    void set_bool(bool tf) { set_type(tf ? VALUE_TYPE::TRUE : VALUE_TYPE::FALSE); }

    [[nodiscard]] double get_number() const
    {
        assert(type == VALUE_TYPE::NUMBER && "json value is not number");
        return std::get<double>(data);
    }
    void set_number(double n)
    {
        data = n;
        set_type(VALUE_TYPE::NUMBER);
    }
    [[nodiscard]] std::string get_string() const
    {
        assert(type == VALUE_TYPE::STRING && "json value is not string");
        return std::get<std::string>(data);
    }

    void set_string(std::string&& s)
    {
        data = std::move(s);
        set_type(VALUE_TYPE::STRING);
    }

    void set_array(array&& arr)
    {
        data = std::make_unique<array>(std::move(arr));
        set_type(VALUE_TYPE::ARRAY);
    }

    [[nodiscard]] array get_array() const
    {
        array arr;
        if (std::get<array_uptr>(data))
            arr = *std::get<array_uptr>(data);
        return arr;
    }

    void set_object(object&& obj)
    {
        data = std::make_unique<object>(std::move(obj));
        set_type(VALUE_TYPE::OBJECT);
    }

    [[nodiscard]] object get_object() const
    {
        object obj;
        if (std::get<object_uptr>(data))
            obj = *std::get<object_uptr>(data);
        return obj;
    }

private:
    std::variant<std::string, double, array_uptr, object_uptr> data;
    VALUE_TYPE                                                 type{VALUE_TYPE::NUL};
};

class parser final
{
    // alias
    using str_itr = std::string_view::iterator;

public:
    // delete copy
    parser(parser const&)            = delete;
    parser& operator=(parser const&) = delete;
    // destructor
    ~parser() = default;

    // parse content to val
    static value parse(std::string_view content)
    {
        return parser(content.begin(), content.end()).parse();
    }

private:
    // constructor
    parser(str_itr const& b, str_itr const& e) : cur(b), end(e) {}
    // real parse
    value parse();
    // parse tool

    value parse_value();
    void  parse_null(value&);
    void  parse_true(value&);
    void  parse_false(value&);
    void  parse_number(value&);
    void  parse_string(value&);
    void  parse_array(value&);
    void  parse_object(value&);

    void parse_whitespace()
    {
        while (*cur == ' ' || *cur == '\t' || *cur == '\n' || *cur == '\r')
            cur++;
    }

    // parse number helper
    template<char lower, char upper>
    bool is_digital(char ch)
    {
        return ch >= lower && ch <= upper;
    }

    // parse number helper
    bool is_invalid_char(char ch)
    {
        // unescaped = %x20-21 / %x23-5B / %x5D-10FFFF
        // '\x22' is '\"' , '\x5C' is '\\' , they will be deal, so the invalid char is less than
        // '\x20' unsigned char : range[0,255], signed char : range[-127,127]
        // When calulate char with number should static_cast
        // Because char could be unsigned or signed in different compiler
        return static_cast<unsigned char>(ch) < '\x20';
    }
    // parse unicode helper
    char16_t parse_string_hex4()
    {
        char16_t surrogate = 0;
        for (int i = 0; i < 4; i++) {
            surrogate <<= 4;
            if ('0' <= *cur && *cur <= '9')
                surrogate |= *cur - '0';
            else if ('a' <= *cur && *cur <= 'f')
                surrogate |= *cur - ('a' - 10);
            else if ('A' <= *cur && *cur <= 'F')
                surrogate |= *cur - ('A' - 10);
            else
                throw std::invalid_argument("INVALID_UNICODE_HEX");
            ++cur;
        }
        return surrogate;
    }

    std::string parse_string_utf8()
    {
        std::u16string u16;
        char16_t       surrogate_h = parse_string_hex4();
        u16 += surrogate_h;
        if (0xD800 <= surrogate_h && surrogate_h <= 0xDBFF) {
            if (cur[0] != '\\' || cur[1] != 'u')
                throw std::invalid_argument("INVALID_UNICODE_SURROGATE");
            cur += 2;
            char16_t surrogate_l = parse_string_hex4();
            if (surrogate_l < 0xDC00 || 0xDFFF < surrogate_l)
                throw std::invalid_argument("INVALID_UNICODE_SURROGATE");
            u16 += surrogate_l;
        }
        std::string u8_conv =
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(u16);
        return u8_conv;
    }

    str_itr cur;
    str_itr end;
};

// parse json string to value, if failed, throw an exception
inline value parse(std::string_view content)
{
    return parser::parse(content);
}

}   // namespace tijson


#endif   // INCLUDE_TIJSON_H

// WARN: comment the marco when test
// #define TIJSON_IMP
#ifdef TIJSON_IMP
// --------------------------------------------------------------
// -                      IMPLEMENTATION                        -
// --------------------------------------------------------------
namespace tijson {

// --------------------------------------------------------------
// -                   PARSER IMPLEMENTATION                    -
// --------------------------------------------------------------
// enum class ERROR_CODE : size_t
// {
//     OK = 0,
//     EXPECT_VALUE,
//     INVALID_VALUE,
//     ROOT_NOT_SINGULAR,
//     NUMBER_TOO_BIG,
//     MISS_QUOTATION_MARK,
//     INVALID_STRING_ESCAPE,
//     INVALID_STRING_CHAR,
//     INVALID_UNICODE_HEX,
//     INVALID_UNICODE_SURROGATE,
//     MISS_COMMA_OR_SQUARE_BRACKET,
//     MISS_KEY,
//     MISS_COLON,
//     MISS_COMMA_OR_CURLY_BRACKET
// };
value parser::parse()
{
    parse_whitespace();
    if (cur == end)
        throw std::invalid_argument("EXPECT_VALUE");
    value result = parse_value();
    parse_whitespace();
    if (cur != end)
        throw std::invalid_argument("ROOT_NOT_SINGULAR");
    return result;
}

value parser::parse_value()
{
    value result;
    switch (*cur) {
    case 'n': ++cur, parse_null(result); break;
    case 't': ++cur, parse_true(result); break;
    case 'f': ++cur, parse_false(result); break;
    case '\"': ++cur, parse_string(result); break;
    case '[': ++cur, parse_array(result); break;
    case '{': ++cur, parse_object(result); break;
    default: parse_number(result);
    }
    return result;
}

void parser::parse_null(value& val)   //{{{
{
    if (cur[0] == 'u' && cur[1] == 'l' && cur[2] == 'l') {
        cur += 3;
        val.set_type(value::VALUE_TYPE::NUL);
        return;
    }
    throw std::invalid_argument("INVALID_VALUE");
}   //}}}

void parser::parse_true(value& val)   //{{{
{
    if (cur[0] == 'r' && cur[1] == 'u' && cur[2] == 'e') {
        cur += 3;
        val.set_type(value::VALUE_TYPE::TRUE);
        return;
    }
    throw std::invalid_argument("INVALID_VALUE");
}   //}}}

void parser::parse_false(value& val)   //{{{
{
    if (cur[0] == 'a' && cur[1] == 'l' && cur[2] == 's' && cur[3] == 'e') {
        cur += 4;
        val.set_type(value::VALUE_TYPE::FALSE);
        return;
    }
    throw std::invalid_argument("INVALID_VALUE");
}   //}}}

void parser::parse_number(value& val)   //{{{
{
    auto number_begin = cur;
    if (*cur == '-')
        ++cur;
    if (*cur == '0')
        ++cur;
    else if (is_digital<'1', '9'>(*cur)) {
        ++cur;
        while (is_digital<'0', '9'>(*cur))
            ++cur;
    }
    else
        throw std::invalid_argument("INVALID_VALUE");
    if (*cur == '.') {
        ++cur;
        if (!is_digital<'0', '9'>(*cur))
            throw std::invalid_argument("INVALID_VALUE");
        while (is_digital<'0', '9'>(*cur))
            cur++;
    }
    if (*cur == 'e' || *cur == 'E') {
        ++cur;
        if (*cur == '+' || *cur == '-')
            ++cur;
        if (!is_digital<'0', '9'>(*cur))
            throw std::invalid_argument("INVALID_VALUE");
        while (is_digital<'0', '9'>(*cur))
            ++cur;
    }
    if (number_begin == cur)
        throw std::invalid_argument("INVALID_VALUE");
    // Abort: stod() If the converted value would fall out of the range, will throw an out_of_range
    // exception And std::stod() will throw out_of_range exception when converts subnormal value
    // https://stackoverflow.com/questions/48086830/stdstod-throws-out-of-range-error-for-a-string-that-should-be-valid
    //
    // double n;
    // try {
    //     n = std::strtod(cur);
    // }
    // catch (std::out_of_range e) {
    //     throw std::invalid_argument("NUMBER_TOO_BIG");
    // }
    double n = std::strtod(std::string(number_begin, cur).c_str(), nullptr);
    if (n == HUGE_VAL || n == -HUGE_VAL)
        throw std::invalid_argument("NUMBER_TOO_BIG");
    val.set_number(n);
}   //}}}

void parser::parse_string(value& val)   //{{{
{
    std::string s;
    while (true) {
        if (cur == end)
            throw std::invalid_argument("MISS_QUOTATION_MARK");
        // deal with invalid char
        if (is_invalid_char(*cur))
            throw std::invalid_argument("INVALID_STRING_CHAR");
        if (*cur == '\"') {
            ++cur;
            break;
        }
        // deal with escape
        if (*cur == '\\') {
            if (++cur == end)
                throw std::invalid_argument("INVALID_STRING_ESCAPE");
            switch (*cur++) {
            case '\"': s.push_back('\"'); break;
            case '\\': s.push_back('\\'); break;
            case '/': s.push_back('/'); break;
            case 'b': s.push_back('\b'); break;
            case 'f': s.push_back('\f'); break;
            case 'n': s.push_back('\n'); break;
            case 'r': s.push_back('\r'); break;
            case 't': s.push_back('\t'); break;
            case 'u':
            {
                std::string u8stirng = parse_string_utf8();
                s += u8stirng;
                break;
            }
            default: throw std::invalid_argument("INVALID_STRING_ESCAPE");
            }
            continue;
        }
        // deal with unescape char
        s.push_back(*cur++);
    }
    val.set_string(std::move(s));
    return;
}   //}}}

void parser::parse_array(value& val)   //{{{
{
    std::vector<value> result;
    parse_whitespace();
    if (*cur != ']') {
        while (true) {
            result.emplace_back(std::move(parse_value()));
            parse_whitespace();
            if (*cur == ',') {
                ++cur;
                parse_whitespace();
                continue;
            }
            if (*cur == ']')
                break;
            throw std::invalid_argument("MISS_COMMA_OR_SQUARE_BRACKET");
        }
    }
    ++cur;
    val.set_array(std::move(result));
    return;
}   //}}}

void parser::parse_object(value& val)   //{{{
{
    std::unordered_map<std::string, value> result;
    parse_whitespace();
    if (*cur != '}') {
        while (true) {
            if (*cur != '\"')
                throw std::invalid_argument("MISS_KEY");
            ++cur;
            value string_val;
            parse_string(string_val);
            std::string key = std::move(string_val.get_string());
            parse_whitespace();
            if (*cur != ':')
                throw std::invalid_argument("MISS_COLON");
            ++cur;
            parse_whitespace();
            value val              = parse_value();
            result[std::move(key)] = std::move(val);
            parse_whitespace();
            if (*cur == ',') {
                ++cur;
                parse_whitespace();
                continue;
            }
            if (*cur == '}')
                break;
            throw std::invalid_argument("MISS_COMMA_OR_CURLY_BRACKET");
        }
    }
    ++cur;
    val.set_object(std::move(result));
    return;
}   //}}}

}   // namespace tijson
#endif   // TIJSON_IMP
