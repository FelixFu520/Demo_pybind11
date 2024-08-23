#include <Windows.h>
#include <iostream>
#include <filesystem>
#include <vector>
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

struct BBox {
    float x;
    float y;
    float width;
    float height;
    float conf;
    float label;
};

PYBIND11_MODULE(example, m) {
    py::class_<BBox>(m, "BBox")
        .def(py::init<>())
        .def_readwrite("x", &BBox::x)
        .def_readwrite("y", &BBox::y)
        .def_readwrite("width", &BBox::width)
        .def_readwrite("height", &BBox::height)
        .def_readwrite("conf", &BBox::conf)
        .def_readwrite("label", &BBox::label);

    m.def("call_python_function", [](const py::object& python_function, cv::Mat& mat) -> std::vector<BBox> {
        py::object np = py::module::import("numpy");
        py::object np_array = np.attr("array")(mat);

        // 调用Python函数
        py::object result = python_function(np_array);

        // 确保返回值是一个列表
        if (!result.is(py::module::import("numpy").attr("ndarray"))) {
            throw std::runtime_error("Python function did not return a numpy array.");
        }

        // 将numpy数组转换为BBox列表
        auto result_list = result.cast<py::list>();
        std::vector<BBox> bbox_vector;
        for (auto& item : result_list) {
            if (!item.is(py::module::import("numpy").attr("ndarray"))) {
                throw std::runtime_error("List item is not a numpy array.");
            }
            auto bbox_array = item.cast<py::array_t<float>>();
            if (bbox_array.size() != 6) {
                throw std::runtime_error("Numpy array does not have exactly 6 elements.");
            }
            BBox bbox;
            bbox.x = bbox_array.at(0);
            bbox.y = bbox_array.at(1);
            bbox.width = bbox_array.at(2);
            bbox.height = bbox_array.at(3);
            bbox.conf = bbox_array.at(4);
            bbox.label = bbox_array.at(5);
            bbox_vector.push_back(bbox);
        }

        return bbox_vector;
        });
}

int inferByPython()
{
	auto exePath = GetExeDirectory();
    std::cout << "On C++ side " << "\n";
    
    // 加载Python环境和函数
    pybind11::scoped_interpreter guard{}; // 启动Python解释器
    //py::scoped_interpreter python;

    auto tests = py::module::import("tests");

    // The following is the equivalent in Python of calling
    // "tests.test1.printStringAndReturnSqrt2(s)"
    double result = tests.attr("test1").attr("printStringAndReturnSqrt2")("hello world").cast<double>();
    std::cout << "On C++ side, the function result is: " << result << "\n";



    // The following is the equivalent in Python of calling
    // "tests.test2.inferByPython(image)"
	cv::Mat image = cv::imread((exePath / "xz.jpg").string());
    pybind11::module_ example = pybind11::module_::import("example");
    // 添加Python模块路径
    std::string tests_path = "tests"; // 替换为实际路径
    pybind11::module_ sys = pybind11::module_::import("sys");
    sys.attr("path").cast<pybind11::list>().append(tests_path);
    // 导入Python函数
    pybind11::object test2 = pybind11::module_::import("tests.test2");
    pybind11::object python_function = test2.attr("inferByPython");
    // 调用Python函数
    pybind11::object result_obj = example.attr("call_python_function")(python_function, image);
    std::vector<BBox> result2 = result_obj.cast<std::vector<BBox>>();
    // 打印结果
    for (const auto& bbox : result2) {
        std::cout << "BBox: [" << bbox.x << ", " << bbox.y << ", " << bbox.width << ", "
            << bbox.height << ", " << bbox.conf << ", " << bbox.label << "]" << std::endl;
    }
    return 0;
}