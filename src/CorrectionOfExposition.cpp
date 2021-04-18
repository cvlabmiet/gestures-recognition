/*
    Реализация функции коррекции яркости.
*/

#include <algorithm>

#include <opencv2/imgproc.hpp>

#include <CorrectionOfExposition.hpp>

namespace
{
const uchar Background = 0;
}

void correctionOfExposition(const cv::Mat& segmentation_mask, const cv::Mat& background_img, cv::Mat& current_image)
{
    cv::Mat background_image = background_img.clone();
    cv::cvtColor(background_image, background_image, cv::COLOR_BGR2YCrCb);
    cv::cvtColor(current_image, current_image, cv::COLOR_BGR2YCrCb);

    // Размер зоны вокруг объектов, зарезервированной под движение.
    // (Ориентировочное максимальное расстояние, на которое могли
    // переместиться объекты между двумя кадрами).
    constexpr int reserved_area = 5;

    // Изображение с отмечеными фоновыми пикселями.
    cv::Mat marked_image;

    // Создраём вокруг объектов область из reserved_area точек,
    // в которой может появиться движение.
    cv::Matx<uchar, 3, 3> kernel = {
        0, 1, 0,
        1, 1, 1,
        0, 1, 0};
    cv::dilate(segmentation_mask, marked_image, kernel, cv::Point(1, 1), reserved_area);

    // Высчитываем суммарную яркость точек фона.
    int counter = 0;
    double current_brightness = 0;
    double background_brightness = 0;

    using Pixel = cv::Point3_<uchar>;
    for (int y = reserved_area; y < marked_image.rows - reserved_area; ++y)
    {
        for (int x = reserved_area; x < marked_image.cols - reserved_area; ++x)
        {
            if (marked_image.at<uchar>(y, x) != Background)
                continue;

            background_brightness += background_image.at<Pixel>(y, x).x;
            current_brightness += current_image.at<Pixel>(y, x).x;
            counter++;
        }
    }

    // Корректируем яркость на изображении.
    if (counter != 0)
    {
        const int diff = (background_brightness - current_brightness) / counter;

        auto correction_lambda = [diff](Pixel& value, const int[])
        {
            value.x = std::clamp(value.x + diff, 0, 255);
        };
        current_image.forEach<Pixel>(correction_lambda);
    }

    cv::cvtColor(current_image, current_image, cv::COLOR_YCrCb2BGR);
}
