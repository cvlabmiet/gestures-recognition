#define PY_ARRAY_UNIQUE_SYMBOL pbcvt_ARRAY_API

#include <vector>
#include <boost/python.hpp>

#include <Hand.h>
#include <exports.hpp>

using namespace boost::python;

void export_finger()
{
    class_<Finger>("Finger")
        .def_readwrite("start", &Finger::start)
        .def_readwrite("peak", &Finger::peak)
        .def_readwrite("length", &Finger::length)
        .def_readwrite("is_bent", &Finger::is_bent)
        .def_readwrite("status_changed", &Finger::status_changed);
}

void export_hand()
{
    class_<Hand>("Hand", init<const std::vector<cv::Point2i>&>())
        .def("getWrist", &Hand::getWrist)
        .def("getHandFingers", &Hand::getHandFingers, return_value_policy<manage_new_object>())
        .def("print", &Hand::print)
        .def("getBoundingBox", &Hand::getBoundingBox)
        .def("update", &Hand::update);
}
