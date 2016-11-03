/*
    ���������� ������� ��� �������� ������ �������� � ��������� �����������.
*/

#include <vector>

#include "deletenoise.h"

using namespace cv;
using namespace std;

const uchar BackGround = 0;
const uchar ForeGround = 255;

// ��������� ��� ������� �� �������� ����������� � �������
// �������, ������� �� ������� ������, ��� max_area.
static void markAndClearImage(Mat& srcImage, Mat& dstImage, int max_area);

// ������� ����� ���� ��������.
static int mergeObjects(int top, int left, vector<int>& parents);

// ������� �������, ������� ������� ������, ��� max_area
// � ������� ��� ������� ������� ������ ������� �������� � �������.
static void setLabels(vector<int>& table, vector<int>& square, int max_area);

// �������������� ������� �� ������� ����������� � �������� ���������� �������
// �� �������� �������� �����������.
static void reassignObjects(vector<int>& table, Mat& marked_image, Mat& binary_image);

// ����������� �������� �����������.
static void inverseImage(Mat& binary_image, Mat& marked_image);

void deleteNoise(Mat &image, Mat &marked_image, int max_area)
{
    marked_image.create(image.rows, image.cols, CV_32S);

    // ������� ��� "�����".
    inverseImage(image, marked_image);
    markAndClearImage(marked_image, image, max_area);

    // ������� ��� "����"
    inverseImage(image, marked_image);
    markAndClearImage(marked_image, image, max_area);
}

static void markAndClearImage(Mat& marked_image, Mat& dstImage, int max_area)
{
    // ������ ��� �������� �������� ��������.
    vector<int> square;
    // ������ ��� �������� ��������� ��������.
    vector<int> parents;

    // ��������� ����������� � ������ ������� �� �������� ��������.
    int counter = 0;
    for (int y = 0; y < marked_image.rows; ++y)
    {
        int* ptr = marked_image.ptr<int>(y);
        for (int x = 0; x < marked_image.cols; ++x)
        {
            // �������� �������� ���������������� �������.
            int current = ptr[x];
            if (current == BackGround)
                continue;

            // �������� �������� ������ �������.
            int left = 0;
            if (x < 1) left = BackGround;
            else left = ptr[x - 1];

            // �������� �������� �������� �������.
            int top = 0;
            if (y < 1) top = BackGround;
            else top = marked_image.ptr<int>(y - 1)[x];

            if ((left == BackGround) && (top == BackGround))
            {
                // ������ ����� ������.
                square.push_back(1);
                parents.push_back(-1);
                ++counter;
                ptr[x] = counter;
                continue;
            }
            if ((left != BackGround) && (top == BackGround))
            {
                ++square[left - 1];
                ptr[x] = left;
                continue;
            }
            if ((left == BackGround) && (top != BackGround))
            {
                ++square[top - 1];
                ptr[x] = top;
                continue;
            }
            if ((left != BackGround) && (top != BackGround))
            {
                if (left != top)
                    top = mergeObjects(left, top, parents);

                ++square[top - 1];
                ptr[x] = top;
                continue;
            }
        }
    }

    setLabels(parents, square, max_area);
    reassignObjects(parents, marked_image, dstImage);
}

static int mergeObjects(int top, int left, vector<int>& parents)
{
    // ������ ������� ����� ����������.
    if (left < top)
        std::swap(top, left);

    if (parents[left - 1] == top - 1)
        return top;

    if (parents[left - 1] == -1)
    {
        // �������� �� ����.
        parents[left - 1] = top - 1;
    }
    else
    {
        // ������ ������������ ����� ������
        // �������� ������� � �������� ���������.
        int parent = top - 1;
        while (parents[parent] != -1)
            parent = parents[parent];

        int child = left - 1;
        while (parents[child] != -1)
            child = parents[child];

        if (child != parent)
        {
            // ������� ����� - parent,
            // ������� ����� - child.
            if (child < parent)
                std::swap(parent, child);

            parents[child] = parent;
        }
    }

    return top;
}

static void setLabels(vector<int>& table, vector<int>& square, int max_area)
{
    for (int i = 0; i < table.size(); ++i)
    {
        if (table[i] == -1) continue;

        // ������� ������ ������� �������� �������.
        int parent = table[i];
        while (table[parent] != -1)
            parent = table[parent];

        table[i] = parent;
        square[parent] += square[i];
        square[i] = 0;
    }

    // ������������� ������� ������ �����.
    for (int i = 0; i < table.size(); ++i)
    {
        if (table[i] == -1)
            table[i] = i + 1;
        else
            ++table[i];
    }

    // ���� ������� ������� ������ ������, �� ������� ���.
    for (int i = 0; i < table.size(); ++i)
    {
        if ((square[i] < max_area) && (square[i] != 0))
        {
            for (int j = 0; j < table.size(); ++j)
            {
                if (table[j] == i + 1)
                    table[j] = BackGround;
            }
        }
    }
}

static void reassignObjects(vector<int>& table, Mat& marked_image, Mat& binary_image)
{
    // ���������� ������� ������� �� ������������ �����������.
    for (int y = 0; y < marked_image.rows; ++y)
    {
        int* ptr = marked_image.ptr<int>(y);
        for (int x = 0; x < marked_image.cols; ++x)
        {
            if (ptr[x] == BackGround)
                continue;

            ptr[x] = table[ptr[x] - 1];
        }
    }

    // ��������� ���������� �� �������� �����������.
    for (int y = 0; y < binary_image.rows; ++y)
    {
        const int* src = marked_image.ptr<int>(y);
        uchar* dst = binary_image.ptr(y);
        for (int x = 0; x < binary_image.cols; ++x)
        {
            if (src[x] == BackGround)
                dst[x] = BackGround;
            else
                dst[x] = ForeGround;
        }
    }
}

static void inverseImage(Mat& binary_image, Mat& marked_image)
{
    for (int y = 0; y < binary_image.rows; ++y)
    {
        const uchar* src = binary_image.ptr(y);
        int* dst = marked_image.ptr<int>(y);
        for (int x = 0; x < binary_image.cols; ++x)
        {
            if (src[x] == BackGround)
                dst[x] = -1;
            else
                dst[x] = BackGround;
        }
    }
}
