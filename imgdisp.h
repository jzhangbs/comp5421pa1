#ifndef IMGDISP_H
#define IMGDISP_H

#include "image.h"

#include <QLabel>
#include <QMouseEvent>

class ImgDisp: public QLabel {

    Q_OBJECT

public:
    ImgDisp(QWidget *parent=Q_NULLPTR);

protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;

};

extern Image *image;

#endif // IMGDISP_H
