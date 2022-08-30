#include "tijson.h"

#include <iostream>

int main()
{
    auto json_val0 = tijson::Parse(R"({"I am key":"I am value"})");
    auto json_val1 = tijson::Parser::Parse(R"({"I am key":"I am value"})");
    std::cout << "json_val0 == json_val1 ? " << (json_val0 == json_val1 ? true : false) << '\n';
    std::cout << "json_val0:\n" << json_val0.Stringify() << '\n';
    std::cout << "json_val1:\n" << json_val1.Stringify() << '\n';

    tijson::Value json_val2 = tijson::Parse(R"(
        {
            "Null" : null,
            "True" : true,
            "False" : false,
            "Number" : -213.123E-13,
            "String" : "HAPPY\u4455",
            "Array" : [ [null], [false], [ ["Hello\nWorld", -123.321,
            {"key":null}]]],
            "Object": {
                    "o_key":[123, "asdad"]
            }
        }
    )");
    auto&&        obj       = json_val2.GetObject();
    std::cout << obj["Null"].IsNull();
    std::cout << obj["True"].GetBool();
    std::cout << obj["False"].GetBool();
    std::cout << obj["Number"].GetNumber();
    std::cout << obj["String"].GetString();
    std::cout << obj["Array"].GetArray()[2].GetArray()[0].GetString();
    /* std::cout << obj["Object"].get_object()[2].GetArray()[0].get_string(); */
    std::cout << "json_val2:\n" << json_val2.Stringify() << '\n';
    return 0;
}
