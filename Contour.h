/*
    �������� ������ �������� �� �������� �����������.
*/

#include <highgui.hpp>
#include <video\video.hpp>
#include <vector>

struct Contour
{
    // ������ �������.
    cv::Point2i start;
    // ������ ��� �������� ������� ����.
    std::vector<int> chain_code;
};

class ContourMap
{
public:
    // ������� ���������� ���������� �������� ��������.
    size_t getNumberOfContours() const;
    // ������� ������ ������ �� �������� �����������.
    void print�ontour(cv::Mat& image, int number) const;
    // ������� ������ ��� ������� �� �����������.
    void printAllContours(cv::Mat& image) const;
    // ������� ������������� ������� �� �������� �����.
    void sortContours();
    // ������� ��������� �������� ������� � ������ �����.
    int getCurvature(std::vector<float>& curvature, const int chord_length, int number) const;
    // ����� �������� �� �����������.
    virtual void extractContours(cv::InputArray& Image) = 0;

protected:
    // ������ �������� �� �����������.
    std::vector<Contour> contours_;
};

class ContourMapMorph : public ContourMap
{
public:
    //������� ��������� ��� ������� � ����������� ��� ������ ��������������� ��������������.
    void extractContours(cv::InputArray& Image);
};
