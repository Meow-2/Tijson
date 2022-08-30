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
    <li>
      <a href="# 关于Tijson">关于Tijson</a>
    </li>
    <li>
      <a href="# 快速上手">快速上手</a>
    </li>
    <li><a href="# 第三方依赖">第三方依赖</a></li>
    <li><a href="# TODO">TODO</a></li>
    <li><a href="# 参考">参考</a></li>
    <li><a href="# License">License</a></li>
  </ol>
</details>

<!-- ABOUT THE PROJECT -->

## 关于 Tijson

Tijson 是一个基于 C++17 编写的符合标准的递归下降 Json 解析器/生成器, 跨平台(Windows/Linux/OS X), 跨编译器(MSVC/Gcc/Clang), 仅支持 UTF-8 文本, 简单轻量, 易于使用, 是我的现代 C++ 练手之作

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## 快速上手

Tijson 是一个 header-only 库, 只需要将本仓库中`include`文件夹下的`tijson.h`拷贝到项目目录下作为头文件引入即可使用

```cpp
#include "tijson.h"
```

### 解析

```cpp
// 两种方式
auto json_val0 = tijson::Parse(R"({"I am key":"I am value"})");
auto json_val1 = tijson::Parser::Parse(R"({"I am key":"I am value"})");
// auto 为 tijson::Value, json_val0, json_val1 完全相同
```

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

- [ ] 优化 Get/Set 返回类型
- [ ] 提供解析错误码作为异常之外的错误处理方式
- [ ] 添加 GetIf 作为无异常的 Get
- [ ] 提供更加便捷的访问(Get/Set)语法糖
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
