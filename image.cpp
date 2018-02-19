#include "image.h"
#include "shortest.h"

#include <imgproc.hpp>
#include <imgcodecs.hpp>
#include <highgui.hpp>

#include <cmath>

#define I3(i,j,k) ((i)*w()*8+(j)*8+(k))
#define I2(i,j) ((i)*w()+(j))

Image::Image() {
    has_data = false;
    has_seed = false;
    adj = nullptr;
    pred = nullptr;
    mode = IMG;
}

Image::~Image() {
    if (adj != nullptr) delete[] adj;
    if (pred != nullptr) delete[] pred;
}

void Image::open(const std::string & filename) {
    img = cv::imread(filename);
    cv::cvtColor(img, img, CV_BGR2RGB);
    scale = 1.;
    has_data = true;
    mode = IMG;

    get_edges();
    get_pixel_node();
    get_cost_graph();

    show_img();
}

void Image::show_img() {
    if (!has_data) return;
    mode = IMG;
    QPixmap pixmap = QPixmap::fromImage(
                QImage(img.data,
                       img.cols,
                       img.rows,
                       img.step,
                       QImage::Format_RGB888));
    if (scale != 1.) {
        pixmap = pixmap.scaledToHeight(
                    int(scale * img.rows),
                    Qt::SmoothTransformation);
    }
    label->setPixmap(pixmap);
}

void Image::show_pixel_node() {
    if (!has_data) return;
    mode = PIX;
    QPixmap pixmap = QPixmap::fromImage(
                QImage(pixel_node.data,
                       pixel_node.cols,
                       pixel_node.rows,
                       pixel_node.step,
                       QImage::Format_RGB888));
    label->setPixmap(pixmap);
}

void Image::show_cost_graph() {
    if (!has_data) return;
    mode = COST;
    QPixmap pixmap = QPixmap::fromImage(
                QImage(cost_graph.data,
                       cost_graph.cols,
                       cost_graph.rows,
                       cost_graph.step,
                       QImage::Format_RGB888));
    label->setPixmap(pixmap);
}

void Image::show_path_tree() {
    if (!has_data || !has_seed) return;
    mode = PATH;
    QPixmap pixmap = QPixmap::fromImage(
                QImage(path_tree.data,
                       path_tree.cols,
                       path_tree.rows,
                       path_tree.step,
                       QImage::Format_RGB888));
    label->setPixmap(pixmap);
}

void Image::zoom_in() {
    if (!has_data) return;
    scale += .2;
    show_img();
}

void Image::zoom_out() {
    if (!has_data) return;
    scale -= .2;
    show_img();
}

void Image::set_label(QLabel *_label) {
    label = _label;
}

int Image::h() {
    return img.rows;
}

int Image::hs() {
    return int(img.rows * scale);
}

int Image::w() {
    return img.cols;
}

int Image::ws() {
    return int(img.cols * scale);
}

// TODO: grayscale
void Image::get_edges() {
    if (adj != nullptr) delete[] adj;
    adj = new double[h()*w()*8];

    double max = 0;
    for (int k=0; k<8; k++) {
        cv::Mat filtered;
        cv::filter2D(img, filtered, CV_64FC3, filt_kernel[k]);
        for (int i=0; i<h(); i++) {
            for (int j=0; j<w(); j++) {
                double *p = filtered.ptr<double>(i, j);
//                qDebug("%d", I3(i,j,k));
                adj[I3(i,j,k)] = sqrt((p[0]*p[0]+p[1]*p[1]+p[2]*p[2])/3.);
                if (k==0 || k==2 || k==4 || k==6)
                    adj[I3(i,j,k)] /= 4.;
                if (adj[I3(i,j,k)] > max)
                    max = adj[I3(i,j,k)];
            }
        }
    }
//    qDebug("%f", max);
    for (int i=0; i<h(); i++) {
        for (int j=0; j<w(); j++) {
            for (int k=0; k<8; k++) {
                adj[I3(i,j,k)] = max - adj[I3(i,j,k)];
            }
        }
    }
}

void Image::get_pixel_node() {
    pixel_node = cv::Mat::zeros(h()*3, w()*3, CV_8UC3);
    for (int i=0; i<h(); i++)
        for (int j=0; j<w(); j++) {
            uchar *p = pixel_node.ptr<uchar>(i*3+1, j*3+1);
            uchar *po = img.ptr<uchar>(i, j);
            p[0] = po[0];
            p[1] = po[1];
            p[2] = po[2];
        }
}

void Image::get_cost_graph() {
    int dx[] = {-1, -1, 0, 1, 1, 1, 0, -1};
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};

    cost_graph = pixel_node.clone();
    for (int i=0; i<h(); i++)
        for (int j=0; j<w(); j++)
            for (int k=0; k<8; k++) {
                uchar *p = cost_graph.ptr<uchar>(3*i+1+dx[k], 3*j+1+dy[k]);
//                qDebug("%f", adj[I3(i,j,k)] * 255 / 2.);
                p[0] = p[1] = p[2] = uchar(adj[I3(i,j,k)] / 2.);
            }
}

void Image::get_seed_pos(int x, int y) {
    if (mode == IMG) {
        seed_x = int(x/scale);
        seed_y = int(y/scale);
    }
    else if (mode == PIX ||
             mode == COST ||
             mode == PATH) {
        seed_x = x/3;
        seed_y = y/3;
    }
    qDebug("%d %d raw", seed_x, seed_y);
}

void Image::get_path_tree(int x, int y) {
    int dx[] = {-1, -1, 0, 1, 1, 1, 0, -1};
    int dy[] = {0, 1, 1, 1, 0, -1, -1, -1};

    if (pred != nullptr) delete[] pred;
    pred = new int[h()*w()];

    get_seed_pos(x, y);
    shortest(adj, h(), w(), seed_x, seed_y, pred);

    path_tree = cv::Mat::zeros(3*h(), 3*w(), CV_8UC3);
    for (int i=0; i<h(); i++)
        for (int j=0; j<w(); j++) {
            if (i == seed_x && j == seed_y) continue;
            path_tree.ptr<uchar>(3*i+1, 3*j+1)[1] = 255;
            int p = pred[I2(i, j)];
            path_tree.ptr<uchar>(3*i+1+dx[p], 3*j+1+dy[p])[1] = 255;
        }
}
