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
    image = new Image();
    image->set_label(ui->label);

    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::open_img);
    connect(ui->actionSave_with_contour, &QAction::triggered, this, &MainWindow::save_with_contour);
    connect(ui->actionSave_with_alpha_channel, &QAction::triggered, this, &MainWindow::save_with_alpha);
    connect(ui->actionSave_mask, &QAction::triggered, this, &MainWindow::save_mask);
    connect(ui->actionZoom_in, &QAction::triggered, this, &MainWindow::zoom_in);
    connect(ui->actionZoom_out, &QAction::triggered, this, &MainWindow::zoom_out);
    connect(ui->actionPixel_node, &QAction::triggered, this, &MainWindow::pixel_node);
    connect(ui->actionCost_graph, &QAction::triggered, this, &MainWindow::cost_graph);
    connect(ui->actionPath_tree, &QAction::triggered, this, &MainWindow::path_tree);
    connect(ui->actionImage, &QAction::triggered, this, &MainWindow::image_);
    connect(ui->actionSeed_snap, &QAction::triggered, this, &MainWindow::seed_snap);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open_img()
{
    QString f = QFileDialog::getOpenFileName(this,
                                             tr("Open Image"), "",
                                             tr("Image (*.bmp;*.jpg;*.png;*.tif);;All Files (*)"));
    if (f.isNull()) return;
    image->act_open(f.toStdString());
}

void MainWindow::save_with_contour() {
    QString f = QFileDialog::getSaveFileName(
                this,
                tr("Save image with contours"), "c",
                tr("Image (*.png)")
                );
    if (f.isNull()) return;
    image->save_with_contour(f.toStdString());
}

void MainWindow::save_with_alpha() {
    QString f = QFileDialog::getSaveFileName(
                this,
                tr("Save image with alpha channel"), "a",
                tr("Image (*.png)")
                );
    if (f.isNull()) return;
    image->save_with_alpha(f.toStdString());
}

void MainWindow::save_mask() {
    QString f = QFileDialog::getSaveFileName(
                this,
                tr("Save mask"), "m",
                tr("Image (*.png)")
                );
    if (f.isNull()) return;
    image->save_mask(f.toStdString());
}

void MainWindow::zoom_in() {
    image->act_zoom_in();
}

void MainWindow::zoom_out() {
    image->act_zoom_out();
}

void MainWindow::pixel_node() {
    image->show_pixel_node();
}

void MainWindow::cost_graph() {
    image->show_cost_graph();
}

void MainWindow::path_tree() {
    image->show_path_tree();
}

void MainWindow::image_() {
    image->show_img();
}

void MainWindow::seed_snap() {
    image->seed_snap();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Escape:
        image->act_del_seed(); break;
    };
}
