#ifndef INCLUDE_TIJSON_H
#define INCLUDE_TIJSON_H

/* NOTE: INCLUDE */

#include <cassert>
#include <cmath>
#include <codecvt>
#include <cstdio>
#include <exception>
#include <locale>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace tijson {

/*  NOTE: CLASS VALUE */

class Value final
{
    using Array      = std::vector<Value>;
    using Object     = std::unordered_map<std::string, Value>;
    using ArrayUPtr  = std::unique_ptr<Array>;
    using ObjectUPtr = std::unique_ptr<Object>;

public:
    /* the type of json value */
    enum class TYPE : char
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

    Value() = default;

    // deep copy
    Value(Value const& rhs);
    Value& operator=(Value const& rhs);

    Value(Value&&) noexcept            = default;
    Value& operator=(Value&&) noexcept = default;

    /* type check */
    bool IsInvalid() { return type_ == TYPE::INVALID ? true : false; }
    bool IsNull() { return type_ == TYPE::NUL ? true : false; }
    bool IsTrue() { return type_ == TYPE::TRUE ? true : false; }
    bool IsFalse() { return type_ == TYPE::FALSE ? true : false; }
    bool IsNumber() { return type_ == TYPE::NUMBER ? true : false; }
    bool IsString() { return type_ == TYPE::STRING ? true : false; }
    bool IsArray() { return type_ == TYPE::ARRAY ? true : false; }
    bool IsObject() { return type_ == TYPE::OBJECT ? true : false; }

    /* getter setter */
    [[nodiscard]] TYPE        GetType() const;
    [[nodiscard]] bool        GetBool() const;
    [[nodiscard]] double      GetNumber() const;
    [[nodiscard]] std::string GetString() const;
    [[nodiscard]] Array       GetArray() const;
    [[nodiscard]] Object      GetObject() const;

    void SetNull();
    void SetBool(bool);
    void SetNumber(double);
    void SetString(std::string&&);
    void SetArray(Array&&);
    void SetObject(Object&&);

    /* value to json string */
    [[nodiscard]] std::string Stringify() const;

    bool operator==(Value const& rhs) const;

private:
    /* stringify utils */
    [[nodiscard]] std::string StringifyNumber() const;
    [[nodiscard]] std::string StringifyString() const;
    [[nodiscard]] std::string StringifyArray() const;
    [[nodiscard]] std::string StringifyObject() const;
    [[nodiscard]] std::string StringifyString(std::string_view) const;

    std::variant<double, std::string, ArrayUPtr, ObjectUPtr> data_{0.0};
    TYPE                                                     type_{TYPE::INVALID};
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
    Value ParseValue();
    void  ParseWhitespace();
    void  ParseNull(Value&);
    void  ParseTrue(Value&);
    void  ParseFalse(Value&);
    void  ParseNumber(Value&);
    void  ParseString(Value&);
    void  ParseArray(Value&);
    void  ParseObject(Value&);

    /* parse string, return raw string */
    std::string ParseString();


    /* parse number util */
    template<char lower, char upper>
    bool IsDigital(char ch);

    /* parse string util */
    bool IsInvalidChar(char ch);

    /* parse unicode util */
    char16_t    ParseStringHex4();
    std::string ParseStringUtf8();

    /* data */
    str_itr cur_;
    str_itr end_;
};
/* NOTE: ENUM CLASS PARSER ERROR CODE */
enum class PARSE_ERROR : size_t
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
/* NOTE: CLASS PARSER EXCEPTION */
class ParseException : public std::exception
{
public:
    ParseException() = default;
    ParseException(PARSE_ERROR type, std::string what) : type_(type), what_(std::move(what)){};

    ParseException(ParseException const&)            = default;
    ParseException& operator=(ParseException const&) = default;

    ParseException(ParseException&&)            = default;
    ParseException& operator=(ParseException&&) = default;

    ~ParseException() noexcept override = default;

    [[nodiscard]] const char* what() const noexcept override
    {
        return what_.empty() ? "Unknown exception" : what_.c_str();
    }

    /* construct ParseError with string matched enum */
    template<PARSE_ERROR N>
    static ParseException ParseErrorWithString()
    {
        std::string_view sv  = __PRETTY_FUNCTION__;
        auto             pos = sv.find_last_of("::") + 1;
        return {N, std::string(sv.substr(pos, sv.size() - pos - 1))};
    }

private:
    std::string what_;
    PARSE_ERROR type_;
};

