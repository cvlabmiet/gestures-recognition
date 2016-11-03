/*
    ���������� ������� ��������� �������.
*/

#include <imgproc.hpp>
#include <math.h>

#include "CorrectionOfExposition.h"

using namespace cv;

const uchar Background = 0;

void correctionOfExposition(const Mat& segmentationMask,
                            const Mat& backgroundImage,
                            Mat& currentImage)
{
    Mat background_image = backgroundImage.clone();
    cvtColor(background_image, background_image, COLOR_BGR2YCrCb);
    cvtColor(currentImage, currentImage, COLOR_BGR2YCrCb);

    // ������ ���� ������ ��������, ����������������� ��� ��������.
    // (��������������� ����������, �� ������� ����� �������������
    // ������� ����� ����� �������).
    const int reserved_area = 5;
    // ����������� � ���������� �������� ���������.
    Mat markedImage;

    // ������� ������ �������� ������� �� reserved_area �����,
    // � ������� ����� ��������� ��������.
    Matx<uchar, 3, 3> kernel = {
        0, 1, 0,
        1, 1, 1,
        0, 1, 0};
    dilate(segmentationMask, markedImage, kernel, Point(1, 1), reserved_area);

    // ����������� ��������� ������� ����� ����.
    double background_light = 0; // ���������� ��� �������� ��������� ������� ����.
    double current_light = 0; // ���������� ��� �������� ��������� ������� �������� �����.
    int counter = 0; // ���������� ��� �������� ���������� ����� �� ������� ��������� �����.
    for (int y = reserved_area; y < markedImage.rows - reserved_area; ++y)
    {
        const uchar* markedImage_ptr = markedImage.ptr(y);
        const uchar* background_image_ptr = background_image.ptr(y);
        const uchar* currentImage_ptr = currentImage.ptr(y);
        for (int x = reserved_area; x < markedImage.cols - reserved_area; ++x)
        {
            if (markedImage_ptr[x] != Background)
                continue;
            background_light += background_image_ptr[3 * x];
            current_light += currentImage_ptr[3 * x];
            counter++;
        }
    }

    // ��������� ������� �������� ��������� �������.
    if (counter != 0)
    {
        const double diff = (double)(background_light - current_light) / counter;

        //������������ ������� �� �����������.
        for (int y = 0; y < currentImage.rows; ++y)
        {
            uchar* currentImage_ptr = currentImage.ptr(y);
            for (int x = 0; x < currentImage.cols; ++x)
            {
                currentImage_ptr[3 * x] = (uchar)min(max((currentImage_ptr[3 * x] + diff), 0.0), 255.0);
            }
        }
    }

    cvtColor(currentImage, currentImage, COLOR_YCrCb2BGR);
}
