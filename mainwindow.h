#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "image.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void open_img();
    void save_with_contour();
    void save_with_alpha();
    void save_mask();
    void zoom_in();
    void zoom_out();
    void pixel_node();
    void cost_graph();
    void path_tree();
    void image_();
    void seed_snap();

private:
    Ui::MainWindow *ui;

protected:
    void keyPressEvent(QKeyEvent*) override;
};

extern Image *image;

#endif // MAINWINDOW_H
