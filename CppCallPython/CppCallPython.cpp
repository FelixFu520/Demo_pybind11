#include <iostream>

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cout << "Usage: <exe> <stringToPrint>\n";
        return 1;
    }

    std::string s(argv[1]);

    std::cout << "On C++ side, string is: " << s << "\n";


    py::scoped_interpreter python;

    auto tests = py::module::import("tests");

    // The following is the equivalent in Python of calling
    // "tests.test1.printStringAndReturnSqrt2(s)"
    double result = tests.attr("test1").attr("printStringAndReturnSqrt2")(s).cast<double>();
    std::cout << "On C++ side, the function result is: " << result << "\n";



    return 0;
}