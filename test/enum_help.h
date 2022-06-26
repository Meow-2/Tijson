#include "tijson.h"

inline const char* get_str_of_enum(tijson::value::VALUE_TYPE e)
{
    switch (e) {
    case tijson::value::VALUE_TYPE::INVALID: return "INVALID";
    case tijson::value::VALUE_TYPE::NUL: return "NUL";
    case tijson::value::VALUE_TYPE::TRUE: return "TRUE";
    case tijson::value::VALUE_TYPE::FALSE: return "FALSE";
    case tijson::value::VALUE_TYPE::NUMBER: return "NUMBER";
    case tijson::value::VALUE_TYPE::STRING: return "STRING";
    case tijson::value::VALUE_TYPE::ARRAY: return "ARRAY";
    case tijson::value::VALUE_TYPE::OBJECT: return "OBJECT";
    }
    return "";
}
