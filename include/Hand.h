/*
    Класс руки.
*/

#ifndef __HAND_H__
#define __HAND_H__

#include <vector>
#include <opencv2/core.hpp>

struct Finger
{
    // Точка начала пальца (вблизи ладони).
    cv::Point2i start;
    // Вершина пальца.
    cv::Point2i peak;
    // Длина пальца.
    double length;
    // Палец согнут.
    bool is_bent;
    // Состояние пальца изменено.
    bool status_changed;
};

class Hand
{
public:
    // Создание объекта руки по точкам пальцев.
    Hand(const std::vector<cv::Point2i>& points);
    // Возвращает точку на запястье.
    cv::Point2i getWrist() const;
    // Возвращает массив пальцев руки.
    const Finger* getHandFingers() const;
    // Отрисовка точек пальцев на изображении.
    void print(cv::Mat& image) const;
    // Возвращает прямоугольник, содержащий руку.
    cv::Rect2i getBoundingBox();
    // Обновление модели руки.
    int update(const std::vector<cv::Mat>& prevPyr, const std::vector<cv::Mat>& nextPyr);
    // Оператор сравнения для хранения объектов в std::vector.
    bool operator==(const Hand& other) const
    {
        for (size_t i = 0; i < 5; ++i)
        {
            const auto& first_finger = this->fingers_[i];
            const auto& second_finger = other.fingers_[i];
            if (first_finger.start != second_finger.start || first_finger.peak != second_finger.peak)
                return false;
        }
        return this->midle_point_ == other.midle_point_;
    }

private:
    // Массив пальцев руки.
    Finger fingers_[5];
    // Точка локального максимума кривизы контура между средним и безымянным пальцами.
    cv::Point2i midle_point_;
};

#endif // __HAND_H__
