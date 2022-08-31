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
    一个用现代cpp编写的易于使用的json解析器/生成器
  </p>
</div>

<!-- [English](https://github.com/Meow-2/Tijson/blob/main/README.md) -->

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li><a href="#关于Tijson">关于Tijson</a></li>
    <li>
      <a href="#快速上手">快速上手</a>
      <ul>
        <li><a href="#解析">解析</a></li>
        <li><a href="#访问">访问</a></li>
      </ul>
    </li>
    <li><a href="#第三方依赖">第三方依赖</a></li>
    <li><a href="#TODO">TODO</a></li>
    <li><a href="#参考">参考</a></li>
    <li><a href="#License">License</a></li>
  </ol>
</details>

<!-- ABOUT THE PROJECT -->

## 关于 Tijson

Tijson 是一个基于 C++17 编写的符合标准的递归下降 Json 解析器/生成器, 跨平台(Windows/Linux/OS X), 跨编译器(MSVC/Gcc/Clang), 仅支持 UTF-8 文本, 简单轻量, 易于使用, 并支持多种错误处理方式

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## 快速上手

Tijson 是一个 header-only 库, 只需要将本仓库中`include`文件夹下的`tijson.h`拷贝到项目目录下作为头文件引入即可使用

```cpp
#include "tijson.h"
```

### 解析

```cpp
// 两种方式
auto json_val0 = tijson::Parse(R"({"Meow-2":"Tijson"})");
auto json_val1 = tijson::Parser::Parse(R"({"Meow-2":"Tijson"})");
// auto 为 tijson::Value, 若解析正确, json_val0, json_val1 完全相同
```

以上两种方式的区别在于方式一在解析失败时, 将错误码写入 Value 中, 程序继续执行, 而方式二直接抛出异常, 程序中止

**错误码用例**

```Cpp
auto json_val = tijson::Parse(R"({"Meow-2":"Tijson"})");
if (json_val){
    std::cout << json_val.Stringify() << '\n';
}else
    std::cout << json_val.GetParseErrorCode() << '\n';
```

**错误码定义**

```
enum class PARSE_ERROR : size_t
{
    NO_ERROR = 0,                   // 没有错误
    EXPECT_VALUE,                   // 缺少Json值
    INVALID_VALUE,                  // 非法的 Null、False、True 或 Number
    ROOT_NOT_SINGULAR,              // 多个Json值
    NUMBER_TOO_BIG,                 // Number: 数字超过了 double 的最大精度
    MISS_QUOTATION_MARK,            // String: 引号缺失
    INVALID_STRING_ESCAPE,          // String: 非法转义
    INVALID_STRING_CHAR,            // String: 非法字符
    INVALID_UNICODE_HEX,            // String: 非法的 Unicode 转义,
                                    // \u后不足四个十六进制数
    INVALID_UNICODE_SURROGATE,      // String: 非法的低代理项
    MISS_COMMA_OR_SQUARE_BRACKET,   // Array: 缺少','或']'
    MISS_KEY,                       // Object: 缺少Key
    MISS_COLON,                     // Object: 缺少':'
    MISS_COMMA_OR_CURLY_BRACKET     // Object: 缺少','或'}'
};

```

**异常用例**

```
try{
    auto json_val = tijson::Parse(R"({"Meow-2":"Tijson"})");
}catch(tijson::ParseException e){
    std::cout<<e.what();
}

```

抛出异常时, 异常携带的信息为错误码对应的字符串, 与错误码含义一致, 与错误码不同的是, 如果不进行捕获, 会终止程序

### 访问

```cpp
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
```

## 第三方依赖

Tijson 不依赖于第三方库, 仅仅只需要标准库和 C++17 支持

如果想要运行`test`文件夹下的测试用例, 需要

- magic_enum
- gtest

本项目支持`vcpkg Manifest`, 若想要自动安装上述依赖, 只需在克隆本项目后

```Cpp
git submodule update --init \
&& ./vcpkg/bootstrap-vcpkg.sh \
&& cmake -B build \
&& cmake --build build \
```

或者直接使用`build_test.sh`

```Cpp
./build_test.sh
```

## TODO

- [x] 优化 Get/Set 返回类型
- [x] 提供解析错误码作为异常之外的错误处理方式
- [x] 提供更加便捷的访问(Get/Set)语法糖
- [ ] 添加 GetIf 作为无异常的 Get
- [ ] 支持 C++20 Module
- [ ] 使用 C++20 std::format 来格式化字符串
- [ ] 生成器生成格式美化
- [ ] 加入 nativejson-benchmark 测试, 并优化性能
- [ ] ...

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## 参考

- [从零开始的 JSON 库教程](https://github.com/miloyip/json-tutorial)
- [ECMA-404](https://www.ecma-international.org/publications-and-standards/standards/ecma-404/)

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## License

MIT License

<p align="right">(<a href="#readme-top">back to top</a>)</p>

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
