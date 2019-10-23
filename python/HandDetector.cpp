#include <vector>
#include <boost/python.hpp>
#include <pyboostcvconverter/pyboostcvconverter.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <Hand.h>
#include <HandDetector.hpp>
#include <exports.hpp>

using namespace boost::python;

class HandDetector_py : public HandDetector
{
public:
    using HandDetector::HandDetector;

    void detect(PyObject* image)
    {
        cv::Mat image_mat;
        image_mat = pbcvt::fromNDArrayToMat(image);
        HandDetector::detect(image_mat);
    }

    std::vector<Hand> getHands() const
    {
        const auto& hands_list = HandDetector::getHands();
        std::vector<Hand> hands_vec(hands_list.begin(), hands_list.end());
        return hands_vec;
    }

    PyObject* getMotionImage()
    {
        const cv::Mat& motion = HandDetector::getMotionImage();
        return pbcvt::fromMatToNDArray(motion);
    }
};

void export_handdetector()
{
    using HandsList = std::vector<Hand>;
    class_<HandsList>("HandsList")
        .def(vector_indexing_suite<HandsList>());

    class_<HandDetector_py>("HandDetector_py", init<int, int>())
        .def("detect", &HandDetector_py::detect)
        .def("getHands", static_cast<std::vector<Hand> (HandDetector_py::*)() const>(&HandDetector_py::getHands))
        .def("getMotionImage", static_cast<PyObject* (HandDetector_py::*)()>(&HandDetector_py::getMotionImage))
        .def("getMotionTime", &HandDetector_py::getMotionTime)
        .def("getExpositionTime", &HandDetector_py::getExpositionTime)
        .def("getTrackerTime", &HandDetector_py::getTrackerTime)
        .def("getDetectorTime", &HandDetector_py::getDetectorTime);
}
