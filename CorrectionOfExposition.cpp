/*
    ���������� ������� ��������� �������.
*/

#include "CorrectionOfExposition.h"

#include <imgproc.hpp>
#include <math.h>

using namespace cv;

const uchar Background = 0;

void correctionOfExposition(const Mat& SegmentationMask, const Mat& BackgroundImage, Mat& CurrentImage)
{
    Mat background_image = BackgroundImage.clone();
    cvtColor(background_image, background_image, COLOR_BGR2YCrCb);
    cvtColor(CurrentImage, CurrentImage, COLOR_BGR2YCrCb);

    // ������ ���� ������ ��������, ����������������� ��� ��������.
    // (��������������� ����������, �� ������� ����� �������������
    // ������� ����� ����� �������).
    const int reserved_motion_distance = 5;
    // ����������� � ���������� �������� ���������.
    Mat markedImage;

    // ������� ������ �������� ������� �� reserved_motion_distance �����,
    // � ������� ����� ��������� ��������.
    Matx<uchar, 3, 3> kernel = {
        0, 1, 0,
        1, 1, 1,
        0, 1, 0};
    dilate(SegmentationMask, markedImage, kernel, Point(-1, -1), reserved_motion_distance);

    // ����������� ��������� ������� ����� ����.
    double background_light = 0; // ���������� ��� �������� ��������� ������� ����.
    double current_light = 0; // ���������� ��� �������� ��������� ������� �������� �����.
    int counter = 0; // ���������� ��� �������� ���������� ����� �� ������� ��������� �����.
    for (int y = reserved_motion_distance; y < markedImage.rows - reserved_motion_distance; ++y)
    {
        const uchar* ptr_markedImage = markedImage.ptr(y);
        const uchar* ptr_background_image = background_image.ptr(y);
        const uchar* ptr_CurrentImage = CurrentImage.ptr(y);
        for (int x = reserved_motion_distance; x < markedImage.cols - reserved_motion_distance; ++x)
        {
            if (ptr_markedImage[x] != Background) 
                continue;
            background_light += ptr_background_image[3 * x];
            current_light += ptr_CurrentImage[3 * x];
            counter++;
        }
    }

    // ��������� ������� �������� ��������� �������.
    if (counter != 0)
    {
        double diff = (double)(background_light - current_light) / counter;

        //������������ ������� �� �����������.
        for (int y = 0; y < CurrentImage.rows; ++y)
        {
            uchar* ptr_CurrentImage = CurrentImage.ptr(y);
            for (int x = 0; x < CurrentImage.cols; ++x)
            {
                ptr_CurrentImage[3 * x] = (uchar)min(max((ptr_CurrentImage[3 * x] + diff), 0.0), 255.0);
            }
        }
    }

    cvtColor(CurrentImage, CurrentImage, COLOR_YCrCb2BGR);
}
