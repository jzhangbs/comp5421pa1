#include "mainwindow.h"
#include "image.h"

#include <QApplication>
#include <QtCore>

#include <core.hpp>

Image* image;
cv::Mat filt_kernel[8];
double k[] = {
    -1, 0,  1,
    -1, 0,  1,
    0,  0,  0,

    0, -1,  0,
    0,  0,  1,
    0,  0,  0,

    0, -1, -1,
    0,  0,  0,
    0,  1,  1,

    0,  0,  0,
    0,  0, -1,
    0,  1,  0,

    0,  0,  0,
    -1, 0,  1,
    -1, 0,  1,

    0,  0,  0,
    -1, 0,  0,
    0,  1,  0,

    -1, -1, 0,
    0,  0,  0,
    1,  1,  0,

    0, -1,  0,
    1,  0,  0,
    0,  0,  0
};

void create_kernel() {

    for (int i=0; i<8; i++) {
        filt_kernel[i] = cv::Mat(3, 3, CV_64FC1, k+i*9);
//        qDebug("%f %f %f\n%f %f %f\n%f %f %f\n", kernel[i].at<double>(0, 0), kernel[i].at<double>(0, 1), kernel[i].at<double>(0, 2), kernel[i].at<double>(1, 0), kernel[i].at<double>(1, 1), kernel[i].at<double>(1, 2), kernel[i].at<double>(2, 0), kernel[i].at<double>(2, 1), kernel[i].at<double>(2, 2));
//        qDebug("%f", filt_kernel[0].at<double>(0,0));
    }
}

int main(int argc, char *argv[])
{
    create_kernel();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
