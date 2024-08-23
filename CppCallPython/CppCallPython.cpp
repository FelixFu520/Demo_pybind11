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

py::array_t<unsigned char> matToNumpy_Color(cv::Mat& img) {
    return py::array_t<unsigned char>({ img.rows,img.cols,3 }, img.data);
}

py::array_t<unsigned char> matToNumpy_Gray(cv::Mat& img) {
    return py::array_t<unsigned char>({ img.rows,img.cols }, img.data);
}

int inferByPython(const std::string& path)
{
    // 加载Python环境和函数
    pybind11::scoped_interpreter guard{}; // 启动Python解释器

    auto tests = py::module::import("tests");

    // The following is the equivalent in Python of calling
    // "tests.test1.printStringAndReturnSqrt2(s)"
    std::cout << "测试程序1: " << std::endl;
    double result = tests.attr("test1").attr("printStringAndReturnSqrt2")("hello world").cast<double>();
    std::cout << "On C++ side, the function result is: " << result << "\n";


    // The following is the equivalent in Python of calling
    // "tests.test2.inferByPython(image)"
    std::cout << "测试程序2: " << std::endl;
	if (!std::filesystem::exists(path)) {
		std::cout << "Image not found: " << path << std::endl;
		return 1;
	}
    cv::Mat image = cv::imread(path);
    pybind11::array_t<unsigned char> image_np;
    if (image.channels() == 1) {
		image_np = matToNumpy_Gray(image);
    }
    else if (image.channels() == 3) {
        image_np =  matToNumpy_Color(image);
    }
    else {
		std::cout << "Unsupported number of channels in image" << std::endl;
        return 1;
    }
    auto result2 = tests.attr("test2").attr("inferByPython")(image_np);
    auto result_list = result2.cast<py::list>();
    std::vector<BBox> bbox_vector;
    for (auto& item : result_list) {
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
		std::cout << "result, x: " << bbox.x << " y: " << bbox.y << " width: " << bbox.width << " height: " << bbox.height << " conf: " << bbox.conf << " label: " << bbox.label << std::endl;
    }

    return 0;
}