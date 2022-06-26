#ifndef INCLUDE_TIJSON_H
#define INCLUDE_TIJSON_H

#include <cassert>
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

    // method
    void       set_type(VALUE_TYPE t) { type = t; }
    VALUE_TYPE get_type() { return type; }

    // type judgment
    bool is_invalid() { return type == VALUE_TYPE::INVALID ? true : false; }
    bool is_null() { return type == VALUE_TYPE::NUL ? true : false; }
    bool is_true() { return type == VALUE_TYPE::TRUE ? true : false; }
    bool is_false() { return type == VALUE_TYPE::FALSE ? true : false; }
    bool is_number() { return type == VALUE_TYPE::NUMBER ? true : false; }
    bool is_string() { return type == VALUE_TYPE::STRING ? true : false; }
    bool is_array() { return type == VALUE_TYPE::ARRAY ? true : false; }
    bool is_object() { return type == VALUE_TYPE::OBJECT ? true : false; }

    // get value
    bool get_bool()
    {
        assert((type == VALUE_TYPE::TRUE || type == VALUE_TYPE::FALSE) && "json value is not bool");
        return type == VALUE_TYPE::TRUE ? true : false;
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
    switch (*cur++) {
    case 'n': parse_null(result); break;
    case 't': parse_true(result); break;
    case 'f': parse_false(result); break;
    case '\"': parse_string(result); break;
    case '[': parse_array(result); break;
    case '{': parse_object(result); break;
    default: parse_number(result);
    }
    return result;
}

void parser::parse_null(value& val)
{
    if (cur[0] == 'u' && cur[1] == 'l' && cur[2] == 'l') {
        cur += 3;
        val.set_type(value::VALUE_TYPE::NUL);
    }
    else
        throw std::invalid_argument("INVALID_VALUE");
}
void parser::parse_true(value& val)
{
    if (cur[0] == 'r' && cur[1] == 'u' && cur[2] == 'e') {
        cur += 3;
        val.set_type(value::VALUE_TYPE::TRUE);
    }
    else
        throw std::invalid_argument("INVALID_VALUE");
}
void parser::parse_false(value& val)
{
    if (cur[0] == 'a' && cur[1] == 'l' && cur[2] == 's' && cur[3] == 'e') {
        cur += 4;
        val.set_type(value::VALUE_TYPE::FALSE);
    }
    else
        throw std::invalid_argument("INVALID_VALUE");
}
void parser::parse_number(value& val) {}
void parser::parse_string(value& val) {}
void parser::parse_array(value& val) {}
void parser::parse_object(value& val) {}
}   // namespace tijson
#endif   // TIJSON_IMP
