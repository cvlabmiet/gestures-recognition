/*
    ������ ����������������� ��������������� ������ �����������������������.
*/

#include <highgui.hpp>

class VideoSequenceCapture
{
public:
    // � �������� ��������� ��������� ���� � ������� ����� �����������������������.
    VideoSequenceCapture(const cv::String& filename);

    // ���������� ��������� ���� �����������������������.
    void operator >> (cv::Mat& image);
private:
    cv::String directory; // ��� ����������, � ������� ���������� �����������������������.
    cv::String name; // ��� �����������������������.
    cv::String extension; // ���������� ������ �����������������������.
    int counter; // ������� �������� �����.
};
