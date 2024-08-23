#include <iostream>

#include "CppCallPython.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: <exe> <ImagePath,eg. D:/test.jpg>\n";
        return 1;
    }

    std::string s(argv[1]);

    if (inferByPython(s) != 0) {
		std::cout << "Error in inferByPython\n";
        return 1;
    }
	return 0;
}