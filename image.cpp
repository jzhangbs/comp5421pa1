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
    cv::Mat img_draw;
    if (scale != 1.) {
        cv::resize(img, img_draw,
                   cv::Size(int(scale*w()), int(scale*h())),
                   scale, scale, cv::INTER_CUBIC);
    }
    else {
        img_draw = img;
    }
    // draw contours
    QPixmap pixmap = QPixmap::fromImage(
                QImage(img_draw.data,
                       img_draw.cols,
                       img_draw.rows,
                       img_draw.step,
                       QImage::Format_RGB888));
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

void Image::show_stored(cv::Mat img_draw,
                        Contours& contours) {

    Contours::iterator i;
    Contour::iterator j;
    for (i=contours.begin(); i!=contours.end(); ++i) {
        if (i->size() < 2) continue;
        for (j=i->begin()+1; j!=i->end(); ++j) {
            cv::Point ctmp = real_to_raw((j-1)->x, (j-1)->y);
            cv::Point ntmp = real_to_raw(j->x, j->y);
            int curr_x_s = ctmp.x;
            int curr_y_s = ctmp.y;
            int next_x_s = ntmp.x;
            int next_y_s = ntmp.y;

            cv::line(img_draw,
                     cv::Point(curr_y_s, curr_x_s),
                     cv::Point(next_y_s, next_x_s),
                     cv::Scalar(255, 0, 0),
                     5);
        }
    }
}

void Image::show_pending(cv::Mat img_draw,
                         int x, int y) {

    cv::Point ptmp = raw_to_real(x, y);
    x = ptmp.x;
    y = ptmp.y;

    if (is_finish_contour(x, y)) {
        x = active.begin()->begin()->x;
        y = active.begin()->begin()->y;
    }

    int curr_x = x;
    int curr_y = y;
    int next_x, next_y;
    int curr_x_s, curr_y_s, next_x_s, next_y_s;
    while (curr_x != seed_x || curr_y != seed_y) {
        next_x = curr_x + di[pred[I2(curr_x, curr_y)]];
        next_y = curr_y + dj[pred[I2(curr_x, curr_y)]];

        cv::Point ctmp = real_to_raw(curr_x, curr_y);
        cv::Point ntmp = real_to_raw(next_x, next_y);
        curr_x_s = ctmp.x;
        curr_y_s = ctmp.y;
        next_x_s = ntmp.x;
        next_y_s = ntmp.y;

        cv::line(img_draw,
                 cv::Point(curr_y_s, curr_x_s),
                 cv::Point(next_y_s, next_x_s),
                 cv::Scalar(255, 0, 0),
                 5);

        curr_x = next_x;
        curr_y = next_y;
    }
}

void Image::show_min_path(int x, int y) {
    if (!has_data || !has_seed) return;

    cv::Mat img_draw;
    switch (mode) {
    case IMG:
        cv::resize(img, img_draw,
                   cv::Size(ws(), hs()),
                   scale, scale, cv::INTER_CUBIC);
        break;
    case PIX:
        img_draw = pixel_node.clone(); break;
    case COST:
        img_draw = cost_graph.clone(); break;
    case PATH:
        img_draw = path_tree.clone(); break;
    }

    show_stored(img_draw, fixed);
    show_stored(img_draw, active);
    show_pending(img_draw, x, y);

    QPixmap pixmap = QPixmap::fromImage(
                QImage(img_draw.data,
                       img_draw.cols,
                       img_draw.rows,
                       img_draw.step,
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
                adj[I3(i,j,k)] = sqrt((p[0]*p[0]+p[1]*p[1]+p[2]*p[2])/3.);
                if (k==0 || k==2 || k==4 || k==6)
                    adj[I3(i,j,k)] /= 4.;
                if (adj[I3(i,j,k)] > max)
                    max = adj[I3(i,j,k)];
            }
        }
    }
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
    cost_graph = pixel_node.clone();
    for (int i=0; i<h(); i++)
        for (int j=0; j<w(); j++)
            for (int k=0; k<8; k++) {
                uchar *p = cost_graph.ptr<uchar>(3*i+1+di[k], 3*j+1+dj[k]);
                p[0] = p[1] = p[2] = uchar(adj[I3(i,j,k)] / 2.);
            }
}