/* parse json string to value, if failed, throw an exception */
static Value Parse(std::string_view content)
{
    Value result;
    try {
        result = Parser::Parse(content);
    }
    catch (ParseException e) {
        /* e.what(); */
    }
    return result;
}

/* NOTE: VALUE IMPLEMENTATION */
inline Value::Value(Value const& rhs) /*{{{*/
{
    this->type_ = rhs.type_;
    if (rhs.type_ == TYPE::ARRAY)
        this->data_ = std::make_unique<Array>(*std::get<ArrayUPtr>(rhs.data_));
    else if (rhs.type_ == TYPE::OBJECT)
        this->data_ = std::make_unique<Object>(*std::get<ObjectUPtr>(rhs.data_));
    else if (rhs.type_ == TYPE::NUMBER)
        this->data_ = std::get<double>(rhs.data_);
    else if (rhs.type_ == TYPE::STRING)
        this->data_ = std::get<std::string>(rhs.data_);
    else
        this->data_ = 0.0;
} /*}}}*/

inline Value& Value::operator=(Value const& rhs) /*{{{*/
{
    this->~Value();
    return *(new (this) Value(rhs));
} /*}}}*/

// TODO: Fix variant bad access of GetObject[""], GetArray[-1]
inline Value::TYPE Value::GetType() const /*{{{*/
{
    return type_;
} /*}}}*/

inline bool Value::GetBool() const /*{{{*/
{
    assert((type_ == TYPE::TRUE || type_ == TYPE::FALSE) && "json value is not bool");
    return type_ == TYPE::TRUE ? true : false;
} /*}}}*/

inline double Value::GetNumber() const /*{{{*/
{
    assert(type_ == TYPE::NUMBER && "json value is not number");
    return std::get<double>(data_);
} /*}}}*/

inline std::string Value::GetString() const /*{{{*/
{
    assert(type_ == TYPE::STRING && "json value is not string");
    return std::get<std::string>(data_);
} /*}}}*/

inline Value::Array Value::GetArray() const /*{{{*/
{
    Array arr;
    if (std::get<ArrayUPtr>(data_))
        arr = *std::get<ArrayUPtr>(data_);
    return arr;
} /*}}}*/

inline Value::Object Value::GetObject() const /*{{{*/
{
    Object obj;
    if (std::get<ObjectUPtr>(data_))
        obj = *std::get<ObjectUPtr>(data_);
    return obj;
} /*}}}*/

inline void Value::SetNull() /*{{{*/
{
    data_ = 0.0;
    type_ = TYPE::NUL;
} /*}}}*/

inline void Value::SetBool(bool tf) /*{{{*/
{
    data_ = 0.0;
    type_ = tf ? TYPE::TRUE : TYPE::FALSE;
} /*}}}*/

inline void Value::SetNumber(double n) /*{{{*/
{
    data_ = n;
    type_ = TYPE::NUMBER;
} /*}}}*/

inline void Value::SetString(std::string&& s) /*{{{*/
{
    data_ = std::move(s);
    type_ = TYPE::STRING;
} /*}}}*/

inline void Value::SetArray(Array&& arr) /*{{{*/
{
    data_ = std::make_unique<Array>(std::move(arr));
    type_ = TYPE::ARRAY;
} /*}}}*/

inline void Value::SetObject(Object&& obj) /*{{{*/
{
    data_ = std::make_unique<Object>(std::move(obj));
    type_ = TYPE::OBJECT;
} /*}}}*/

inline std::string Value::Stringify() const /*{{{*/
{
    switch (type_) {
    case TYPE::NUL: return "null";
    case TYPE::TRUE: return "true";
    case TYPE::FALSE: return "false";
    case TYPE::NUMBER: return StringifyNumber();
    case TYPE::STRING: return StringifyString();
    case TYPE::ARRAY: return StringifyArray();
    case TYPE::OBJECT: return StringifyObject();
    case TYPE::INVALID: break;
    }
    return "";
} /*}}}*/

inline std::string Value::StringifyNumber() const /*{{{*/
{
    auto              fmt        = "%.17g";
    double            number_raw = std::get<double>(data_);
    auto              sz         = std::snprintf(nullptr, 0, fmt, number_raw);
    std::vector<char> buf(sz + 1);
    std::sprintf(&buf[0], fmt, number_raw);
    std::string result(buf.begin(), buf.end() - 1);
    return result;
} /*}}}*/

