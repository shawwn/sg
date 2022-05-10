#include <iostream>

#include "sg/sg.hpp"

int main() {
  std::cout << std::boolalpha;
  sg::testCoroutine();
  //  std::cout << "Hello, World!" << std::endl;
  sg::test_sg();
  return 0;
}
