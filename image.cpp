#include "image.h"

Image::Image() {
    this->hasData = false;
}

void Image::open(const std::string & filename) {
    this->img = cv::imread(filename);
    cv::cvtColor(img, img, CV_BGR2RGB);
    this->scale = 1.;
    this->hasData = true;
    this->refresh();
}

void Image::refresh() {
    this->pixmap = QPixmap::fromImage(
                QImage(this->img.data,
                       this->img.cols,
                       this->img.rows,
                       this->img.step,
                       QImage::Format_RGB888));
    if (this->scale != 1.) {
        this->pixmap = this->pixmap.scaledToHeight(
                    int(this->scale * this->img.rows),
                    Qt::SmoothTransformation);
    }
    this->label->setPixmap(this->pixmap);
}

void Image::zoom_in() {
    if (!this->hasData) return;
    this->scale += .2;
    this->refresh();
}

void Image::zoom_out() {
    if (!this->hasData) return;
    this->scale -= .2;
    this->refresh();
}

void Image::setLabel(QLabel *_label) {
    this->label = _label;
}