inline std::string Value::StringifyString() const /*{{{*/
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

inline std::string Value::StringifyString(std::string_view str) const /*{{{*/
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

inline std::string Value::StringifyArray() const /*{{{*/
{
    std::string result = "[ ";
    for (int i = 0; i < std::get<ArrayUPtr>(data_)->size(); i++) {
        if (i > 0)
            result += ", ";
        result += std::get<ArrayUPtr>(data_)->at(i).Stringify();
    }
    result += " ]";
    return result;
} /*}}}*/

inline std::string Value::StringifyObject() const /*{{{*/
{
    std::string result = "{\n";
    int         i      = 0;
    for (auto const& [key, val] : *(std::get<ObjectUPtr>(data_))) {
        if (i > 0)
            result += ",\n";
        result += StringifyString(key) + ':' + val.Stringify();
        i++;
    }
    result += "\n}";
    return result;
} /*}}}*/

inline bool Value::operator==(Value const& rhs) const /*{{{*/
{
    if (type_ != rhs.type_)
        return false;
    if (type_ == TYPE::TRUE || type_ == TYPE::FALSE || type_ == TYPE::NUL || type_ == TYPE::INVALID)
        return true;
    if (type_ == TYPE::STRING || type_ == TYPE::NUMBER)
        return data_ == rhs.data_;
    if (type_ == TYPE::ARRAY)
        return *std::get<ArrayUPtr>(data_) == *std::get<ArrayUPtr>(rhs.data_);
    return *std::get<ObjectUPtr>(data_) == *std::get<ObjectUPtr>(rhs.data_);
} /*}}}*/

/* NOTE: PARSER IMPLEMENTATION */
inline Value Parser::Parse(std::string_view content) /*{{{*/
{
    return Parser(content.begin(), content.end()).Parse();
} /*}}}*/

inline void Parser::ParseWhitespace() /*{{{*/
{
    while (*cur_ == ' ' || *cur_ == '\t' || *cur_ == '\n' || *cur_ == '\r')
        cur_++;
} /*}}}*/

template<char lower, char upper> /*{{{*/
inline bool Parser::IsDigital(char ch)
{
    /* parse number helper */
    return ch >= lower && ch <= upper;
} /*}}}*/

inline bool Parser::IsInvalidChar(char ch) /*{{{*/
{
    /* parse number helper */
    // unescaped = %x20-21 / %x23-5B / %x5D-10FFFF
    // '\x22' is '\"' , '\x5C' is '\\' , they will be deal, so the invalid char is less than
    // '\x20' unsigned char : range[0,255], signed char : range[-127,127]
    // When calulate char with number should static_cast
    // Because char could be unsigned or signed in different compiler
    return static_cast<unsigned char>(ch) < '\x20';
} /*}}}*/

inline char16_t Parser::ParseStringHex4() /*{{{*/
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

