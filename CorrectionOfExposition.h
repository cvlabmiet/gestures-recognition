/*
    ���������� ������� ��������� �������.
*/

#include <highgui.hpp>

/*
    ������� ������������ ������� �������� ����� ��� ������� �������� �����������.
    ������� ���������:
    SegmentationMask - �������� ����������� � ����������� ����������� ��������� � ����������� �����.
    BackgroundImage - ������� ����������� �������� ����.
    CurrentImage - ������� ���� � ������� Y, ������� �������� ����� �������� � ������������ � ������� ������������.
*/
void correctionOfExposition(const cv::Mat& SegmentationMask, const cv::Mat& BackgroundImage, cv::Mat& CurrentImage);
