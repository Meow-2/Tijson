#ifndef INCLUDE_TIJSON_H
#define INCLUDE_TIJSON_H

/* NOTE: INCLUDE */

#include <cmath>
#include <codecvt>
#include <cstdio>
#include <locale>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace tijson {

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

enum class ACCESS_ERROR : size_t
{
    NO_ERROR = 0,
    ACCESS_WRONG_TYPE,
};

template<class T>
class Exception;
/*  NOTE: CUSTOM EXCEPTION */
using ParseException  = Exception<PARSE_ERROR>;
using AccessException = Exception<ACCESS_ERROR>;

class Value;
/* NOTE: JSON ARRAY AND OBJECT */
using Array  = std::vector<Value>;
using Object = std::unordered_map<std::string, Value>;

/*  NOTE: CLASS VALUE */
class Value final
{

    using ArrayUPtr  = std::unique_ptr<Array>;
    using ObjectUPtr = std::unique_ptr<Object>;

public:
    /* the type of json value */
    enum class TYPE : char
    {
        INVALID = 'I',
        NUL     = 'n',
        TRUE    = 'T',
        FALSE   = 'F',
        STRING  = 'S',
        NUMBER  = 'N',
        ARRAY   = 'A',
        OBJECT  = 'O',
    };

    // constructor
    Value() = default;

    Value(bool bl) : data_(PARSE_ERROR::NO_ERROR), type_(bl ? TYPE::TRUE : TYPE::FALSE){};

    Value(double num) : data_(num), type_(TYPE::NUMBER){};
    Value(int num) : data_(static_cast<double>(num)), type_(TYPE::NUMBER){};

    Value(const char* p) : data_(std::string(p)), type_(TYPE::STRING){};
    Value(std::string const& str) : data_(str), type_(TYPE::STRING){};
    Value(std::string&& str) : data_(std::move(str)), type_(TYPE::STRING){};

    Value(Array const& arr) : data_(std::make_unique<Array>(arr)), type_(TYPE::ARRAY){};
    Value(Array&& arr) : data_(std::make_unique<Array>(std::move(arr))), type_(TYPE::ARRAY){};

    Value(Object const& arr) : data_(std::make_unique<Object>(arr)), type_(TYPE::OBJECT){};
    Value(Object&& arr) : data_(std::make_unique<Object>(std::move(arr))), type_(TYPE::OBJECT){};

    // deep copy
    Value(Value const& rhs);
    Value& operator=(Value const& rhs);

    // move
    Value(Value&&) noexcept;
    Value& operator=(Value&&) noexcept;

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
    [[nodiscard]] Array&      GetArray() const;
    [[nodiscard]] Object&     GetObject() const;
    [[nodiscard]] PARSE_ERROR GetParseErrorCode() const;

    void SetInvalid(PARSE_ERROR);
    void SetNull();
    void SetBool(bool);
    void SetNumber(double);
    void SetString(std::string&&);
    void SetArray(Array&&);
    void SetObject(Object&&);

    /* value to json string */
    [[nodiscard]] std::string Stringify() const;

    bool operator==(bool) const;
    bool operator==(Value const& rhs) const;
    bool operator!=(Value const& rhs) const;
         operator bool() const;

    /* disable other '==', '==' can only use to check if the value is valid,  */
    /* to campare Value() == std::string(), use Value().GetString() == std::string()*/
    template<class T>
    bool operator==(T const& arg) const = delete;


private:
    /* stringify utils */
    [[nodiscard]] std::string StringifyNumber() const;
    [[nodiscard]] std::string StringifyString() const;
    [[nodiscard]] std::string StringifyArray() const;
    [[nodiscard]] std::string StringifyObject() const;
    [[nodiscard]] std::string StringifyString(std::string_view) const;

    std::variant<PARSE_ERROR, std::string, double, ArrayUPtr, ObjectUPtr> data_{
        PARSE_ERROR::NO_ERROR};
    TYPE type_{TYPE::NUL};
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

/* NOTE: CLASS PARSER EXCEPTION */
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
        auto             pos = sv.find("N = tijson::") + 12;
        pos                  = sv.find_first_of("::", pos) + 2;
        auto end_pos         = sv.find_first_of(';', pos);
        return {N, std::string(sv.substr(pos, end_pos - pos))};
    }

private:
    std::string what_;
    T           type_;
};

