﻿/*
    Алгоритм поиска контуров на бинарном изображении.
*/

#ifndef __CONTOUR_H__
#define __CONTOUR_H__

#include <opencv2/highgui.hpp>
#include <opencv2/video/video.hpp>
#include <vector>

class Contour
{
public:
    // Реализация алгоритма прослеживания границы.
    Contour(const cv::Mat& image, const cv::Point2i& point);

    size_t size() const;
    // Возвращает вектор точек контура.
    std::vector<cv::Point2i> getContour() const;
    // Функция рисует контур на заданном изображении.
    void printContour(cv::Mat& image, uchar label) const;

private:
    // Начало контура.
    cv::Point2i start_;
    // Вектор для хранения цепного кода.
    std::vector<int> chain_code_;
};

// Поиск контуров на изображении.
std::vector<Contour> extractContours(cv::InputArray BinImage, cv::InputArray Mask);
// Функция рисует все контуры на изображении.
void printContours(cv::Mat& image, const std::vector<Contour>& contours);
// Функция упорядочивает контуры по убыванию длины.
void sortContours(std::vector<Contour>& contours);
// Функция вычисляет кривизну контура в каждой точке.
std::vector<float> getCurvature(const std::vector<cv::Point2i>& contour, const cv::Size& image_size, const int chord_length);

#endif // __CONTOUR_H__
