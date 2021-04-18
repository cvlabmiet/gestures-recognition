/*
    Реализация класса детектора руки на основе анализа кривизны контура.
*/

#include <opencv2/highgui.hpp>

#include <HandDetector.hpp>
#include <CorrectionOfExposition.hpp>

using namespace cv;

void HandDetector::detect(Mat& frame)
{
    //Mat frame = image.getMat();
    // Коррекция яркости.
    motion_.getBackgroundImage(bg_image_);
    if (!bg_image_.empty())
    {
        exposition_timer_.start();
        correctionOfExposition(fgmask_, bg_image_, frame);
        exposition_timer_.stop();
    }

    // Выделение движения.
    motion_timer_.start();
    motion_.apply(frame, fgmask_, 1.0 / 15);
    motion_timer_.stop();

    // Размыкание маски движущихся объектов.
    const uchar kernel_values[25] = { 1, 1, 1, 1, 1,
                                      1, 1, 1, 1, 1,
                                      1, 1, 1, 1, 1,
                                      1, 1, 1, 1, 1,
                                      1, 1, 1, 1, 1 };

    Matx <uchar, 5, 5> kernel_open(kernel_values);
    morphologyEx(fgmask_, fgmask_, MORPH_OPEN, kernel_open);

    tracker_timer_.start();
    hand_detector_.trace(fgmask_);
    tracker_timer_.stop();

    detector_timer_.start();
    hand_detector_.detect(fgmask_);
    detector_timer_.stop();
}
