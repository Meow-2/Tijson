#ifndef INCLUDE_TIJSON_H
#define INCLUDE_TIJSON_H

/* NOTE: INCLUDE */

#include <cassert>
#include <cmath>
#include <codecvt>
#include <cstdio>
#include <locale>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

namespace tijson {

/*  NOTE: CLASS VALUE */

class Value final
{
    using array       = std::vector<Value>;
    using object      = std::unordered_map<std::string, Value>;
    using array_uptr  = std::unique_ptr<array>;
    using object_uptr = std::unique_ptr<object>;

public:
    /* the type of json value */
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
    Value() = default;

    // deep copy
    Value(Value const& rhs);
    Value& operator=(Value const& rhs);

    Value(Value&&) noexcept            = default;
    Value& operator=(Value&&) noexcept = default;

    /* type check */
    bool is_invalid() { return type_ == VALUE_TYPE::INVALID ? true : false; }
    bool is_null() { return type_ == VALUE_TYPE::NUL ? true : false; }
    bool is_true() { return type_ == VALUE_TYPE::TRUE ? true : false; }
    bool is_false() { return type_ == VALUE_TYPE::FALSE ? true : false; }
    bool is_number() { return type_ == VALUE_TYPE::NUMBER ? true : false; }
    bool is_string() { return type_ == VALUE_TYPE::STRING ? true : false; }
    bool is_array() { return type_ == VALUE_TYPE::ARRAY ? true : false; }
    bool is_object() { return type_ == VALUE_TYPE::OBJECT ? true : false; }

    /* getter setter */
    [[nodiscard]] VALUE_TYPE  get_type() const;
    [[nodiscard]] bool        get_bool() const;
    [[nodiscard]] double      get_number() const;
    [[nodiscard]] std::string get_string() const;
    [[nodiscard]] array       get_array() const;
    [[nodiscard]] object      get_object() const;

    void set_null();
    void set_bool(bool);
    void set_number(double);
    void set_string(std::string&&);
    void set_array(array&&);
    void set_object(object&&);

    /* value to json string */
    [[nodiscard]] std::string Stringify() const;

    bool operator==(Value const& rhs) const;

private:
    /* stringify utils */
    [[nodiscard]] std::string stringify_number() const;
    [[nodiscard]] std::string stringify_string() const;
    [[nodiscard]] std::string stringify_array() const;
    [[nodiscard]] std::string stringify_object() const;
    [[nodiscard]] std::string stringify_string(const std::string&) const;

    std::variant<double, std::string, array_uptr, object_uptr> data_{0.0};
    VALUE_TYPE                                                 type_{VALUE_TYPE::INVALID};
};

/* NOTE: CLASS PARSER */

class Parser final
{
    using str_itr = std::string_view::iterator;

public:
    /* copy deleted */
    Parser(Parser const&)            = delete;
    Parser& operator=(Parser const&) = delete;

    /* move undeclared */

    ~Parser() = default;

    /* parse content to json value */
    static Value Parse(std::string_view content);

private:
    /* constructor private */
    Parser(str_itr cur, str_itr end) : cur_(cur), end_(end) {}

    /* real parse */
    Value Parse();

    /* parse utils */
    Value parse_value();
    void  parse_whitespace();
    void  parse_null(Value&);
    void  parse_true(Value&);
    void  parse_false(Value&);
    void  parse_number(Value&);
    void  parse_string(Value&);
    void  parse_array(Value&);
    void  parse_object(Value&);

    /* parse string, return raw string */
    std::string parse_string();


    /* parse number util */
    template<char lower, char upper>
    bool is_digital(char ch);

    /* parse string util */
    bool is_invalid_char(char ch);

    /* parse unicode util */
    char16_t    parse_string_hex4();
    std::string parse_string_utf8();

