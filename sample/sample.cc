#include "tijson.h"

#include <iostream>

int main()
{
    {
        auto root = tijson::Parse(R"({"repo":"Tijson", "author":"Meow-2", "star":0})");
        root["author"].SetString(root["author"].GetString().substr(0, 4));
        root["star"] = root["star"].GetNumber() + 1;
        std::cout << root.Stringify() << '\n';
    }
    {
        auto json_val0 = tijson::Parse(R"({"Meow-2":"Tijson"})");
        auto json_val1 = tijson::Parser::Parse(R"({"Meow-2":"Tijson"})");
    }
    {
        auto json_val = tijson::Parse(R"({"Meow-2":"Tijson"})");
        if (json_val)
            std::cout << json_val.Stringify() << '\n';   //{ "Meow-2":"Tijson" }
        else
            tijson::PARSE_ERROR err = json_val.GetParseErrorCode();
    }
    {
        try {
            auto json_val = tijson::Parse(R"({"Meow-2":"Tijson"})");
            std::cout << json_val.Stringify() << '\n';   //{ "Meow-2":"Tijson" }
        }
        catch (tijson::ParseException e) {
            std::cout << e.what();
        }
    }
    {
        tijson::Value json_val = tijson::Parse(R"(
            {
                "Null" : null,
                "True" : true,
                "False" : false,
                "Number" : 314159E-5,
                "String" : "tijson",
                "Array" : [ "Meow-2", ["Hello\tWorld"]],
                "Object": {
                        "TIJSON":[123, "Tijson"]
                }
            }
        )");
        auto&         obj      = json_val.GetObject();
        std::cout << obj["Null"].IsNull() << '\n';        //  1
        std::cout << obj["True"].GetBool() << '\n';       //  1
        std::cout << obj["False"].GetBool() << '\n';      //  0
        std::cout << obj["Number"].GetNumber() << '\n';   //  3.14159
        std::cout << obj["String"].GetString() << '\n';   //  tijson
        auto& arr = obj["Array"].GetArray();
        std::cout << arr[0].GetString() << '\n';                 //  Meow-2
        std::cout << arr[1].GetArray()[0].GetString() << '\n';   //  Hello   World
        auto& objj        = obj["Object"].GetObject();
        auto& objj_tijson = objj["TIJSON"].GetArray();
        std::cout << objj_tijson[0].GetNumber() << '\n';   //  123
        std::cout << objj_tijson[1].GetString() << '\n';   //  Tijson
                                                           //
        // Set
        obj["Null"].SetString("NULL");
        std::cout << obj["Null"].GetString() << '\n';   // NULL
        obj["Null"].SetBool(true);
        std::cout << obj["Null"].GetBool() << '\n';   // 1
        obj["Null"].SetNumber(123);
        std::cout << obj["Null"].GetNumber() << '\n';   // 123
        obj["Null"].SetArray({"Meow-2", {"Hello\tWorld"}});
        auto& a = obj["Null"].GetArray();
        std::cout << a[0].GetString() << '\n';                 //  Meow-2
        std::cout << a[1].GetArray()[0].GetString() << '\n';   //  Hello   World
        obj["Null"].SetObject({
            {"TIJSON", {123, "Tijson"}}   // 显式初始化Object
        });
        auto& o        = obj["Null"].GetObject();
        auto& o_tijson = o["TIJSON"].GetArray();
        std::cout << o_tijson[0].GetNumber() << '\n';   //  123
        std::cout << o_tijson[1].GetString() << '\n';   //  Tijson
    }
    {
        tijson::Value me(tijson::Object{});
        me["name"]      = "Meow-2";
        me["age"]       = 24;
        me["interest"]  = {"Animation", "Coding", "Open Source"};
        me["learning"]  = {"C++", "Linux", "Vim"};
        me["remark"]    = tijson::Value();   //默认初始化为null
        me["this repo"] = tijson::Object({
            {"name", "Tijson"},
            {"star", 0},
            {"url", "https://github.com/Meow-2/Tijson"},
        });
        std::cout << me.Stringify() << '\n';
    }
    return 0;
}
