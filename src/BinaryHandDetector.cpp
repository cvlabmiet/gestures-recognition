﻿/*
    Реализация функции распознавания руки на бинарном изображении на основе анализа кривизны контура.
*/

#include <algorithm>
#include <optional>

#include <Contour.h>
#include <BinaryHandDetector.hpp>

using namespace std;
using namespace cv;

const uchar ForeGround = 255;
const uchar Background = 0;

// Поиск точек экстремума и их индексов в векторе кривизны.
static pair<vector<float>, vector<size_t>> findExtremums(const vector<float>& curvature)
{
    vector<float> extremums;
    vector<size_t> extremum_indexes;

    const size_t length = curvature.size();
    // Вычисляем первую производную в каждой точке функции кривизны.
    vector<float> derivative(length, 0.0);
    derivative[0] = curvature[1] - curvature[0];
    derivative[length - 1] = curvature[length - 1] - curvature[length - 2];
    for (size_t i = 1; i < length - 1; ++i)
    {
        derivative[i] = (curvature[i + 1] - curvature[i - 1]) / 2;
    }

    // Находим экстремумы с помощью пересечения нуля.
    for (size_t i = 0; i < length - 1; ++i)
    {
        if (derivative[i] * derivative[i + 1] > 0)
            continue;

        extremums.push_back(curvature[i]);
        extremum_indexes.push_back(i);
    }

    return pair(extremums, extremum_indexes);
}

// Поиск индексов максимумов.
static vector<size_t> findMaxIndexes(vector<float>& values, vector<size_t>& indexes)
{
    if (indexes.size() < 9)
        return indexes;

    vector<size_t> max_indexes(9, 0);
    for (size_t i = 0; i < 9; ++i)
    {
        float max = 0.0;
        size_t index = 0;
        for (size_t j = i; j < values.size(); ++j)
        {
            if (values[j] < max)
                continue;

            // Отсеиваем максимумы, обнаруженные ранее
            bool local = false;
            for (size_t k = 0; k < i; ++k)
            {
                int difference = abs((int)max_indexes[k] - (int)indexes[j]);
                if (difference < 50)
                {
                    local = true;
                    break;
                }
            }

            if (local == true)
                continue;

            max = values[j];
            index = j;
        }

        max_indexes[i] = indexes[index];
        std::swap(values[index], values[i]);
        std::swap(indexes[index], indexes[i]);
    }

    std::sort(max_indexes.begin(), max_indexes.end());
    return max_indexes;
}

// Проверка соотношений длин пальцев.
static bool checkFingersLength(const Finger fingers[5])
{
    // Указательный и безымянный пальцы должны быть примерно одной длины.
    double difference = abs(fingers[1].length - fingers[3].length) / fingers[1].length;
    if (difference > 0.1)
        return false;

    // Большой палец короче, чем указательный, средний и безымянный пальцы.
    if (fingers[0].length > fingers[1].length ||
        fingers[0].length > fingers[2].length ||
        fingers[0].length > fingers[3].length)
    {
        return false;
    }

    // Мизинец короче, чем указательный, средний и безымянный пальцы.
    if (fingers[4].length > fingers[1].length ||
        fingers[4].length > fingers[2].length ||
        fingers[4].length > fingers[3].length)
    {
        return false;
    }

    // Указательный и безымянный пальцы короче среднего пальца.
    if (fingers[1].length > fingers[2].length ||
        fingers[3].length > fingers[2].length)
    {
        return false;
    }

    return true;
}

// Поиск координат точек контура с заданными индексами.
static vector<Point2i> getContourPoints(const vector<Point2i>& contour, const vector<size_t>& point_indexes)
{
    size_t size = point_indexes.size();
    vector<Point2i> points(size);

    for (size_t i = 0; i < size; ++i)
    {
        points[i] = contour[point_indexes[i]];
    }

    return points;
}

// Функция на основании анализа кривизны контура вычисляет, является ли контур рукой.
static std::optional<Hand> isHand(const vector<Point2i>& contour, const vector<float>& curvature)
{
    const size_t length = curvature.size();
    if (length < 2)
        throw;

    auto extremums = findExtremums(curvature);
    vector<float> extremum_values = get<0>(extremums);
    vector<size_t> extremum_indexes = get<1>(extremums);
    if (extremum_indexes.size() < 9)
        return {};

    vector<size_t> max = findMaxIndexes(extremum_values, extremum_indexes);
    vector<Point2i> max_points = getContourPoints(contour, max);
    Hand hand(max_points);

    if (!checkFingersLength(hand.getHandFingers()))
        return {};

    return hand;
}

void BinaryHandDetector::trace(InputArray BinaryImage)
{
    Mat image = BinaryImage.getMat();
    vector<Mat> next_pyr;
    buildOpticalFlowPyramid(image, next_pyr, Size(31, 31), 1);

    if (prev_pyr_.empty())
    {
        prev_pyr_ = move(next_pyr);
        return;
    }

    for (auto hand = hands_.begin(); hand != hands_.end(); )
    {
        int status = hand->update(prev_pyr_, next_pyr);
        // Обработка пропадания руки.
        if (status == -1)
        {
            auto to_remove = hand;
            ++hand;
            hands_.erase(to_remove);
            continue;
        }

        ++hand;
    }

    prev_pyr_ = move(next_pyr);
    return;
}

// Сглаживание индексов в векторе.
static void smoothVector(vector<float>& ticks, int nonzero)
{
    dft(ticks, ticks);

    nonzero = nonzero * 2;
    for (size_t i = nonzero + 1; i < ticks.size(); ++i)
    {
        ticks[i] = 0;
    }

    dft (ticks, ticks, DFT_INVERSE + DFT_SCALE);
    return;
}

// Сглаживание контура объекта.
static void smoothContour(vector<Point2i>& contour, int nonzero)
{
    vector<float> ticks(contour.size());
    for (size_t i = 0; i < contour.size(); ++i)
    {
        ticks[i] = contour[i].x;
    }

    smoothVector(ticks, nonzero);
    for (size_t i = 0; i < contour.size(); ++i)
    {
        contour[i].x = ticks[i];
        ticks[i] = contour[i].y;
    }

    smoothVector(ticks, nonzero);
    for (size_t i = 0; i < contour.size(); ++i)
    {
        contour[i].y = ticks[i];
    }

    return;
}

void BinaryHandDetector::detect(InputArray BinaryImage)
{
    Mat image = BinaryImage.getMat();
    updateMask(image.size());

    // Извлечение контуров.
    vector<Contour> contours = extractContours(image, mask_);
    for (size_t i = 0; i < contours.size(); ++i)
    {
        vector<Point2i> contour = contours[i].getContour();
        // Оставляем только 5% низкочастотных дескрипторов
        smoothContour(contour, 0.05 * contours[i].size());
        vector<float> curvature = getCurvature(contour, image.size(), 75);
        // Распознавание руки.
        optional<Hand> hand = isHand(contour, curvature);
        if (hand)
            hands_.push_back(*hand);
    }

    return;
}

const list<Hand>& BinaryHandDetector::getHands() const
{
    return hands_;
}

void BinaryHandDetector::updateMask(Size size)
{
    mask_.create(size, CV_8UC1);
    mask_.setTo(ForeGround);
    for (auto& hand : hands_)
    {
        Rect2i box = hand.getBoundingBox();
        rectangle(mask_, box, Background, FILLED);
    }
}
