#include <cstdlib>
#include <iostream>
#include <limits>

#include "../tool/LOCALSPACE/localspace.hpp"
#include "../tool/PASS/ghostkey.hpp"
#include "../tool/WALLET/hidewallet.hpp"
#include "./visual/banner.hpp"

void clear_console() {
#ifdef _WIN32
  std::system("cls");
#else
  std::system("clear");
#endif
}

void menu();

void wait_and_return_to_menu() {
  char num = 1;
  std::cout << std::endl << "  IN 0 TO NEXT: ";
  std::cin >> num;
  menu();
}

void menu() {
  clear_console();
  main_banner();
  menu_banner();

  int menu_item = 0;
  std::cout << "> ";
  std::cin >> menu_item;

  if (std::cin.fail()) {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "  ERROR: IN NUM" << std::endl;
    wait_and_return_to_menu();
    return;
  }

  switch (menu_item) {
  case 1: {
    ghostkey();
    wait_and_return_to_menu();
    return;
  }
  case 2: {
    hidewallet();
    wait_and_return_to_menu();
    return;
  }
  case 3: {
    localspace();
    wait_and_return_to_menu();
    return;
  }
  case 0: {
    clear_console();
    main_banner();
    out_P();
    wait_and_return_to_menu();
    return;
  }
  case 9: {
    clear_console();
    main_banner();
    out_W();
    wait_and_return_to_menu();
    return;
  }
  case 90: {
    clear_console();
    main_banner();
    out_P();
    out_W();
    wait_and_return_to_menu();
    return;
  }
  }
}
