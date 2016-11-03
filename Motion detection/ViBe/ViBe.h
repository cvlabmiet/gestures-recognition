/*
    �������� ����������� �������� ViBe.
*/

#ifndef __VIBE_H__
#define __VIBE_H__

#include <core.hpp>
#include <video.hpp>

class ViBe : public cv::BackgroundSubtractor
{
public:
    // ������� ��������� ����� ����������� � ��������� ������.
    // TODO: ��������� prob ����� learningrate.
    void apply(const cv::InputArray &Image, cv::OutputArray &mask, double);
    // ������� ��������� ����������� ����.
    void getBackgroundImage(cv::OutputArray& backgroundImage) const;

    ViBe();
    ViBe(int history_depth, int radius, int min_overlap, int probability);
    ~ViBe();

private:
    int history_depth_; // ���������� �������� �������� ��� ������� �������.
    int sqr_rad_; // ������� ������������� ���������� ��� ��������� ����� � ������.
    int min_overlap_; // ����������� ���������� ���������� �������� ������� � �������.
    int probability_; // ����������� ���������� ������.
    cv::Mat_<cv::Point3_<uchar>*> samples_; // ������� ��� �������� �������� ��������.
    cv::Mat bg_mat_; // ������� ��� �������� ����.
    cv::RNG generator_; // ��������� ��������� ����� (������������ ����������� ����� �������������).

    // ������� ������������� ������.
    void initialize(const cv::Mat &);
    // ������� ����� ��������� ����� �� ������������� �������.
    cv::Point2i getRandomNeiborPixel(const cv::Point2i &);

    // ����������� ���������
    void operator=(const ViBe &) = delete;
};

#endif // __VIBE_H__
