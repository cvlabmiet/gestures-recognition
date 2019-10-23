#define PY_ARRAY_UNIQUE_SYMBOL pbcvt_ARRAY_API

#include <boost/python.hpp>
#include <pyboostcvconverter/pyboostcvconverter.hpp>

#include <exports.hpp>

using namespace boost::python;
using namespace pbcvt;

// Функция для инициализации массивов из pbcvt.
#if (PY_VERSION_HEX >= 0x03000000)
static void* init_ar()
{
#else
static void init_ar()
{
#endif
    Py_Initialize();

    import_array();
    return NUMPY_IMPORT_ARRAY_RETVAL;
}

BOOST_PYTHON_MODULE(HandMouse_py)
{
    init_ar();
    // Подключение ковертеров между классами cv::Mat и np::array.
    to_python_converter<cv::Mat, pbcvt::matToNDArrayBoostConverter>();
    pbcvt::matFromNDArrayBoostConverter();

    export_opencv_types();
    export_finger();
    export_hand();
    export_handdetector();
}
