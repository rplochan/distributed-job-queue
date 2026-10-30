#include "../include/hash-utils.h"
#include <iostream>
#include <string>

int main(){
    std::string input = "Hello world!";
    std::string result = sha256(input);

    std::cout << result << "\n";

    return 0;
}
