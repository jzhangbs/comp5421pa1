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

    Image image;

public slots:
    void openImg();
    void zoom_in();
    void zoom_out();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