inline std::string Parser::ParseStringUtf8() /*{{{*/
{
    std::u16string u16;
    char16_t       surrogate_h = ParseStringHex4();
    u16 += surrogate_h;
    if (0xD800 <= surrogate_h && surrogate_h <= 0xDBFF) {
        if (cur_[0] != '\\' || cur_[1] != 'u')
            throw std::invalid_argument("INVALID_UNICODE_SURROGATE");
        cur_ += 2;
        char16_t surrogate_l = ParseStringHex4();
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
    ParseWhitespace();
    if (cur_ == end_)
        throw std::invalid_argument("EXPECT_VALUE");
    Value result = ParseValue();
    ParseWhitespace();
    if (cur_ != end_)
        throw std::invalid_argument("ROOT_NOT_SINGULAR");
    return result;
} /*}}}*/

inline Value Parser::ParseValue() /*{{{*/
{
    Value result;
    switch (*cur_) {
    case 'n': ++cur_, ParseNull(result); break;
    case 't': ++cur_, ParseTrue(result); break;
    case 'f': ++cur_, ParseFalse(result); break;
    case '\"': ++cur_, ParseString(result); break;
    case '[': ++cur_, ParseArray(result); break;
    case '{': ++cur_, ParseObject(result); break;
    default: ParseNumber(result);
    }
    return result;
} /*}}}*/

inline void Parser::ParseNull(Value& val) /*{{{*/
{
    if (cur_[0] == 'u' && cur_[1] == 'l' && cur_[2] == 'l') {
        cur_ += 3;
        val.SetNull();
        return;
    }
    throw std::invalid_argument("INVALID_VALUE");
} /*}}}*/

inline void Parser::ParseTrue(Value& val) /*{{{*/
{
    if (cur_[0] == 'r' && cur_[1] == 'u' && cur_[2] == 'e') {
        cur_ += 3;
        val.SetBool(true);
        return;
    }
    throw std::invalid_argument("INVALID_VALUE");
} /*}}}*/

inline void Parser::ParseFalse(Value& val) /*{{{*/
{
    if (cur_[0] == 'a' && cur_[1] == 'l' && cur_[2] == 's' && cur_[3] == 'e') {
        cur_ += 4;
        val.SetBool(false);
        return;
    }
    throw std::invalid_argument("INVALID_VALUE");
} /*}}}*/

inline void Parser::ParseNumber(Value& val) /*{{{*/
{
    auto number_begin = cur_;
    if (*cur_ == '-')
        ++cur_;
    if (*cur_ == '0')
        ++cur_;
    else if (IsDigital<'1', '9'>(*cur_)) {
        ++cur_;
        while (IsDigital<'0', '9'>(*cur_))
            ++cur_;
    }
    else
        throw std::invalid_argument("INVALID_VALUE");
    if (*cur_ == '.') {
        ++cur_;
        if (!IsDigital<'0', '9'>(*cur_))
            throw std::invalid_argument("INVALID_VALUE");
        while (IsDigital<'0', '9'>(*cur_))
            cur_++;
    }
    if (*cur_ == 'e' || *cur_ == 'E') {
        ++cur_;
        if (*cur_ == '+' || *cur_ == '-')
            ++cur_;
        if (!IsDigital<'0', '9'>(*cur_))
            throw std::invalid_argument("INVALID_VALUE");
        while (IsDigital<'0', '9'>(*cur_))
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
    val.SetNumber(n);
} /*}}}*/

inline std::string Parser::ParseString() /*{{{*/
{
    std::string s;
    while (true) {
        if (cur_ == end_)
            throw std::invalid_argument("MISS_QUOTATION_MARK");
        /* deal with invalid char */
        if (IsInvalidChar(*cur_))
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
                std::string u8stirng = ParseStringUtf8();
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

inline void Parser::ParseString(Value& val) /*{{{*/
{
    std::string s;
    while (true) {
        if (cur_ == end_)
            throw std::invalid_argument("MISS_QUOTATION_MARK");
        // deal with invalid char
        if (IsInvalidChar(*cur_))
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
                std::string u8stirng = ParseStringUtf8();
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
    val.SetString(std::move(s));
    return;
} /*}}}*/

inline void Parser::ParseArray(Value& val) /*{{{*/
{
    std::vector<Value> result;
    ParseWhitespace();
    if (*cur_ != ']') {
        while (true) {
            result.emplace_back(std::move(ParseValue()));
            ParseWhitespace();
            if (*cur_ == ',') {
                ++cur_;
                ParseWhitespace();
                continue;
            }
            if (*cur_ == ']')
                break;
            throw std::invalid_argument("MISS_COMMA_OR_SQUARE_BRACKET");
        }
    }
    ++cur_;
    val.SetArray(std::move(result));
    return;
} /*}}}*/

inline void Parser::ParseObject(Value& val) /*{{{*/
{
    std::unordered_map<std::string, Value> result;
    ParseWhitespace();
    if (*cur_ != '}') {
        while (true) {
            if (*cur_ != '\"')
                throw std::invalid_argument("MISS_KEY");
            ++cur_;
            std::string key = ParseString();
            ParseWhitespace();
            if (*cur_ != ':')
                throw std::invalid_argument("MISS_COLON");
            ++cur_;
            ParseWhitespace();
            Value val              = ParseValue();
            result[std::move(key)] = std::move(val);
            ParseWhitespace();
            if (*cur_ == ',') {
                ++cur_;
                ParseWhitespace();
                continue;
            }
            if (*cur_ == '}')
                break;
            throw std::invalid_argument("MISS_COMMA_OR_CURLY_BRACKET");
        }
    }
    ++cur_;
    val.SetObject(std::move(result));
    return;
} /*}}}*/

} /* namespace tijson */
#endif /* INCLUDE_TIJSON_H */
