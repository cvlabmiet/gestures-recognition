/*
    Функция для удаления мелких объектов с бинарного изображения.
*/

#ifndef __DENOISE_HPP__
#define __DENOISE_HPP__

#include <opencv2/core.hpp>

// Функция удаляет объекты, меньшие по площади, чем min_fg_area
// и "дырки" в объектах, меньшие по площади, чем min_bg_area.
void denoise(cv::Mat &image, int min_fg_area, int min_bg_area);

#endif // __DENOISE_HPP__
