/*
    ���������� ������� ��������� �������.
*/

#include <highgui.hpp>

/*
    ������� ������������ ������� �������� ����� ��� ������� �������� �����������.

    ������� ���������:
    SegmentationMask - �������� ����������� � ����������� ����������� ���������
                       � ����������� �����.
    BackgroundImage  - ������� ����������� �������� ����.
    CurrentImage     - ������� ���� � ������� Y, ������� �������� �����
                       �������� � ������������ � ������� ������������.
*/
void correctionOfExposition(const cv::Mat& segmentationMask,
                            const cv::Mat& backgroundImage,
                            cv::Mat& currentImage);
