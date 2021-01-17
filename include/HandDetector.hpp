/*
    Класс детектора руки на основе анализа кривизны контура.
*/

#ifndef __HANDDETECTOR_HPP__
#define __HANDDETECTOR_HPP__

#include <list>
#include <opencv2/core.hpp>

#include <ViBe_plus.hpp>
#include <BinaryHandDetector.hpp>
#include <Hand.h>
#include <Timer.h>

class HandDetector
{
public:
    HandDetector(int rows, int cols) :
        motion_(20, 20, 2, 15),
        bg_image_(rows, cols, CV_8UC3),
        fgmask_(rows, cols, CV_8UC1)
    {
    }

    // Обнаружение новых рук на изображении.
    void detect(cv::Mat& image);

    // Возвращает список обнаруженных рук.
    const std::list<Hand>& getHands() const
    {
        return hand_detector_.getHands();
    }

    // Возвращает ссылку на маску сегментации движения.
    const cv::Mat& getMotionImage() const
    {
        return fgmask_;
    }

    // Возвращает текущее значение времени, затраченного на выделение движения.
    double getMotionTime() const
    {
        return motion_timer_.getTime();
    }

    // Возвращает текущее значение времени, затраченного на коррекцию экспозиции.
    double getExpositionTime() const
    {
        return exposition_timer_.getTime();
    }

    // Возвращает текущее значение времени, затраченного на трекинг рук.
    double getTrackerTime() const
    {
        return tracker_timer_.getTime();
    }

    // Возвращает текущее значение времени, затраченного на обнаружение рук.
    double getDetectorTime() const
    {
        return detector_timer_.getTime();
    }

private:
    // Объект для выделения движения.
    ViBe_plus motion_;
    // Детектор руки на бинарном изображении.
    BinaryHandDetector hand_detector_;

    // Изображение текущего фона.
    cv::Mat bg_image_;
    // Текущая маска сегментации движения.
    cv::Mat fgmask_;

    // Таймер для сегментации движения.
    Timer motion_timer_;
    // Таймер для коррекции экспозиции.
    Timer exposition_timer_;
    // Таймер для трекинга рук.
    Timer tracker_timer_;
    // Таймер для обнаружения рук.
    Timer detector_timer_;
};

#endif // __HANDDETECTOR_HPP__