cv::Point Image::raw_to_real(int x, int y) {
    cv::Point p;
    if (mode == IMG) {
        p.x = int(x/scale);
        p.y = int(y/scale);
    }
    else if (mode == PIX ||
             mode == COST ||
             mode == PATH) {
        p.x = x/3;
        p.y = y/3;
    }
    return p;
}

cv::Point Image::real_to_raw(int x, int y) {
    cv::Point p;
    if (mode == PATH ||
            mode == PIX ||
            mode == COST) {
        p.x = x * 3 + 1;
        p.y = y * 3 + 1;
    }
    else if (mode == IMG) {
        p.x = int(x * scale);
        p.y = int(y * scale);
    }
    return p;
}

void Image::get_path_tree(int x, int y) {
    if (pred != nullptr) delete[] pred;
    pred = new int[h()*w()];

    cv::Point ptmp = raw_to_real(x, y);
    seed_x = ptmp.x;
    seed_y = ptmp.y;

    shortest(adj, h(), w(), seed_x, seed_y, pred);

    path_tree = cv::Mat::zeros(3*h(), 3*w(), CV_8UC3);
    for (int i=0; i<h(); i++)
        for (int j=0; j<w(); j++) {
            if (i == seed_x && j == seed_y) continue;
            path_tree.ptr<uchar>(3*i+1, 3*j+1)[1] = 255;
            int p = pred[I2(i, j)];
            path_tree.ptr<uchar>(3*i+1+di[p], 3*j+1+dj[p])[1] = 255;
        }

    if (mode == PATH)
        show_path_tree();
}

void Image::del_seed() {

}

void Image::start_contour(int x, int y) {
    image->has_seed = true;
    get_path_tree(x, y);
}

void Image::add_interm(int x, int y) {
    cv::Point ptmp = raw_to_real(x, y);
    int curr_x = ptmp.x;
    int curr_y = ptmp.y;

    Contour new_path;
    while (curr_x != seed_x || curr_y != seed_y) {
        new_path.push_back(cv::Point(curr_x, curr_y));
        curr_x = curr_x + di[pred[I2(curr_x, curr_y)]];
        curr_y = curr_y + dj[pred[I2(curr_x, curr_y)]];
    }
    active.push_back(new_path);

    get_path_tree(x, y);
}

void Image::complete_contour() {
    cv::Point start_seed = active.size()==0?
                cv::Point(seed_x, seed_y) :
                *active.begin()->begin();
    int curr_x = start_seed.x;
    int curr_y = start_seed.y;

    Contour new_path;
    while (curr_x != seed_x || curr_y != seed_y) {
        new_path.push_back(cv::Point(curr_x, curr_y));
        curr_x = curr_x + di[pred[I2(curr_x, curr_y)]];
        curr_y = curr_y + dj[pred[I2(curr_x, curr_y)]];
    }
    active.push_back(new_path);

    Contours::iterator i;
    Contour::iterator j;
    new_path.erase(new_path.begin(), new_path.end());
    for (i=active.begin(); i!=active.end(); ++i)
        for (j=i->begin(); j!=i->end(); ++j) {
            new_path.push_back(*j);
        }
    if (new_path.size() >= 3)
        fixed.push_back(new_path);

    image->has_seed = false;
}

bool Image::is_finish_contour(int x, int y) {
    cv::Point start_seed = active.size()==0?
                cv::Point(seed_x, seed_y) :
                *active.begin()->begin();
    cv::Point ptmp = real_to_raw(start_seed.x, start_seed.y);
    double d = cv::norm(ptmp-cv::Point(x,y));
    return d < 3.;
}
