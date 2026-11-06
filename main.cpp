#include "./core/menu.hpp"

#include <iostream>
#include <unistd.h>

int main() {

  if (geteuid() != 0) {
    std::cerr << "-------------" << std::endl;
    std::cerr << "! ROOT ONLY !" << std::endl;
    std::cerr << "-------------" << std::endl;
    return 1;
  }
  menu();

  return 0;
}
