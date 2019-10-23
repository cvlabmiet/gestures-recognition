#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <boost/python.hpp>

#include <exports.hpp>

using namespace boost::python;

void export_opencv_types()
{
    class_<cv::Point2i>("Point2i", init<int, int>())
        .def_readwrite("x", &cv::Point2i::x)
        .def_readwrite("y", &cv::Point2i::y);

    class_<cv::Rect2i>("Rect2i", init<int, int, int, int>())
        .def_readwrite("height", &cv::Rect2i::height)
        .def_readwrite("width", &cv::Rect2i::width)
        .def_readwrite("x", &cv::Rect2i::x)
        .def_readwrite("y", &cv::Rect2i::y);
}
