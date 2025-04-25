#include <gtest/gtest.h>

#include <sstream>

#include "type_analyser.hpp"

// Helper function to capture std::cout output
std::string captureOutput(std::function<void()> func) {
  std::ostringstream oss;
  std::streambuf* oldCoutBuf = std::cout.rdbuf(oss.rdbuf());
  func();
  std::cout.rdbuf(oldCoutBuf);
  return oss.str();
}

// Test case for analyzeType with an integer
TEST(AnalyzeTypeTest, IntegerTest) {
  int value = 42;
  std::string output = captureOutput([&]() { analyzeType(value); });

  // Check if the output contains expected information
  EXPECT_NE(output.find("数据类型: int"), std::string::npos);
  EXPECT_NE(output.find("字节数: 4"), std::string::npos);
  EXPECT_NE(output.find("内存中的二进制表示"), std::string::npos);
  EXPECT_NE(output.find("值: 42"), std::string::npos);
}
// Test case for analyzeType with an integer
TEST(AnalyzeTypeTest, LiteralTest) {
  std::string output = captureOutput([&]() { analyzeType("Hello world!"); });

  // Check if the output contains expected information
  EXPECT_NE(output.find("数据类型: char [13] [ const ]"), std::string::npos);
  EXPECT_NE(output.find("字节数: 13"), std::string::npos);
  EXPECT_NE(output.find("内存中的二进制表示"), std::string::npos);
  EXPECT_NE(output.find("值: Hello world!"), std::string::npos);
}

// Test case for analyzeType with a pointer
TEST(AnalyzeTypeTest, PointerTest) {
  int value = 42;
  int* ptr = &value;
  std::string output = captureOutput([&]() { analyzeType(ptr); });

  // Check if the output contains expected information
  EXPECT_NE(output.find("数据类型: int*"), std::string::npos);
  EXPECT_NE(output.find("指向的地址 (hex):"), std::string::npos);
  EXPECT_NE(output.find("指向的数据类型: int"), std::string::npos);
  EXPECT_NE(output.find("值: 42"), std::string::npos);
}

// Test case for analyzeType with a nullptr
TEST(AnalyzeTypeTest, NullPointerTest) {
  int* ptr = nullptr;
  std::string output = captureOutput([&]() { analyzeType(ptr); });

  // Check if the output contains expected information
  EXPECT_NE(output.find("数据类型: int*"), std::string::npos);
  EXPECT_NE(output.find("指向的地址 (hex): 0x0"), std::string::npos);
  EXPECT_NE(output.find("指针指向的区域不可访问（野指针？）"),
            std::string::npos);
}

// Test case for analyzeType with a custom struct
struct CustomType {
  int a;
  double b;
};

TEST(AnalyzeTypeTest, CustomTypeTest) {
  CustomType value = {1, 3.14};
  std::string output = captureOutput([&]() { analyzeType(value); });

  // Check if the output contains expected information
  EXPECT_NE(output.find("数据类型: CustomType"), std::string::npos);
  EXPECT_NE(output.find("字节数: 16"), std::string::npos);
  EXPECT_NE(output.find("内存中的二进制表示"), std::string::npos);
}

struct EmptyCustomType {};
TEST(AnalyzeTypeTest, EmptyCustomTypeTest) {
  EmptyCustomType* ptr = new EmptyCustomType;
  std::string output = captureOutput([&]() { analyzeType(ptr); });

  // Check if the output contains expected information
  EXPECT_NE(output.find("数据类型: EmptyCustomType*"), std::string::npos);
  EXPECT_NE(output.find("内存中的二进制表示"), std::string::npos);
  EXPECT_NE(output.find("数据类型: EmptyCustomType"), std::string::npos);
  EXPECT_NE(output.find("字节数: 1"), std::string::npos);
  EXPECT_NE(output.find(" 此类型不可打印"), std::string::npos);
}

// Test case for function pointer
TEST(AnalyzeTypeTest, FunctionPointerTest) {
  using FuncType = int (*)(int, int);
  FuncType func = [](int a, int b) { return a + b; };
  std::string output = captureOutput([&]() { analyzeType(func); });

  EXPECT_NE(output.find("数据类型: int (*)(int, int)"), std::string::npos);
  EXPECT_NE(output.find("数据类型: int (int, int)"), std::string::npos);
}

