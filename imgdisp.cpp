#include "imgdisp.h"

#include <QWidget>
#include <QtCore>

void ImgDisp::mousePressEvent(QMouseEvent *event) {
    qDebug("%d %d", event->x(), event->y());
    if (!image->has_data) return;
    if (event->x() >= pixmap()->width() ||
            event->y() >= pixmap()->height()) return;
    if (!image->has_seed) {
        image->act_start_contour(event->y(), event->x());
    }
    else if (!image->is_finish_contour(event->y(), event->x())) {
        image->act_add_interm(event->y(), event->x());
    }
    else {
        image->act_complete_contour();
    }
}

void ImgDisp::mouseMoveEvent(QMouseEvent *event) {
    if (!image->has_data || !image->has_seed) return;
    if (event->x() >= pixmap()->width() ||
            event->y() >= pixmap()->height()) return;
    image->show_min_path(event->y(), event->x());
}

void ImgDisp::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Escape:
        image->act_del_seed(); break;
    };
}

ImgDisp::ImgDisp(QWidget *parent)
    :QLabel(parent) {

}
