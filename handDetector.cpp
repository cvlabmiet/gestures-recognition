/*
    ���������� ������� ������������� ����.
*/

#include "handDetector.h"

using namespace std;

int handDetector(vector<float>& curvature, const float min_treshold, const float max_trethold, const int min_counter, const int max_counter)
{
    const size_t length = curvature.size();
    if (length < 2)
        return -1;

    vector<float> derivative;
    derivative.resize(length, 0.0);

    // ��������� ������ ����������� � ������ ����� ������� ��������.
    derivative[0] = curvature[1] - curvature[0];
    derivative[length - 1] = curvature[length - 1] - curvature[length - 2];
    for (int i = 1; i < length - 1; ++i)
    {
        derivative[i] = (curvature[i + 1] - curvature[i - 1]) / 2;
    }

    // ������������ ���������� ��������� ����������.
    int counter = 0; // ������� ��������� ����������.
    float extr = 0; // ���������� �������� � ����� ����������.
    bool search_max = true; // ����, ����������� ������ �������� ��� �������.
    for (int i = 0; i < length - 1; ++i)
    {
        const float multiply = derivative[i] * derivative[i + 1];
        if (multiply < 0)
        {
            const float current = curvature[i];
            if (search_max)
            {
                // ���� ������ ��������� ��������, ������� ��� �������� �����.
                if ((current < extr) && (extr >= max_trethold))
                {
                    search_max = false;
                    ++counter;
                }
                extr = current;
            }
            else
            {
                // ���� ������ ��������� �������, ������� ��� �������� �����.
                if ((current > extr) && (extr < min_treshold))
                    search_max = true;
                extr = current;
            }
        }
    }

    if ((counter >= min_counter) && (counter <= max_counter))
        return 1;

    return 0;
}
