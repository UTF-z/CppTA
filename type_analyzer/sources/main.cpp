#include <iostream>
#include <string>
#include <vector>

#include "type_analyzer.hpp"

using namespace std;

int main() {
  int *ptr = nullptr;
  analyzeType(ptr);
  return 0;
}
