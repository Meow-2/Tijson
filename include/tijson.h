#ifndef INCLUDE_TIJSON_H
#define INCLUDE_TIJSON_H

#include <cassert>
#include <cmath>
#include <exception>
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
    using array_ptr  = std::unique_ptr<std::vector<value>>;
    using object_ptr = std::unique_ptr<std::unordered_map<std::string, value>>;

public:
    // the type of json value
    enum class VALUE_TYPE : char
    {
        NUL,
        TRUE,
        FALSE,
        STRING,
        NUMBER,
        ARRAY,
        OBJECT,
        INVALID,
    };

public:
    // defualt constructor
    value() = default;

    // copy constructor(custom)
    // value(value const& rhs) = delete;
    // value& operator=(value const& rhs) = delete;

    // move constructor
    value(value&&) noexcept = default;
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

private:
    std::variant<std::string, double, array_ptr, object_ptr> data;
    VALUE_TYPE                                               type{VALUE_TYPE::NUL};
};

class parser final
{
    // alias
    using str_itr = std::string_view::iterator;

public:
    // delete copy
    parser(parser const&) = delete;
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
    bool is_unescaped(char ch)
    {
        return ch >= '\x20' && ch <= '\x21' || ch >= '\x23' && ch <= '\x5B' || ch >= '\x5D';
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


// #define TIJSON_IMP
#ifdef TIJSON_IMP
// --------------------------------------------------------------
// -                      IMPLEMENTATION                        -
// --------------------------------------------------------------
namespace tijson {


// --------------------------------------------------------------
// -                   VALUE IMPLEMENTATION                     -
// --------------------------------------------------------------
// value::value(value const& rhs)
// {
//     // TODO: deep copy
// }
// value& value::operator=(value const& rhs)
// {
//     // TODO: deep copy
//     return *this;
// }


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

void parser::parse_null(value& val)
{ /*{{{*/
    if (cur[0] == 'u' && cur[1] == 'l' && cur[2] == 'l') {
        cur += 3;
        val.set_type(value::VALUE_TYPE::NUL);
        return;
    }
    throw std::invalid_argument("INVALID_VALUE");
} /*}}}*/
void parser::parse_true(value& val)
{ /*{{{*/
    if (cur[0] == 'r' && cur[1] == 'u' && cur[2] == 'e') {
        cur += 3;
        val.set_type(value::VALUE_TYPE::TRUE);
        return;
    }
    throw std::invalid_argument("INVALID_VALUE");
} /*}}}*/
void parser::parse_false(value& val)
{ /*{{{*/
    if (cur[0] == 'a' && cur[1] == 'l' && cur[2] == 's' && cur[3] == 'e') {
        cur += 4;
        val.set_type(value::VALUE_TYPE::FALSE);
        return;
    }
    throw std::invalid_argument("INVALID_VALUE");
} /*}}}*/
void parser::parse_number(value& val)
{ /*{{{*/
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
} /*}}}*/
// TODO: string parse
void parser::parse_string(value& val)
{
    std::string s;
    while (true) {
        if (cur == end)
            throw std::invalid_argument("MISS_QUOTATION_MARK");
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
            case 'u': break;
            default: throw std::invalid_argument("INVALID_STRING_ESCAPE");
            }
            continue;
        }
        // deal with unescape
        if (is_unescaped(*cur)) {
            s.push_back(*cur++);
            continue;
        }
        throw std::invalid_argument("INVALID_STRING_CHAR");
    }
    val.set_string(std::move(s));
    return;
}
// TODO: array parse
void parser::parse_array(value& val) {}
// TODO: object parse
void parser::parse_object(value& val) {}
}   // namespace tijson
#endif   // TIJSON_IMP
