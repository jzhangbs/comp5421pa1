#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QPixmap>

#include <opencv.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->image.setLabel(ui->label);

    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openImg);
    connect(ui->actionZoom_in, &QAction::triggered, this, &MainWindow::zoom_in);
    connect(ui->actionZoom_out, &QAction::triggered, this, &MainWindow::zoom_out);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openImg()
{
    QString f = QFileDialog::getOpenFileName(this,
                                             tr("Open Image"), "",
                                             tr("Image (*.bmp;*.jpg;*.png);;All Files (*)"));
    this->image.open(f.toStdString());
}

void MainWindow::zoom_in() {
    this->image.zoom_in();
}

void MainWindow::zoom_out() {
    this->image.zoom_out();
}
