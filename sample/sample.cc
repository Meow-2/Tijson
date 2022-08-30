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
            "Number" : 314159E-5,
            "String" : "tijson",
            "Array" : [ ["Hello\nWorld"]], 
            "Object": {
                    "TIJSON":[123, "Tijson"]
            }
        }
    )");
    auto&&        obj       = json_val2.GetObject();
    std::cout << "Null:" << obj["Null"].IsNull() << '\n';
    // Null:1
    std::cout << "True:" << obj["True"].GetBool() << '\n';
    // True:1
    std::cout << "False:" << obj["False"].GetBool() << '\n';
    // False:0
    std::cout << "Number:" << obj["Number"].GetNumber() << '\n';
    // Number:3.14159
    std::cout << "String:" << obj["String"].GetString() << '\n';
    // String:tijson
    std::cout << "Array[0][0]:" << obj["Array"].GetArray()[0].GetArray()[0].GetString() << '\n';
    // Hello
    // World
    std::cout << "Object[\"TIJSON\"][0]:"
              << obj["Object"].GetObject()["TIJSON"].GetArray()[0].GetNumber() << '\n';
    // 123
    return 0;
}
