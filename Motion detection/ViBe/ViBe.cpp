/*
    ���������� ��������� ����������� �������� ViBe.
*/

#include "ViBe.h"

using namespace cv;

ViBe::ViBe() 
    : history_depth(20), sqr_rad(20 * 20), min_overlap(2), prob(16), samples(), generator()
{
}

ViBe::ViBe(int _history_depth, int rad, int _min_overlap, int _prob)
    : history_depth(_history_depth), sqr_rad(rad*rad), min_overlap(_min_overlap), prob(_prob), generator(), samples()
{
}

ViBe::~ViBe()
{
    for (int y = 0; y < samples.rows; ++y)
    {
        for (int x = 0; x < samples.cols; ++x)
        {
            delete[] samples(y, x);
        }
    }
    samples.release();
}

double ViBe::distancesqr(const Point3i &pixel, const Point3i &sample) const
{
    double sum = 0;
    sum += (pixel.x - sample.x)*(pixel.x - sample.x);
    sum += (pixel.y - sample.y)*(pixel.y - sample.y);
    sum += (pixel.z - sample.z)*(pixel.z - sample.z);
    return sum;
}

void ViBe::initialization(const Mat &image)
{
    samples.release();
    samples.create(image.rows, image.cols);

    for (int y = 0; y < image.rows; ++y)
    {
        for (int x = 0; x < image.cols; ++x)
        {
            samples(y, x) = new Point3i[history_depth];
            //��������� ������ �������� ������ ��������� �������� �������.
            samples(y, x)[0] = {image.ptr(y)[3 * x], 
                                image.ptr(y)[3 * x + 1], 
                                image.ptr(y)[3 * x + 2]};

            //��������� �������� ������ ��������� ���������� �������� ��������.
            for (int k = 1; k < history_depth; ++k)
            {
                Point2i neib_pixel = GetRandNeibPixel(Point2i(x, y)); // TODO: ������� �������� �� ������.
                samples(y, x)[k] = {image.ptr(neib_pixel.y)[3 * neib_pixel.x],
                                    image.ptr(neib_pixel.y)[3 * neib_pixel.x + 1], 
                                    image.ptr(neib_pixel.y)[3 * neib_pixel.x + 2]};
            }
        }
    }
}

Point2i ViBe::GetRandNeibPixel(const Point2i &pixel)
{
    Point2i neib_pixel;
    do
    {
        //������� ���������� � ��������� �������.
        if (pixel.x == 0)
            neib_pixel.x = generator.uniform(0, 2);
        else if (pixel.x == samples.cols - 1)
            neib_pixel.x = generator.uniform(samples.cols - 2, samples.cols);
        else
            neib_pixel.x = generator.uniform(pixel.x - 1, pixel.x + 2);

        //������� ���������� � ��������� �������.
        if (pixel.y == 0)
            neib_pixel.y = generator.uniform(0, 2);
        else if (pixel.y == samples.rows - 1)
            neib_pixel.y = generator.uniform(samples.rows - 2, samples.rows);
        else
            neib_pixel.y = generator.uniform(pixel.y - 1, pixel.y + 2);
    } while ((neib_pixel.x == pixel.x) && (neib_pixel.y == pixel.y));

    return neib_pixel;
}

void ViBe::apply(const InputArray &_image, OutputArray &_fgmask, double)
{
    const Mat image = _image.getMat();
    _fgmask.create(image.rows, image.cols, CV_8U);
    Mat fgmask = _fgmask.getMat();

    // ����������������� ���� ������ ������ ��� ������� ���� � ������� ���������.
    if ((samples.empty() == 1) || (samples.rows != image.rows) || (samples.cols != image.cols))
    {
        initialization(image);
        return;
    }

    for (int y = 0; y < image.rows; ++y)
    {
        const uchar* src = image.ptr(y);
        uchar* dst = fgmask.ptr(y);
        for (int x = 0; x < image.cols; ++x)
        {
            // ������� ���������� ���������� �������� �������� ������� � �������.
            Point3i pixel(src[x * 3], src[x * 3 + 1], src[x * 3 + 2]);
            int counter = 0;
            int index = 0;
            double dist = 0;

            while ((counter < min_overlap) && (index < history_depth))
            {
                Point3i model_pixel = samples(y, x)[index];
                dist = distancesqr(pixel, model_pixel);
                if (dist < sqr_rad)
                {
                    counter++;
                }
                index++;
            }

            // ���� ������� ����������� ����, �� ��������� ������.
            if (counter >= min_overlap)
            {
                dst[x] = BackGround;
                // ���������� ������ �������� �������.
                int randnumber = generator.uniform(0, prob);
                if (randnumber == 0)
                {
                    randnumber = generator.uniform(0, history_depth);
                    samples(y, x)[randnumber] = pixel;
                }

                // ���������� ������ ���������� ������ �� ������������� �������.
                randnumber = generator.uniform(0, prob);
                if (randnumber == 0)
                {
                    Point2i neib_pixel = GetRandNeibPixel(Point2i(x, y));
                    randnumber = generator.uniform(0, history_depth);
                    samples(neib_pixel.y, neib_pixel.x)[randnumber] = pixel;
                }
            }
            // �������� ���������� �����.
            else
            {
                dst[x] = ForeGround;
            }
        }
    }
}

void ViBe::getBackgroundImage(cv::OutputArray &) const
{
}
