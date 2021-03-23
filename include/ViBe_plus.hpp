/*
    Алгоритм сегментации движения ViBe+.
*/

#ifndef __VIBE_PLUS_HPP__
#define __VIBE_PLUS_HPP__

#include <algorithm>

#include <opencv2/core.hpp>

#include <ViBe.hpp>
#include <denoise.hpp>

class ViBe_plus : public ViBe
{
public:
    using ViBe_plus::ViBe::ViBe;

    // Функция вычисляет маску сегментации и обновляет модель.
    // TODO: вычислять prob через learningrate.
    void apply(cv::InputArray &image, cv::OutputArray &mask, double) override
    {
        const cv::Mat img = image.getMat();
        mask.create(img.rows, img.cols, CV_8U);
        cv::Mat fg_mask = mask.getMat();

        getSegmentationMask(img, fg_mask);

        cv::Mat update_mask = fg_mask.clone();
        denoise(fg_mask, 11, 21);
        denoise(update_mask, 0, 51);

        updateModel(img, update_mask);
    }

private:
    // Вычисление квадрата градиента в заданной точке изображения.
    static double computeGradientSqr(const cv::Mat& image, int y, int x)
    {
        // Переводим окрестность точки в градации серого.
        double gray[3][3] = { {0} };
        for (int y_i = std::max(0, y - 1); y_i < std::min(image.rows, y + 2); ++y_i)
        {
            for (int x_i = std::max(0, x - 1); x_i < std::min(image.cols, x + 2); ++x_i)
            {
                auto pixel = image.at<Pixel>(y_i, x_i);
                gray[y_i - y + 1][x_i - x + 1] = (0.114 * pixel.x + 0.587 * pixel.y + 0.299 * pixel.z) / 255.0;
            }
        }

        double grad_x = gray[0][2] - gray[0][0];
        grad_x += 2 * (gray[1][2] - gray[1][0]);
        grad_x += gray[2][2] - gray[2][0];

        double grad_y = gray[2][0] - gray[0][0];
        grad_y += 2 * (gray[2][1] - gray[0][1]);
        grad_y += gray[2][2] - gray[0][2];

        double sqr_grad = grad_x * grad_x + grad_y * grad_y;
        // Нормировка квадрата градиента в диапазон от 0 до 1
        return sqr_grad / 32.0;
    }

    void updateModel(const cv::Mat& image, const cv::Mat& update_mask) override
    {
        if (needToInit())
        {
            initialize(image);
            return;
        }

        for (int y = 0; y < image.rows; ++y)
        {
            for (int x = 0; x < image.cols; ++x)
            {
                if (update_mask.at<uchar>(y, x) != BackGround)
                    continue;

                updatePixel(image, y, x);

                // Ограничиваем пространственное распространение.
                size_t result = 0;
                for (int y_i = std::max(0, y - 1); y_i < std::min(image.rows, y + 2); ++y_i)
                    for (int x_i = std::max(0, x - 1); x_i < std::min(image.cols, x + 2); ++x_i)
                        result += update_mask.at<uchar>(y_i, x_i);

                if (result > BackGround)
                {
                    double sqr_grad = computeGradientSqr(image, y, x);
                    const double max_grad = 50.0 / 255.0;
                    if (sqr_grad > max_grad * max_grad)
                        continue;
                }

                updateNeiborPixel(image, y, x);
            }
        }
    }
};

#endif // __VIBE_PLUS_HPP__
