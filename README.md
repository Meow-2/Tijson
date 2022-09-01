<!-- Improved compatibility of back to top link: See: https://github.com/othneildrew/Best-README-Template/pull/73 -->

<a name="readme-top"></a>

<!--
*** Thanks for checking out the Best-README-Template. If you have a suggestion
*** that would make this better, please fork the repo and create a pull request
*** or simply open an issue with the tag "enhancement".
*** Don't forget to give the project a star!
*** Thanks again! Now go create something AMAZING! :D
-->

<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->

[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]

<div align="center">

  <h3 align="center">Tijson</h3>

  <p align="center">
    An easy-to-use json parser/generator written in modern cpp
  </p>
</div>

<!-- [简体中文](https://github.com/Meow-2/Tijson/blob/main/README_zh_CN.md) -->

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li><a href="#about-tijson">About Tijson</a></li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#Example">Example</a></li>
        <li><a href="#Parse">Parse</a></li>
        <li><a href="#Access">Access</a></li>
        <li><a href="#Serialization">Serialization</a></li>
        <li><a href="#Compare">Compare</a></li>
      </ul>
    </li>
    <li><a href="#Prerequisites">Prerequisites</a></li>
    <li><a href="#TODO">TODO</a></li>
    <li><a href="#Acknowledgments">Acknowledgments</a></li>
    <li><a href="#License">License</a></li>
  </ol>
</details>

<!-- ABOUT THE PROJECT -->

## About Tijson

Tijson is a standard recursive descent JSON parser / generator written based on C++ 17.Cross platform (Windows / Linux / OS X) and cross compiler (GCC / Clang). It only supports UTF-8 text, aims to be simple ,lightweight and easy to use, and supports a variety of error handling methods.

<p align="right"><a href="#readme-top">back to top</a></p>

## Getting Started

Tijson is a header-only library, you only need to copy the `tijson.h` in the `include` folder in this repository to your project directory and include it as a header file to use it.

```cpp
#include "tijson.h"
```

### Example

```Cpp
#include "tijson.h"
#include <iostream>
int main() {
    auto root = tijson::Parse(R"({"repo":"Tijson", "author":"Meow-2", "star":0})");
    root["author"].SetString(root["author"].GetString().substr(0, 4));
    root["star"] = root["star"].GetNumber()+1;
    std::cout << root.Stringify()<<'\n';
    //{ "star":1, "author":"Meow", "repo":"Tijson" }
    return 0;
}
```

### Parse

```cpp
// two methods
auto json_val0 = tijson::Parse(R"({"Meow-2":"Tijson"})");
auto json_val1 = tijson::Parser::Parse(R"({"Meow-2":"Tijson"})");
// auto is tijson::Value, json_val0 is the same as json_val1
```

The difference between the above two methods is that in the first method, when the parsing fails, the error code will be written into the Value, and the program continues to execute, while the second method directly throws an exception and the program terminates.

**Example of Error Code**

```Cpp
auto json_val = tijson::Parse(R"({"Meow-2":"Tijson"})");
if (json_val)
    std::cout << json_val.Stringify() << '\n';   //{ "Meow-2":"Tijson" }
else
    tijson::PARSE_ERROR err = json_val.GetParseErrorCode();
```

**The Definition of Parse Error Code**

```
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

```

**Example of Exception**

```
try{
    auto json_val = tijson::Parse(R"({"Meow-2":"Tijson"})");
    std::cout << json_val.Stringify() << '\n';   //{ "Meow-2":"Tijson" }
}catch(tijson::ParseException e){
    std::cout<<e.what();
}
```

When an exception is thrown, the information carried by the exception is the string corresponding to the error code, which has the same meaning as the error code. Unlike the error code, if it is not caught, the program will be terminated.

### Access

```cpp
tijson::Value json_val = tijson::Parse(R"(
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
```

**Example of Get/Set**

```Cpp

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

```

The json value parsed from the json text is stored in the `tijson::Value` object.You need to use `Get` of the corresponding type to get its value, but you need to pay attention to the following three points:

The json value of `null` type can only be checked by `IsNull()`, but cannot be gotten. Similarly, each json type value has its corresponding `Is` methods

- `Array` and `Object` are actually `std::vector<tijson::Value>` and `std::unordermap<std::string, tijson::Value>`
- `Get` with the wrong type will throw an `AccessException` exception

For any kind of `tijson::Value`, you can use `Set` to set it to any type of `json` value, such as freely modifying the above `obj["Null"]`.

```Cpp
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
    {"TIJSON", {123, "Tijson"}}
});
auto& o        = obj["Null"].GetObject();
auto& o_tijson = o["TIJSON"].GetArray();
std::cout << o_tijson[0].GetNumber() << '\n';   //  123
std::cout << o_tijson[1].GetString() << '\n';   //  Tijson
```

For `SetArray()`, you can use `std::initializer_list` method and it supports nesting of `std::initializer_list`.
For `SetObject()`, you can also use the `std::initializer_list` method, the nesting of `std::initializer_list` is not supported (it will be regarded as `Array`).
If you want to implement `Oject` nesting, you need to explicitly specify it as `tijson::Object`, such as:

```Cpp
obj["Null"].SetObject({
            { "TIJSON", tijson::Object({ {"Meow-2", "Tijson"} }) }
        });
```

**Example of Assignment Operator**

Get/Set directly accesses/modifies inside `tojson::Value`, and can also use assignment instead of Set, but there will be overhead of implicit construction and movement.

```Cpp

obj["Null"] = "NULL";
std::cout << obj["Null"].GetString() << '\n';   // NULL

obj["Null"] = true;
std::cout << obj["Null"].GetBool() << '\n';   // 1

obj["Null"] = 123;
std::cout << obj["Null"].GetNumber() << '\n';   // 123

obj["Null"] = {"Meow-2", {"Hello\tWorld"}};
auto& a = obj["Null"].GetArray();
std::cout << a[0].GetString() << '\n';                 //  Meow-2
std::cout << a[1].GetArray()[0].GetString() << '\n';   //  Hello   World

obj["Null"] = tijson::Object({
    {"TIJSON", {123, "Tijson"}}   // Explicit Initialize Object
});
auto& o        = obj["Null"].GetObject();
auto& o_tijson = o["TIJSON"].GetArray();
std::cout << o_tijson[0].GetNumber() << '\n';   //  123
std::cout << o_tijson[1].GetString() << '\n';   //  Tijson

```

When use assignment operator to assignment `tijson::Object`, `std::initializer_list` is not supported, because it will be regarded as `Array`.

### Serialization

The `Stringify()` member function can be used to generate compact strings for any type of json value.

```Cpp
tijson::Value me(tijson::Object{});
me["name"]      = "Meow-2";
me["age"]       = 24;
me["interest"]  = {"Animation", "Coding", "Open Source"};
me["learning"]  = {"C++", "Linux", "Vim"};
me[""] = Value();       //default initialize as `null`
me["this repo"] = tijson::Object({
    {"name", "Tijson"},
    {"star", 0},
    {"url", "https://github.com/Meow-2/Tijson"},
});
std::cout << me.Stringify() << '\n';
// { "this repo":{ "url":"https:\/\/github.com\/Meow-2\/Tijson", "star":0, "name":"Tijson" }, "remark":null, "learning":[ "C++", "Lin
// ux", "Vim" ], "interest":[ "Animation", "Coding", "Open Source" ], "age":24, "name":"Meow-2" }
```

### Compare

The `json::Value` type only supports the same type of comparison (==, !=), when both represent the same json value, they are considered to be the same.

At the same time, `tijson::Value` supports automatic conversion to `bool`, and it is false only when `tijson::Value::type_` is `tijson::Value::TYPE::INVALID`, so it can be used to check json Whether the parsing was successful.

```cpp
auto json_val = tijson::Parse(R"({"Meow-2":"Tijson"})");
if (json_val){
    // parse succeeds
}
if (!json_val){
    // parse fails
}
```

## Prerequisites

Tojson does not depend on third-party libraries, only the standard library and C++17 support are required.

If you want to run the test cases in the `test` folder, you need to have:

- magic_enum
- gtest

This project supports `vcpkg Manifest`, if you want to install the above dependencies automatically, just clone this project and then:

```Cpp
git submodule update --init \
&& ./vcpkg/bootstrap-vcpkg.sh \
&& cmake -B build \
&& cmake --build build \
```

or run `build_test.sh` directly.

```Cpp
./build_test.sh
```

## TODO

- [x] Optimize Get/Set return type
- [x] Provide parsing error codes as error handling methods other than exceptions
- [x] Provide more convenient access (Get/Set) syntactic sugar
- [ ] Added GetIf as an exception-free Get
- [ ] Support C++20 Module
- [ ] Use C++20 std::format to format strings
- [ ] Generator generated format beautification
- [ ] Added nativejson-benchmark test and optimized performance
- [ ] ...

<p align="right"><a href="#readme-top">back to top</a></p>

## 参考

- [JSON library tutorial from scratch](https://github.com/miloyip/json-tutorial)
- [ECMA-404](https://www.ecma-international.org/publications-and-standards/standards/ecma-404/)

<p align="right"><a href="#readme-top">back to top</a></p>

## License

MIT License

<p align="right"><a href="#readme-top">back to top</a></p>

<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->

[contributors-shield]: https://img.shields.io/github/contributors/Meow-2/Tijson.svg?style=for-the-badge
[contributors-url]: https://github.com/Meow-2/Tijson/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/Meow-2/Tijson.svg?style=for-the-badge
[forks-url]: https://github.com/Meow-2/Tijson/network/members
[stars-shield]: https://img.shields.io/github/stars/Meow-2/Tijson.svg?style=for-the-badge
[stars-url]: https://github.com/Meow-2/Tijson/stargazers
[issues-shield]: https://img.shields.io/github/issues/Meow-2/Tijson.svg?style=for-the-badge
[issues-url]: https://github.com/Meow-2/Tijson/issues
[license-shield]: https://img.shields.io/github/license/Meow-2/Tijson.svg?style=for-the-badge
[license-url]: https://github.com/Meow-2/Tijson/blob/master/LICENSE