    /* data */
    str_itr cur_;
    str_itr end_;
};

/* NOTE: PARSER EXCEPTION */

/* parse json string to value, if failed, throw an exception */
static Value parse(std::string_view content)
{
    Value result;
    try {
        result = Parser::Parse(content);
    }
    catch (std::invalid_argument e) {
        e.what();
    }
    return result;
}

/* NOTE: VALUE IMPLEMENTATION */
inline Value::Value(Value const& rhs) /*{{{*/
{
    this->type_ = rhs.type_;
    if (rhs.type_ == VALUE_TYPE::ARRAY)
        this->data_ = std::make_unique<array>(*std::get<array_uptr>(rhs.data_));
    else if (rhs.type_ == VALUE_TYPE::OBJECT)
        this->data_ = std::make_unique<object>(*std::get<object_uptr>(rhs.data_));
    else if (rhs.type_ == VALUE_TYPE::NUMBER)
        this->data_ = std::get<double>(rhs.data_);
    else if (rhs.type_ == VALUE_TYPE::STRING)
        this->data_ = std::get<std::string>(rhs.data_);
    else
        this->data_ = 0.0;
} /*}}}*/

inline Value& Value::operator=(Value const& rhs) /*{{{*/
{
    this->~Value();
    return *(new (this) Value(rhs));
} /*}}}*/

inline Value::VALUE_TYPE Value::get_type() const /*{{{*/
{
    return type_;
} /*}}}*/

inline bool Value::get_bool() const /*{{{*/
{
    assert((type_ == VALUE_TYPE::TRUE || type_ == VALUE_TYPE::FALSE) && "json value is not bool");
    return type_ == VALUE_TYPE::TRUE ? true : false;
} /*}}}*/

inline double Value::get_number() const /*{{{*/
{
    assert(type_ == VALUE_TYPE::NUMBER && "json value is not number");
    return std::get<double>(data_);
} /*}}}*/

inline std::string Value::get_string() const /*{{{*/
{
    assert(type_ == VALUE_TYPE::STRING && "json value is not string");
    return std::get<std::string>(data_);
} /*}}}*/

inline Value::array Value::get_array() const /*{{{*/
{
    array arr;
    if (std::get<array_uptr>(data_))
        arr = *std::get<array_uptr>(data_);
    return arr;
} /*}}}*/

inline Value::object Value::get_object() const /*{{{*/
{
    object obj;
    if (std::get<object_uptr>(data_))
        obj = *std::get<object_uptr>(data_);
    return obj;
} /*}}}*/

inline void Value::set_null() /*{{{*/
{
    data_ = 0.0;
    type_ = VALUE_TYPE::NUL;
} /*}}}*/

inline void Value::set_bool(bool tf) /*{{{*/
{
    data_ = 0.0;
    type_ = tf ? VALUE_TYPE::TRUE : VALUE_TYPE::FALSE;
} /*}}}*/

inline void Value::set_number(double n) /*{{{*/
{
    data_ = n;
    type_ = VALUE_TYPE::NUMBER;
} /*}}}*/

inline void Value::set_string(std::string&& s) /*{{{*/
{
    data_ = std::move(s);
    type_ = VALUE_TYPE::STRING;
} /*}}}*/

inline void Value::set_array(array&& arr) /*{{{*/
{
    data_ = std::make_unique<array>(std::move(arr));
    type_ = VALUE_TYPE::ARRAY;
} /*}}}*/

inline void Value::set_object(object&& obj) /*{{{*/
{
    data_ = std::make_unique<object>(std::move(obj));
    type_ = VALUE_TYPE::OBJECT;
} /*}}}*/

inline std::string Value::Stringify() const /*{{{*/
{
    switch (type_) {
    case VALUE_TYPE::NUL:
    {
        std::string result = "null";
        return result;
    }
    case VALUE_TYPE::TRUE:
    {
        std::string result = "true";
        return result;
    }
    case VALUE_TYPE::FALSE:
    {
        std::string result = "false";
        return result;
    }
    case VALUE_TYPE::NUMBER:
    {
        std::string result = stringify_number();
        return result;
    }
    case VALUE_TYPE::STRING:
    {
        std::string result = stringify_string();
        return result;
    }
    case VALUE_TYPE::ARRAY:
    {
        std::string result = stringify_array();
        return result;
    }
    case VALUE_TYPE::OBJECT:
    {
        std::string result = stringify_object();
        return result;
    }
    case VALUE_TYPE::INVALID: return "";
    }
} /*}}}*/

inline std::string Value::stringify_number() const /*{{{*/
{
    auto              fmt        = "%.17g";
    double            number_raw = std::get<double>(data_);
    auto              sz         = std::snprintf(nullptr, 0, fmt, number_raw);
    std::vector<char> buf(sz + 1);
    std::sprintf(&buf[0], fmt, number_raw);
    std::string result(buf.begin(), buf.end() - 1);
    return result;
} /*}}}*/

inline std::string Value::stringify_string() const /*{{{*/
{
    std::string result = "\"";
    for (auto const& ch : std::get<std::string>(data_)) {
        switch (ch) {
        case '\"': result += "\\\""; break;
        case '\\': result += "\\\\"; break;
        case '/': result += "\\/"; break;
        case '\b': result += "\\b"; break;
        case '\f': result += "\\f"; break;
        case '\n': result += "\\n"; break;
        case '\r': result += "\\r"; break;
        case '\t': result += "\\t"; break;
        default:
            auto temp = static_cast<unsigned char>(ch);
            if (temp < 0x20) {
                auto              fmt = "\\u%04X";
                auto              sz  = std::snprintf(nullptr, 0, fmt, temp);
                std::vector<char> buf(sz + 1);
                std::sprintf(&buf[0], fmt, temp);
                result.append(std::string(buf.begin(), buf.end() - 1));
            }
            else
                result += ch;
        }
    }
    result += '\"';
    return result;
} /*}}}*/

inline std::string Value::stringify_string(std::string const& str) const /*{{{*/
{
    std::string result = "\"";
    for (auto const& ch : str) {
        switch (ch) {
        case '\"': result += "\\\""; break;
        case '\\': result += "\\\\"; break;
        case '/': result += "\\/"; break;
        case '\b': result += "\\b"; break;
        case '\f': result += "\\f"; break;
        case '\n': result += "\\n"; break;
        case '\r': result += "\\r"; break;
        case '\t': result += "\\t"; break;
        default:
            auto temp = static_cast<unsigned char>(ch);
            if (temp < 0x20) {
                auto              fmt = "\\u04X";
                auto              sz  = std::snprintf(nullptr, 0, fmt, temp);
                std::vector<char> buf(sz + 1);
                std::sprintf(&buf[0], fmt, temp);
                result.append(std::string(buf.begin(), buf.end() - 1));
            }
            else
                result += ch;
        }
    }
    result += '\"';
    return result;
} /*}}}*/

inline std::string Value::stringify_array() const /*{{{*/
{
    std::string result = "[ ";
    for (int i = 0; i < std::get<array_uptr>(data_)->size(); i++) {
        if (i > 0)
            result += ", ";
        result += std::get<array_uptr>(data_)->at(i).Stringify();
    }
    result += " ]";
    return result;
} /*}}}*/

inline std::string Value::stringify_object() const /*{{{*/
{
    std::string result = "{\n";
    int         i      = 0;
    for (auto const& [key, val] : *(std::get<object_uptr>(data_))) {
        if (i > 0)
            result += ",\n";
        result += stringify_string(key) + ':' + val.Stringify();
        i++;
    }
    result += "\n}";
    return result;
} /*}}}*/

inline bool Value::operator==(Value const& rhs) const /*{{{*/
{
    if (type_ != rhs.type_)
        return false;
    if (type_ == VALUE_TYPE::TRUE || type_ == VALUE_TYPE::FALSE || type_ == VALUE_TYPE::NUL ||
        type_ == VALUE_TYPE::INVALID)
        return true;
    if (type_ == VALUE_TYPE::STRING || type_ == VALUE_TYPE::NUMBER)
        return data_ == rhs.data_;
    if (type_ == VALUE_TYPE::ARRAY)
        return *std::get<array_uptr>(data_) == *std::get<array_uptr>(rhs.data_);
    return *std::get<object_uptr>(data_) == *std::get<object_uptr>(rhs.data_);
} /*}}}*/

/* NOTE: PARSER IMPLEMENTATION */
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
inline Value Parser::Parse(std::string_view content) /*{{{*/
{
    return Parser(content.begin(), content.end()).Parse();
} /*}}}*/

inline void Parser::parse_whitespace() /*{{{*/
{
    while (*cur_ == ' ' || *cur_ == '\t' || *cur_ == '\n' || *cur_ == '\r')
        cur_++;
} /*}}}*/

template<char lower, char upper> /*{{{*/
inline bool Parser::is_digital(char ch)
{
    /* parse number helper */
    return ch >= lower && ch <= upper;
} /*}}}*/

inline bool Parser::is_invalid_char(char ch) /*{{{*/
{
    /* parse number helper */
    // unescaped = %x20-21 / %x23-5B / %x5D-10FFFF
    // '\x22' is '\"' , '\x5C' is '\\' , they will be deal, so the invalid char is less than
    // '\x20' unsigned char : range[0,255], signed char : range[-127,127]
    // When calulate char with number should static_cast
    // Because char could be unsigned or signed in different compiler
    return static_cast<unsigned char>(ch) < '\x20';
} /*}}}*/

inline char16_t Parser::parse_string_hex4() /*{{{*/
{
    /* parse unicode helper */
    char16_t surrogate = 0;
    for (int i = 0; i < 4; i++) {
        surrogate <<= 4;
        if ('0' <= *cur_ && *cur_ <= '9')
            surrogate |= *cur_ - '0';
        else if ('a' <= *cur_ && *cur_ <= 'f')
            surrogate |= *cur_ - ('a' - 10);
        else if ('A' <= *cur_ && *cur_ <= 'F')
            surrogate |= *cur_ - ('A' - 10);
        else
            throw std::invalid_argument("INVALID_UNICODE_HEX");
        ++cur_;
    }
    return surrogate;
} /*}}}*/

inline std::string Parser::parse_string_utf8() /*{{{*/
{
    std::u16string u16;
    char16_t       surrogate_h = parse_string_hex4();
    u16 += surrogate_h;
    if (0xD800 <= surrogate_h && surrogate_h <= 0xDBFF) {
        if (cur_[0] != '\\' || cur_[1] != 'u')
            throw std::invalid_argument("INVALID_UNICODE_SURROGATE");
        cur_ += 2;
        char16_t surrogate_l = parse_string_hex4();
        if (surrogate_l < 0xDC00 || 0xDFFF < surrogate_l)
            throw std::invalid_argument("INVALID_UNICODE_SURROGATE");
        u16 += surrogate_l;
    }
    std::string u8_conv =
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(u16);
    return u8_conv;
} /*}}}*/

inline Value Parser::Parse() /*{{{*/
{
    parse_whitespace();
    if (cur_ == end_)
        throw std::invalid_argument("EXPECT_VALUE");
    Value result = parse_value();
    parse_whitespace();
    if (cur_ != end_)
        throw std::invalid_argument("ROOT_NOT_SINGULAR");
    return result;
} /*}}}*/

inline Value Parser::parse_value() /*{{{*/
{
    Value result;
    switch (*cur_) {
    case 'n': ++cur_, parse_null(result); break;
    case 't': ++cur_, parse_true(result); break;
    case 'f': ++cur_, parse_false(result); break;
    case '\"': ++cur_, parse_string(result); break;
    case '[': ++cur_, parse_array(result); break;
    case '{': ++cur_, parse_object(result); break;
    default: parse_number(result);
    }
    return result;
} /*}}}*/

inline void Parser::parse_null(Value& val) /*{{{*/
{
    if (cur_[0] == 'u' && cur_[1] == 'l' && cur_[2] == 'l') {
        cur_ += 3;
        val.set_null();
        return;
    }
    throw std::invalid_argument("INVALID_VALUE");
} /*}}}*/

inline void Parser::parse_true(Value& val) /*{{{*/
{
    if (cur_[0] == 'r' && cur_[1] == 'u' && cur_[2] == 'e') {
        cur_ += 3;
        val.set_bool(true);
        return;
    }
    throw std::invalid_argument("INVALID_VALUE");
} /*}}}*/

inline void Parser::parse_false(Value& val) /*{{{*/
{
    if (cur_[0] == 'a' && cur_[1] == 'l' && cur_[2] == 's' && cur_[3] == 'e') {
        cur_ += 4;
        val.set_bool(false);
        return;
    }
    throw std::invalid_argument("INVALID_VALUE");
} /*}}}*/

inline void Parser::parse_number(Value& val) /*{{{*/
{
    auto number_begin = cur_;
    if (*cur_ == '-')
        ++cur_;
    if (*cur_ == '0')
        ++cur_;
    else if (is_digital<'1', '9'>(*cur_)) {
        ++cur_;
        while (is_digital<'0', '9'>(*cur_))
            ++cur_;
    }
    else
        throw std::invalid_argument("INVALID_VALUE");
    if (*cur_ == '.') {
        ++cur_;
        if (!is_digital<'0', '9'>(*cur_))
            throw std::invalid_argument("INVALID_VALUE");
        while (is_digital<'0', '9'>(*cur_))
            cur_++;
    }
    if (*cur_ == 'e' || *cur_ == 'E') {
        ++cur_;
        if (*cur_ == '+' || *cur_ == '-')
            ++cur_;
        if (!is_digital<'0', '9'>(*cur_))
            throw std::invalid_argument("INVALID_VALUE");
        while (is_digital<'0', '9'>(*cur_))
            ++cur_;
    }
    if (number_begin == cur_)
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
    double n = std::strtod(std::string(number_begin, cur_).c_str(), nullptr);
    if (n == HUGE_VAL || n == -HUGE_VAL)
        throw std::invalid_argument("NUMBER_TOO_BIG");
    val.set_number(n);
} /*}}}*/

inline std::string Parser::parse_string() /*{{{*/
{
    std::string s;
    while (true) {
        if (cur_ == end_)
            throw std::invalid_argument("MISS_QUOTATION_MARK");
        /* deal with invalid char */
        if (is_invalid_char(*cur_))
            throw std::invalid_argument("INVALID_STRING_CHAR");
        if (*cur_ == '\"') {
            ++cur_;
            break;
        }
        /* deal with escape */
        if (*cur_ == '\\') {
            if (++cur_ == end_)
                throw std::invalid_argument("INVALID_STRING_ESCAPE");
            switch (*cur_++) {
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
        /* deal with unescape char */
        s.push_back(*cur_++);
    }
    return s;
} /*}}}*/

inline void Parser::parse_string(Value& val) /*{{{*/
{
    std::string s;
    while (true) {
        if (cur_ == end_)
            throw std::invalid_argument("MISS_QUOTATION_MARK");
        // deal with invalid char
        if (is_invalid_char(*cur_))
            throw std::invalid_argument("INVALID_STRING_CHAR");
        if (*cur_ == '\"') {
            ++cur_;
            break;
        }
        /* deal with escape */
        if (*cur_ == '\\') {
            if (++cur_ == end_)
                throw std::invalid_argument("INVALID_STRING_ESCAPE");
            switch (*cur_++) {
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
        /* deal with unescape char */
        s.push_back(*cur_++);
    }
    val.set_string(std::move(s));
    return;
} /*}}}*/

inline void Parser::parse_array(Value& val) /*{{{*/
{
    std::vector<Value> result;
    parse_whitespace();
    if (*cur_ != ']') {
        while (true) {
            result.emplace_back(std::move(parse_value()));
            parse_whitespace();
            if (*cur_ == ',') {
                ++cur_;
                parse_whitespace();
                continue;
            }
            if (*cur_ == ']')
                break;
            throw std::invalid_argument("MISS_COMMA_OR_SQUARE_BRACKET");
        }
    }
    ++cur_;
    val.set_array(std::move(result));
    return;
} /*}}}*/

inline void Parser::parse_object(Value& val) /*{{{*/
{
    std::unordered_map<std::string, Value> result;
    parse_whitespace();
    if (*cur_ != '}') {
        while (true) {
            if (*cur_ != '\"')
                throw std::invalid_argument("MISS_KEY");
            ++cur_;
            std::string key = parse_string();
            parse_whitespace();
            if (*cur_ != ':')
                throw std::invalid_argument("MISS_COLON");
            ++cur_;
            parse_whitespace();
            Value val              = parse_value();
            result[std::move(key)] = std::move(val);
            parse_whitespace();
            if (*cur_ == ',') {
                ++cur_;
                parse_whitespace();
                continue;
            }
            if (*cur_ == '}')
                break;
            throw std::invalid_argument("MISS_COMMA_OR_CURLY_BRACKET");
        }
    }
    ++cur_;
    val.set_object(std::move(result));
    return;
} /*}}}*/

} /* namespace tijson */
#endif /* INCLUDE_TIJSON_H */
