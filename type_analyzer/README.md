# analyzeType 工具

## 简介

在初学C++的时候, 您是否常为其复杂的类型转换规则而困惑? 您是否好奇过C++中各种类型在内存中的排布究竟如何? 您是否对指针、多级指针的行为不太了解? analyzeType是一个用于帮助初学者学习C++类型系统和指针的工具函数, include"type_analyser.hpp"之后可以直接使用. 这个函数会打印传入的变量/表达式的类型、字节数、内存中的二进制表示等信息. 如果传入的是指针, 则会逐层打印指针信息, 包括指针的类型、指向的类型、指向的地址等.

## Features

- [x] 完整类型
- [x] 不完整类型
- [x] 区分左值、左值引用和右值
- [x] 跨平台的非法地址访问检测
- [x] streamable和sizeofable检测
- [x] 清楚的cv-qualifier标志
- [x] dereferenceable指针检测
- [ ] 更友善的跨平台的匿名类型标志


|| 数据类型 | 数据地址 | 字节数 | 指向的地址 | 指向的数据类型 | 打印值 | 内存中二进制表示 |
|------|----------|----------|--------|------------|----------------|--------|------------------|
|非指针 |     ✅     |      ✅   |    ✅    |     ❌       |        ❌        |    ✅    |      ✅            |
|指针 |     ✅     |      ✅   |    ✅    |     ✅       |        ✅        |    ❌    |      ❌            |

## Usage
可以直接下载头文件`include/type_analyzer.hpp`, 在您的程序里`include"type_analyser.hpp"`即可, 也可以下载本项目玩耍.
```sh
git clone https://github.com/UTF-z/CppTA.git
cd CppTA/type_analyzer
cmake -S . -B build
cmake --build build
./build/main
```

