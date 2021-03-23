/*
    Дополнительный функционал для отладки.
*/

#ifndef __DEBUG_HPP__
#define __DEBUG_HPP__

#include <opencv2/core.hpp>

#define ___DEBUG___ 1

#if ___DEBUG___
#include <string>
#include <filesystem>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

extern size_t debug_frame_counter;
#endif // ___DEBUG___

// В режиме отладки: запись изображения в каталог отладки.
template<class String>
void imageWrite(const String& win_name, const cv::Mat& mat)
{
#if ___DEBUG___
    namespace fs = std::filesystem;

    if (win_name == std::string("Input"))
        ++debug_frame_counter;

    const fs::path debug_dir = "../result/";
    fs::create_directory(debug_dir);
    fs::path image_dir = debug_dir / win_name;
    fs::create_directory(image_dir);
    std::string file_name = std::to_string(debug_frame_counter) + ".png";
    fs::path image_path = image_dir / file_name;
    cv::imwrite(image_path, mat);
#endif // ___DEBUG___
}

// В режиме отладки: запись изображения в каталог отладки.
// В обычном режиме: вывод изображения на экран.
template<class String>
void imageShow(const String& win_name, const cv::Mat& image)
{
    imshow(win_name, image);

#if ___DEBUG___
    if (image.channels() == 1)
    {
        cv::Mat bin_image = image.clone();
        const uchar BackGround = 0;
        const uchar ForeGround = 255;
        cv::threshold(image, bin_image, BackGround, ForeGround, cv::THRESH_BINARY);
        imageWrite(win_name, bin_image);
    }
    else
    {
        imageWrite(win_name, image);
    }
#endif // ___DEBUG___
}

#endif // __DEBUG_HPP__
