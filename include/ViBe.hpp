/*
    Алгоритм сегментации движения ViBe.
*/

#ifndef __VIBE_HPP__
#define __VIBE_HPP__

#include <opencv2/core.hpp>
#include <opencv2/video.hpp>

class ViBe : public cv::BackgroundSubtractor
{
public:
    ViBe() : initialized_(false)
    {
    }

    ViBe(size_t history_depth, size_t radius, size_t min_overlap, size_t probability) :
    history_depth_(history_depth), sqr_rad_(radius * radius), min_overlap_(min_overlap),
    probability_(probability), initialized_(false)
    {
    }

    // Функция вычисляет маску сегментации и обновляет модель.
    // TODO: вычислять prob через learningrate.
    void apply(cv::InputArray image, cv::OutputArray mask, double) override
    {
        const cv::Mat img = image.getMat();
        mask.create(img.rows, img.cols, CV_8U);
        cv::Mat fg_mask = mask.getMat();

        getSegmentationMask(img, fg_mask);
        updateModel(img, fg_mask);
    }

    // Функция вычисляет изображение фона.
    void getBackgroundImage(cv::OutputArray backgroundImage) const override
    {
        bg_mat_.copyTo(backgroundImage);
    }

protected:
    using Pixel = cv::Point3_<uchar>;
    const uchar BackGround = 0;
    const uchar ForeGround = 255;

    // Возвращаемое значение равно true, если необходима инициализация
    // модели фона, и false, иначе.
    bool needToInit() const
    {
        return !initialized_;
    }

    // Функция инизиализации модели.
    void initialize(const cv::Mat& image)
    {
        // Заполняем первое значение модели значением текущего пикселя.
        // Остальные значения модели заполняем значениями соседних пикселей.
        samples_.create({image.rows, image.cols, static_cast<int>(history_depth_)}, image.type());
        auto init_samples = [&image, this](Pixel& pixel, const int position[])
        {
            cv::Point2i point = {position[1], position[0]};
            if (position[2] != 0)
                point = getRandomNeiborPixel(point);

            pixel = image.at<Pixel>(point);
        };
        samples_.forEach<Pixel>(init_samples);

        // Значение фона равно значению текущего пикселя.
        image.copyTo(bg_mat_);
        initialized_ = true;
        return;
    }

    // Функция классификации точек изображения.
    void getSegmentationMask(const cv::Mat& image, cv::Mat& fg_mask)
    {
        fg_mask.setTo(ForeGround);
        if (needToInit())
            return;

        auto fill_fg_mask = [&image, this](uchar& mask_val, const int position[])
        {
            // Находим количество пересечений текущего значения пикселя с моделью.
            size_t counter = 0;
            Pixel pixel = image.at<Pixel>(position[0], position[1]);
            for (size_t i = 0; i < history_depth_; ++i)
            {
                Pixel model_pixel = samples_.at<Pixel>(position[0], position[1], i);
                auto diff = cv::Point3i(pixel) - cv::Point3i(model_pixel);
                double dist = diff.ddot(diff);
                if (dist < sqr_rad_)
                {
                    ++counter;
                    if (counter == min_overlap_)
                    {
                        mask_val = BackGround;
                        break;
                    }
                }
            }
        };

        fg_mask.forEach<uchar>(fill_fg_mask);
    }

    // Обновление модели фона в заданной точке.
    void updatePixel(const cv::Mat& image, int y, int x)
    {
        Pixel img_pixel = image.at<Pixel>(y, x);
        int rand_number = generator_.uniform(0, probability_);
        if (rand_number == 0)
        {
            rand_number = generator_.uniform(0, history_depth_);
            samples_.at<Pixel>(y, x, rand_number) = img_pixel;
            bg_mat_.at<Pixel>(y, x) = img_pixel;
        }
    }

    // Обновление модели фона случайного соседа из восьмисвязной области заданной точки.
    void updateNeiborPixel(const cv::Mat& image, int y, int x)
    {
        // Обновление модели случайного соседа из восьмисвязной области.
        int rand_number = generator_.uniform(0, probability_);
        if (rand_number == 0)
        {
            cv::Point2i neib_pixel = getRandomNeiborPixel(cv::Point2i(x, y));
            rand_number = generator_.uniform(0, history_depth_);
            Pixel img_pixel = image.at<Pixel>(y, x);
            samples_.at<Pixel>(neib_pixel.y, neib_pixel.x, rand_number) = img_pixel;
        }
    }

private:
    // Функция выдаёт случайную точку из восьмисвязной области.
    cv::Point2i getRandomNeiborPixel(const cv::Point2i& pixel)
    {
        auto generate_rand_coord = [&generator = generator_](int x, int max_val)
        {
            return generator.uniform(std::max(x - 1, 0), std::min(x + 2, max_val));
        };

        while (true)
        {
            cv::Point2i neib_pixel = {
                generate_rand_coord(pixel.x, samples_.size[1]),
                generate_rand_coord(pixel.y, samples_.size[0])
            };

            if (neib_pixel != pixel)
                return neib_pixel;
        }
    }

    // Функция обновления модели алгоритма.
    void updateModel(const cv::Mat& image, const cv::Mat& update_mask)
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
                updateNeiborPixel(image, y, x);
            }
        }
    }

    size_t history_depth_; // Количество хранимых значений для каждого пикселя.
    size_t sqr_rad_; // Квадрат максимального расстояния для включения точки в модель.
    size_t min_overlap_; // Минимальное количество совпадений значения пикселя с моделью.
    size_t probability_; // Вероятность обновления модели.
    bool initialized_; // Флаг инициализации модели.
    cv::Mat samples_; // Матрица для хранения значений пикселей.
    cv::Mat bg_mat_; // Матрица для хранения фона.
    cv::RNG generator_; // Генератор случайных чисел (используется равномерный закон распределения).
};

#endif // __VIBE_HPP__
