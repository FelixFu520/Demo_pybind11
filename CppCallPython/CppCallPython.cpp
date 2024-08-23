#include <iostream>
#include <filesystem>
#include <Windows.h>
#include <opencv2/opencv.hpp>
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include "CppCallPython.h"
namespace py = pybind11;

std::filesystem::path GetExeDirectory() {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    std::filesystem::path exePath(path);
    return exePath.parent_path();
}

PYBIND11_MODULE(example, m) {
    m.def("get_mat", []() -> py::object {
        // ����һ��ʾ�� Mat ����
        std::filesystem::path exe_path = GetExeDirectory();
        cv::Mat image = cv::imread((exe_path / "xz.jpg").string());

        // �� Mat ת��Ϊ numpy ����
        py::object np = py::module::import("numpy");
        py::object np_array = np.attr("array")(image);

        return np_array;
        });

    // ����һ��ת���������ڽ� cv::Mat ת��Ϊ numpy ����
    py::implicitly_convertible<cv::Mat, py::object>();
}

int inferByPython()
{

    std::cout << "On C++ side " << "\n";

    py::scoped_interpreter python;

    auto tests = py::module::import("tests");

    // The following is the equivalent in Python of calling
    // "tests.test1.printStringAndReturnSqrt2(s)"
    double result = tests.attr("test1").attr("printStringAndReturnSqrt2")("hello world").cast<double>();
    std::cout << "On C++ side, the function result is: " << result << "\n";

    // The following is the equivalent in Python of calling
    // "tests.test2.inferByPython(image)"
	//int result2 = tests.attr("test2").attr("inferByPython")().cast<int>();

    return 0;
}