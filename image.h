#ifndef IMAGE_H
#define IMAGE_H

#include <QPixmap>
#include <QLabel>
#include <core.hpp>
#include <imgcodecs.hpp>
#include <cstring>
#include <imgproc.hpp>

class Image
{
public:
    QPixmap pixmap;
    bool hasData;

    Image();

    void open(const std::string&);
    void zoom_in();
    void zoom_out();
    void setLabel(QLabel*);

private:
    cv::Mat img;
    double scale;
    QLabel *label;

    void refresh();
};

#endif // IMAGE_H
