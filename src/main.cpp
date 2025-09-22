#include <iostream>

int main() {
    long cpp_version = __cplusplus;
    std::cout << "C++ Language Standard: ";
    if (cpp_version == 199711L) {
        std::cout << "C++98/03" << std::endl;
    } else if (cpp_version == 201103L) {
        std::cout << "C++11" << std::endl;
    } else if (cpp_version == 201402L) {
        std::cout << "C++14" << std::endl;
    } else if (cpp_version == 201703L) {
        std::cout << "C++17" << std::endl;
    } else if (cpp_version == 202002L) {
        std::cout << "C++20" << std::endl;
    } else {
        std::cout << "Unknown or newer standard (" << cpp_version << ")" << std::endl;
    }
    return 0;
}