﻿/*
    Функция для удаления мелких объектов с бинарного изображения.
*/

#include <core.hpp>

// Функция удаляет объекты, меньшие по площади, чем min_fg_area
// и "дырки" в объектах, меньшие по площади, чем min_bg_area.
void deleteNoise(cv::Mat &image, int min_fg_area, int min_bg_area);
