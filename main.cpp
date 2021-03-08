#include <fstream>
#include <optional>
#include <list>

#include <opencv2/highgui.hpp>
#include <opencv2/video/video.hpp>

#include <Hand.h>
#include <Timer.h>
#include <Debug.hpp>
#include <VideoSequenceCapture.h>
#include <HandDetector.hpp>
#include <GesturesRecognition.h>

using namespace std;
using namespace cv;

// Отрисовка всех найденных рук.
void printHands(InputArray image, const std::list<Hand>& hands)
{
    Mat frame = image.getMat();
    for (const auto& hand : hands)
    {
        hand.print(frame);
    }
}

int main()
{
    Timer total_timer, gestures_timer;
    VideoCapture video(0);
    //VideoSequenceCapture video("d:\\test_videos\\Input7\\0.png");

    namedWindow("Input");
    namedWindow("Background");
    namedWindow("Motion");
    namedWindow("Tracker");

    // Пропускаем первые кадры, чтобы стабилизировалась
    // яркость на изображениях, полученных с камеры.
    Mat frame;
    for (int i = 0; i < 20; i++)
    {
        video >> frame;
        if (frame.empty()) continue;
        imageShow("Input", frame);
        waitKey(30);
    }

    Mat tracker_image(frame.size(), CV_8UC3);

    HandDetector hand_detector(frame.rows, frame.cols);
    GesturesRecognition gestures_recognition;

    while (true)
    {
        // Получение входного изображения.
        total_timer.start();
        video >> frame;
        if (frame.empty())
            break;

        imageShow("Input", frame);
        hand_detector.detect(frame);

        imageShow("Motion", hand_detector.getMotionImage());
        auto hands = hand_detector.getHands();

        gestures_timer.start();
        gestures_recognition.apply(hands);
        gestures_timer.stop();

        frame.copyTo(tracker_image);
        printHands(tracker_image, hands);
        gestures_recognition.printClicks(tracker_image);
        imageShow("Tracker", tracker_image);

        total_timer.stop();
        int c = waitKey(30);
        if (c == 27)
            break;
    }

    frame.release();
    tracker_image.release();
    destroyAllWindows();

    // Записываем время работы программы.
    ofstream time_log("Time.txt");
    time_log << "Program time:" << endl;
    time_log << "Total time: " << total_timer.getTime() << " sec." << endl;
    time_log << "Correction of exposition: " << hand_detector.getExpositionTime() << " sec." << endl;
    time_log << "Motion detection: " << hand_detector.getMotionTime() << " sec." << endl;
    time_log << "Hand tracking: " << hand_detector.getTrackerTime() << " sec." << endl;
    time_log << "Hand detection: " << hand_detector.getDetectorTime() << " sec." << endl;
    time_log << "Gestures Recognition: " << gestures_timer.getTime() << " sec." << endl;
    time_log.close();

    return 0;
}
