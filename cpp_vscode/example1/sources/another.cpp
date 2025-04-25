#include <iostream>
template <class T>
void another(T t) {
  std::cout << "another: " << t << std::endl;
}

template void another<float>(float);