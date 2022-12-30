#include "read_input_functions.h"
#include <iostream>

string ReadLine() {
    string s;
    std::getline(std::cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result;
    std::cin >> result;
    ReadLine();
    return result;
}