// Test case for class member pointer
class MyClass {
 public:
  int member;
  static int staticMember;
};
int MyClass::staticMember = 42;

TEST(AnalyzeTypeTest, ClassMemberPointerTest) {
  int MyClass::* memberPtr = &MyClass::member;
  std::string output = captureOutput([&]() { analyzeType(memberPtr); });

  EXPECT_NE(output.find("数据类型: int MyClass::*"), std::string::npos);
  EXPECT_NE(output.find("字节数: 8"), std::string::npos);
  EXPECT_NE(
      output.find("内存中的二进制表示（低地址到高地址）： 00000000 00000000 00000000 00000000"),
      std::string::npos);
}

// Test case for multi-level pointer
TEST(AnalyzeTypeTest, MultiLevelPointerTest) {
  int value = 42;
  int* ptr1 = &value;
  int** ptr2 = &ptr1;
  int*** ptr3 = &ptr2;

  std::string output = captureOutput([&]() { analyzeType(ptr3); });

  EXPECT_NE(output.find("数据类型: int***"), std::string::npos);
  EXPECT_NE(output.find("层级: 3"), std::string::npos);
  EXPECT_NE(output.find("指向的数据类型: int**"), std::string::npos);
}

// Test case for const multi-level pointer
TEST(AnalyzeTypeTest, ConstMultiLevelPointerTest) {
  int value = 42;
  const int* ptr1 = &value;
  const int** ptr2 = &ptr1;
  const int*** ptr3 = &ptr2;

  std::string output = captureOutput([&]() { analyzeType(ptr3); });

  EXPECT_NE(output.find("数据类型: int const***"), std::string::npos);
  EXPECT_NE(output.find("层级: 3"), std::string::npos);
  EXPECT_NE(output.find("指向的数据类型: int const**"), std::string::npos);
}

// Test case for volatile multi-level pointer
TEST(AnalyzeTypeTest, VolatileMultiLevelPointerTest) {
  int value = 42;
  volatile int* ptr1 = &value;
  volatile int** ptr2 = &ptr1;
  volatile int*** ptr3 = &ptr2;

  std::string output = captureOutput([&]() { analyzeType(ptr3); });

  EXPECT_NE(output.find("数据类型: int volatile***"), std::string::npos);
  EXPECT_NE(output.find("层级: 3"), std::string::npos);
  EXPECT_NE(output.find("指向的数据类型: int volatile**"), std::string::npos);
}

// Test case for const volatile multi-level pointer
TEST(AnalyzeTypeTest, ConstVolatileMultiLevelPointerTest) {
  int value = 42;
  const volatile int* ptr1 = &value;
  const volatile int** ptr2 = &ptr1;
  const volatile int*** ptr3 = &ptr2;

  std::string output = captureOutput([&]() { analyzeType(ptr3); });

  EXPECT_NE(output.find("数据类型: int const volatile***"), std::string::npos);
  EXPECT_NE(output.find("层级: 3"), std::string::npos);
  EXPECT_NE(output.find("指向的数据类型: int const volatile**"),
            std::string::npos);
}

// Test case for mixed cv-qualifiers in multi-level pointer
TEST(AnalyzeTypeTest, MixedCvMultiLevelPointerTest) {
  int value = 42;
  const int* ptr1 = &value;
  const volatile int* const* ptr2 = &ptr1;
  const volatile int* const** ptr3 = &ptr2;

  std::string output = captureOutput([&]() { analyzeType(ptr3); });

  EXPECT_NE(output.find("数据类型: int const volatile* const**"),
            std::string::npos);
  EXPECT_NE(output.find("层级: 3"), std::string::npos);
  EXPECT_NE(output.find("指向的数据类型: int const volatile* const*"),
            std::string::npos);
  EXPECT_NE(output.find("数据类型: int const volatile* const*"),
            std::string::npos);
  EXPECT_NE(output.find("层级: 2"), std::string::npos);
  EXPECT_NE(output.find("指向的数据类型: int const volatile* [ const ]"),
            std::string::npos);
  EXPECT_NE(output.find("数据类型: int const volatile* [ const ]"),
            std::string::npos);
  EXPECT_NE(output.find("层级: 1"), std::string::npos);
  EXPECT_NE(output.find("指向的数据类型: int [ const volatile ]"),
            std::string::npos);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}