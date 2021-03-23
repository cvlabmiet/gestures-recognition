/*
    Реализация функции для удаления мелких объектов с бинарного изображения.
*/

#include <algorithm>
#include <vector>

#include <denoise.hpp>

using namespace cv;
using namespace std;

const uchar BackGround = 0;
const uchar ForeGround = 255;

// Маркирует все объекты на бинарном изображении и удаляет
// объекты, которые по площади меньше, чем min_area.
static void markAndClearImage(Mat& srcImage, Mat& dstImage, int min_area);

// Слияние меток двух объектов.
static int mergeObjects(int top, int left, vector<int>& parents);

// Удаляет объекты, площадь которых меньше, чем min_area
// и находит для каждого объекта самого первого родителя в таблице.
static void setLabels(vector<int>& table, vector<int>& square, int min_area);

// Переобозначает объекты на входном изображении и отмечает оставшиеся объекты
// на выходном бинарном изображении.
static void reassignObjects(const vector<int>& table, Mat& marked_image, Mat& binary_image);

void denoise(Mat &image, int min_fg_area, int min_bg_area)
{
    Mat marked_image(image.size(), CV_32S);

    // Удаляем шум "перец"
    if (min_bg_area > 0)
    {
        cv::bitwise_not(image, image);
        image.convertTo(marked_image, CV_32S);
        markAndClearImage(marked_image, image, min_bg_area);
        cv::bitwise_not(image, image);
    }

    // Удаляем шум "соль"
    if (min_fg_area > 0)
    {
        image.convertTo(marked_image, CV_32S);
        markAndClearImage(marked_image, image, min_fg_area);
    }
}

static void markAndClearImage(Mat& marked_image, Mat& dstImage, int min_area)
{
    // Вектор для хранения площадей объектов.
    vector<int> square = { 0 };
    // Вектор для хранения родителей объектов.
    vector<int> parents = { -1 };

    // Маркируем изображение и создаём таблицу со смежными классами.
    int counter = 0;
    for (int y = 0; y < marked_image.rows; ++y)
    {
        for (int x = 0; x < marked_image.cols; ++x)
        {
            // Получаем значение просматриваемого пикселя.
            int& current = marked_image.at<int>(y, x);
            if (current == BackGround)
                continue;

            int left = (x > 0) ? marked_image.at<int>(y, x - 1) : BackGround;
            int top = (y > 0) ? marked_image.at<int>(y - 1, x) : BackGround;

            if ((left == BackGround) && (top == BackGround))
            {
                // Найден новый объект.
                square.push_back(1);
                parents.push_back(-1);
                current = ++counter;
            }
            else if ((left != BackGround) && (top == BackGround))
            {
                current = left;
                ++square[current];
            }
            else if ((left == BackGround) && (top != BackGround))
            {
                current = top;
                ++square[current];
            }
            else if ((left != BackGround) && (top != BackGround))
            {
                current = (left != top) ? mergeObjects(left, top, parents) : top;
                ++square[current];
            }
        }
    }

    setLabels(parents, square, min_area);
    reassignObjects(parents, marked_image, dstImage);
}

static int mergeObjects(int top, int left, vector<int>& parents)
{
    auto [min_label, max_label] = std::minmax(top, left);

    if (parents[max_label] == min_label)
        return min_label;

    if (parents[max_label] == -1)
    {
        // Родителя не было.
        parents[max_label] = min_label;
    }
    else
    {
        // Меняем родительскую метку самому
        // верхнему объекту в иерархии родителей.
        int parent = min_label;
        while (parents[parent] != -1)
            parent = parents[parent];

        int child = max_label;
        while (parents[child] != -1)
            child = parents[child];

        if (child != parent)
        {
            auto [min_label, max_label] = std::minmax(child, parent);
            parents[max_label] = min_label;
        }
    }

    return min_label;
}

static void setLabels(vector<int>& table, vector<int>& square, int min_area)
{
    for (size_t i = 0; i < table.size(); ++i)
    {
        if (table[i] == -1)
            continue;

        // Находим самого первого родителя объекта.
        int parent = table[i];
        while (table[parent] != -1)
            parent = table[parent];

        table[i] = parent;
        square[parent] += square[i];
        square[i] = 0;
    }

    // Если площадь объекта меньше порога, то удаляем его.
    table[0] = BackGround;
    for (size_t i = 0; i < table.size(); ++i)
    {
        if ((square[i] < min_area) && (square[i] != 0))
            table[i] = BackGround;
    }

    // Устанавливаем объекту нужную метку.
    for (size_t i = 0; i < table.size(); ++i)
    {
        if (table[i] == -1)
        {
            table[i] = i;
        }
        else
        {
            int parent = table[i];
            while (parent != table[parent])
                parent = table[parent];

            table[i] = parent;
        }
    }
}

static void reassignObjects(const vector<int>& table, Mat& marked_image, Mat& binary_image)
{
    // Объединяем смежные объекты на маркированом изображении.
    auto reassign_lambda = [&table](int& value, const int[])
    {
        if (value != BackGround)
            value = table[value];
    };
    marked_image.forEach<int>(reassign_lambda);

    // Переносим информацию на бинарное изображение.
    auto fill_lambda = [&marked_image](uchar& value, const int position[])
    {
        int label = marked_image.at<int>(position[0], position[1]);
        value = (label == BackGround) ? BackGround : ForeGround;
    };
    binary_image.forEach<uchar>(fill_lambda);
}
