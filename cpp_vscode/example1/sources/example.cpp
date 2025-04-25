#include <iostream>
#include <string>
#include <vector>

#include "type_analyser.hpp"

using namespace std;

struct EmptyCustomType {};
class MyClass {
 public:
  int member;
  int member2;
  static int staticMember;
};
int MyClass::staticMember = 42;
int foo(int (*pa)[]) {
  analyzeType(pa);
  return 0;
}
int main() {
  const volatile int i{0};
  cout << i << endl;
  int *pi = nullptr;
  int **ppi = &pi;
  const volatile int **cppi = const_cast<const volatile int **>(ppi);
  *cppi = &i;
  analyzeType(cppi);
  **ppi = 1;
  cout << i;
  return 0;
}
