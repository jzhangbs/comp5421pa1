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
    void openImg();
    void zoom_in();
    void zoom_out();
    void pixel_node();
    void cost_graph();
    void path_tree();
    void seed_snap();

private:
    Ui::MainWindow *ui;
};

extern Image *image;

#endif // MAINWINDOW_H
