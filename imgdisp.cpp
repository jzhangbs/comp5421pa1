#include "imgdisp.h"

#include <QWidget>
#include <QtCore>

void ImgDisp::mousePressEvent(QMouseEvent *event) {
    qDebug("%d %d", event->x(), event->y());
    if (!image->has_data) return;
    if (event->x() >= pixmap()->width() ||
            event->y() >= pixmap()->height()) return;
    image->has_seed = true;
    image->get_path_tree(event->y(), event->x());
}

ImgDisp::ImgDisp(QWidget *parent)
    :QLabel(parent) {

}
