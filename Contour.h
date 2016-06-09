#include <highgui.hpp>
#include <video\video.hpp>
#include <vector>

#define ___DEBUG___ 1

struct Contour
{
    // ������ �������.
    cv::Point2i start;
    // ������ ��� �������� ��������� ������� ����.
    std::vector<int> chain_code;
};

class ContourMap
{
protected:
    // ������ �������� �� �����������.
    std::vector<Contour> contours;
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
    virtual void findContours(cv::InputArray& Image) = 0;
};

class ContourMapMorph : public ContourMap
{
public:
    //������� ��������� ��� ������� � ����������� ��� ������ ��������������� ��������������.
    void findContours(cv::InputArray& Image);
};

//class ContourMap2 : public ContourMap
//{
//private:
//    //������� ������� ����� �������� �������� �����.
//    bool IsContourPixel(const cv::Mat &image, int x, int y);
//    std::vector<cv::Point2i> ReturnContour(cv::Mat& image, const cv::Point2i start, int mark);
//    bool FindContour(const cv::Mat& image, cv::Point2i currentPixel, int mark) const;
//    cv::Point2i FindNextPoint(const cv::Mat& image, cv::Point2i analyzPixel, int mark, cv::Point2i prevPixel ) const;
//    void Preprocessing(cv::Mat& image);
//public:
//    void findContours(cv::InputArray& Image);
//};
