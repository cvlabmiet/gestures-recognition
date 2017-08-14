/*
���������� ��������� ������ �������� �� �������� �����������.
*/

#include "Contour.h"

using namespace std;
using namespace cv;

const uchar Background = 0;
const uchar ForeGround = 255;

int getCurvature(vector<float>& curvature, Contour contour, const int chord_length)
{
    size_t length = contour.length();
    if (length < 4)
        return -1;

    vector<Point2i> points;
    contour.getContour(points);

    curvature.resize(length, 0);
    for (int i = 0; i < length; ++i)
    {
        // ��������� ���������� ������ �����.
        const int first_point = i - chord_length + 2;
        const int last_point = i + 1;

        // ������� ����� �� �������.
        for (int shift = 0; shift <= chord_length - 3; ++shift)
        {
            int start_index = first_point + shift;
            if (start_index < 0)
                start_index = 0;
            else if (start_index >= length)
                start_index = (int)length - 1;

            int end_index = last_point + shift;
            if (end_index >= length)
                end_index = (int)length - 1;

            // ��������� ���������� �� ����� �� �����.
            const Point2i chord = points[end_index] - points[start_index];
            const Point2i point_to_chord = points[i] - points[start_index];

            double distance = abs(point_to_chord.x * chord.y -
                                  point_to_chord.y * chord.x);
            distance /= sqrt(chord.x * chord.x + chord.y * chord.y);
            // ������������ ���������� � ����� ��������� ������� � �����.
            if (distance > curvature[i])
                curvature[i] = (float)distance;
        }
    }

    return 0;
}