/* parse json string to value, if failed, throw an exception */
static Value Parse(std::string_view content)
{
    Value result;
    try {
        result = Parser::Parse(content);
    }
    catch (ParseException& e) {
        result.SetInvalid(e.GetErrorCode());
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
        this->data_ = PARSE_ERROR::NO_ERROR;
} /*}}}*/

inline Value& Value::operator=(Value const& rhs) /*{{{*/
{
    this->~Value();
    return *(new (this) Value(rhs));
} /*}}}*/

inline Value::Value(Value&& rhs) noexcept : data_(std::move(rhs.data_)), type_(rhs.type_) /*{{{*/
{
    rhs.data_ = PARSE_ERROR::EXPECT_VALUE;
    rhs.type_ = TYPE::INVALID;
} /*}}}*/

inline Value& Value::operator=(Value&& rhs) noexcept /*{{{*/
{
    data_     = std::move(rhs.data_);
    type_     = rhs.type_;
    rhs.data_ = PARSE_ERROR::EXPECT_VALUE;
    rhs.type_ = TYPE::INVALID;
    return *this;
} /*}}}*/

// TODO: Fix variant bad access of GetObject[""], GetArray[-1]
inline Value::TYPE Value::GetType() const /*{{{*/
{
    return type_;
} /*}}}*/

inline PARSE_ERROR Value::GetParseErrorCode() const /*{{{*/
{
    return type_ == TYPE::INVALID ? std::get<PARSE_ERROR>(data_) : PARSE_ERROR::NO_ERROR;
} /*}}}*/

inline bool Value::GetBool() const /*{{{*/
{
    if (type_ == TYPE::TRUE || type_ == TYPE::FALSE)
        return type_ == TYPE::TRUE ? true : false;
    throw AccessException("VALUE_NOT_BOOL");
} /*}}}*/

inline double Value::GetNumber() const /*{{{*/
{
    if (type_ == TYPE::NUMBER)
        return std::get<double>(data_);
    throw AccessException("VALUE_NOT_NUMBER");
} /*}}}*/

inline std::string Value::GetString() const /*{{{*/
{
    if (type_ == TYPE::STRING)
        return std::get<std::string>(data_);
    throw AccessException("VALUE_NOT_STRING");
} /*}}}*/

inline Array& Value::GetArray() const /*{{{*/
{

    if (type_ == TYPE::ARRAY)
        return *std::get<ArrayUPtr>(data_);
    throw AccessException("VALUE_NOT_ARRAY");
} /*}}}*/

inline Object& Value::GetObject() const /*{{{*/
{
    if (type_ == TYPE::OBJECT)
        return *std::get<ObjectUPtr>(data_);
    throw AccessException("VALUE_NOT_OBJECT");
} /*}}}*/

inline void Value::SetInvalid(PARSE_ERROR parse_error) /*{{{*/
{
    data_ = parse_error;
    type_ = TYPE::INVALID;
} /*}}}*/

inline void Value::SetNull() /*{{{*/
{
    data_ = PARSE_ERROR::NO_ERROR;
    type_ = TYPE::NUL;
} /*}}}*/

inline void Value::SetBool(bool tf) /*{{{*/
{
    data_ = PARSE_ERROR::NO_ERROR;
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
                auto        fmt = "\\u%04X";
                auto        sz  = std::snprintf(nullptr, 0, fmt, temp);
                std::string buf(sz + 1, '\0');
                std::sprintf(&buf[0], fmt, temp);
                buf.pop_back();
                result += buf;
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
                auto        fmt = "\\u04X";
                auto        sz  = std::snprintf(nullptr, 0, fmt, temp);
                std::string buf(sz + 1, '\0');
                std::sprintf(&buf[0], fmt, temp);
                buf.pop_back();
                result += buf;
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

inline bool Value::operator==(bool rhs) const /*{{{*/
{
    // `if (Value == true) ` will use this, rather than Implicit transformation
    return static_cast<bool>(*this) == rhs;
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

inline bool Value::operator!=(Value const& rhs) const /*{{{*/
{
    return !(this->operator==(rhs));
} /*}}}*/

inline Value::operator bool() const /*{{{*/
{
    return type_ == TYPE::INVALID ? false : true;
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
            throw ParseException::ConstructWithErrorCode<PARSE_ERROR::INVALID_UNICODE_HEX>();
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
            throw ParseException::ConstructWithErrorCode<PARSE_ERROR::INVALID_UNICODE_SURROGATE>();
        cur_ += 2;
        char16_t surrogate_l = ParseStringHex4();
        if (surrogate_l < 0xDC00 || 0xDFFF < surrogate_l)
            throw ParseException::ConstructWithErrorCode<PARSE_ERROR::INVALID_UNICODE_SURROGATE>();
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
        throw ParseException::ConstructWithErrorCode<PARSE_ERROR::EXPECT_VALUE>();
    Value result = ParseValue();
    ParseWhitespace();
    if (cur_ != end_)
        throw ParseException::ConstructWithErrorCode<PARSE_ERROR::ROOT_NOT_SINGULAR>();
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
    throw ParseException::ConstructWithErrorCode<PARSE_ERROR::INVALID_VALUE>();
} /*}}}*/

inline void Parser::ParseTrue(Value& val) /*{{{*/
{
    if (cur_[0] == 'r' && cur_[1] == 'u' && cur_[2] == 'e') {
        cur_ += 3;
        val.SetBool(true);
        return;
    }
    throw ParseException::ConstructWithErrorCode<PARSE_ERROR::INVALID_VALUE>();
} /*}}}*/

inline void Parser::ParseFalse(Value& val) /*{{{*/
{
    if (cur_[0] == 'a' && cur_[1] == 'l' && cur_[2] == 's' && cur_[3] == 'e') {
        cur_ += 4;
        val.SetBool(false);
        return;
    }
    throw ParseException::ConstructWithErrorCode<PARSE_ERROR::INVALID_VALUE>();
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
        throw ParseException::ConstructWithErrorCode<PARSE_ERROR::INVALID_VALUE>();
    if (*cur_ == '.') {
        ++cur_;
        if (!IsDigital<'0', '9'>(*cur_))
            throw ParseException::ConstructWithErrorCode<PARSE_ERROR::INVALID_VALUE>();
        while (IsDigital<'0', '9'>(*cur_))
            cur_++;
    }
    if (*cur_ == 'e' || *cur_ == 'E') {
        ++cur_;
        if (*cur_ == '+' || *cur_ == '-')
            ++cur_;
        if (!IsDigital<'0', '9'>(*cur_))
            throw ParseException::ConstructWithErrorCode<PARSE_ERROR::INVALID_VALUE>();
        while (IsDigital<'0', '9'>(*cur_))
            ++cur_;
    }
    if (number_begin == cur_)
        throw ParseException::ConstructWithErrorCode<PARSE_ERROR::INVALID_VALUE>();
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
        throw ParseException::ConstructWithErrorCode<PARSE_ERROR::NUMBER_TOO_BIG>();
    val.SetNumber(n);
} /*}}}*/

inline std::string Parser::ParseString() /*{{{*/
{
    std::string s;
    while (true) {
        if (cur_ == end_)
            throw ParseException::ConstructWithErrorCode<PARSE_ERROR::MISS_QUOTATION_MARK>();
        /* deal with invalid char */
        if (IsInvalidChar(*cur_))
            throw ParseException::ConstructWithErrorCode<PARSE_ERROR::INVALID_STRING_CHAR>();
        if (*cur_ == '\"') {
            ++cur_;
            break;
        }
        /* deal with escape */
        if (*cur_ == '\\') {
            if (++cur_ == end_)
                throw ParseException::ConstructWithErrorCode<PARSE_ERROR::INVALID_STRING_ESCAPE>();
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
            default:
                throw ParseException::ConstructWithErrorCode<PARSE_ERROR::INVALID_STRING_ESCAPE>();
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
            throw ParseException::ConstructWithErrorCode<PARSE_ERROR::MISS_QUOTATION_MARK>();
        // deal with invalid char
        if (IsInvalidChar(*cur_))
            throw ParseException::ConstructWithErrorCode<PARSE_ERROR::INVALID_STRING_CHAR>();
        if (*cur_ == '\"') {
            ++cur_;
            break;
        }
        /* deal with escape */
        if (*cur_ == '\\') {
            if (++cur_ == end_)
                throw ParseException::ConstructWithErrorCode<PARSE_ERROR::INVALID_STRING_ESCAPE>();
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
            default:
                throw ParseException::ConstructWithErrorCode<PARSE_ERROR::INVALID_STRING_ESCAPE>();
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
            result.emplace_back(ParseValue());
            ParseWhitespace();
            if (*cur_ == ',') {
                ++cur_;
                ParseWhitespace();
                continue;
            }
            if (*cur_ == ']')
                break;
            throw ParseException::ConstructWithErrorCode<
                PARSE_ERROR::MISS_COMMA_OR_SQUARE_BRACKET>();
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
                throw ParseException::ConstructWithErrorCode<PARSE_ERROR::MISS_KEY>();
            ++cur_;
            std::string key = ParseString();
            ParseWhitespace();
            if (*cur_ != ':')
                throw ParseException::ConstructWithErrorCode<PARSE_ERROR::MISS_COLON>();
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
            throw ParseException::ConstructWithErrorCode<
                PARSE_ERROR::MISS_COMMA_OR_CURLY_BRACKET>();
        }
    }
    ++cur_;
    val.SetObject(std::move(result));
    return;
} /*}}}*/

} /* namespace tijson */
#endif /* INCLUDE_TIJSON_H */
