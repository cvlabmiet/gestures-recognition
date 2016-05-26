/*
    �������� ����������� �������� ViBe.
*/

#ifndef __VIBE_H__
#define __VIBE_H__

#include <core.hpp>
#include <video.hpp>

class ViBe : public cv::BackgroundSubtractor
{
private:
    int history_depth; // ���������� �������� �������� ��� ������� �������.
    int sqr_rad; // ������� ������������� ���������� ��� ��������� ����� � ������.
    int min_overlap; // ����������� ���������� ���������� �������� ������� � �������.
    int prob; // ����������� ���������� ������.
    cv::Mat_<cv::Point3_<uchar>*> samples; // ������� ��� �������� �������� ��������.
    cv::Mat bg_mat; // ������� ��� �������� ����.
    cv::RNG generator; // ��������� ��������� ����� (������������ ����������� ����� �������������).

    // �������, ����������� ������� ���������� ����� ����� �������.
    double distancesqr(const cv::Point3_<uchar> &, const cv::Point3_<uchar> &) const;
    // ������� ������������� ������.
    void initialization(const cv::Mat &);
    // ������� ����� ��������� ����� �� ������������� �������.
    cv::Point2i GetRandNeibPixel(const cv::Point2i &); 

    // ����������� ���������
    void operator=(const ViBe &) = delete;

public:
    // ������� ��������� ����� ����������� � ��������� ������.
    // TODO: ��������� prob ����� learningrate.
    void apply(const cv::InputArray &Image, cv::OutputArray &mask, double);
    // ������� ��������� ����������� ����.
    void getBackgroundImage(cv::OutputArray& backgroundImage) const;
    ViBe();
    ViBe(int history_depth, int rad, int h, int prob);
    ~ViBe();
};

#endif // __VIBE_H__
