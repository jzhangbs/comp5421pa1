#include "image.h"
#include "shortest.h"

#include <imgproc.hpp>
#include <imgcodecs.hpp>
#include <highgui.hpp>

#include <cmath>
#include <complex>
#include <deque>

#define I3(i,j,k) ((i)*w()*8+(j)*8+(k))
#define I2(i,j) ((i)*w()+(j))

Image::Image() {
    has_data = false;
    has_seed = false;
    adj = nullptr;
    pred = nullptr;
    gradient = nullptr;
    mode = IMG;
    seed_snap_=0;
}

Image::~Image() {
    if (adj != nullptr) delete[] adj;
    if (pred != nullptr) delete[] pred;
    if(this->gradient) delete gradient;
}

void Image::act_open(const std::string & filename) {
    img = cv::imread(filename);
//    qDebug("%d", img.type());
    cv::cvtColor(img,grey_img,CV_BGR2GRAY);
    cv::cvtColor(img, img, CV_BGR2RGB);

    scale = 1.;
    has_data = true;
    mode = IMG;

    fixed.erase(fixed.begin(), fixed.end());
    active.erase(active.begin(), active.end());

    get_edges();
    get_pixel_node();
    get_cost_graph();

    if(this->gradient) delete gradient;
    gradient=get_gradient(img.cols,img.rows);

    show_img();
}

void Image::show_img() {
    if (!has_data) return;
    mode = IMG;
    show_min_path();
}

void Image::show_pixel_node() {
    if (!has_data) return;
    mode = PIX;
    show_min_path();
}

void Image::show_cost_graph() {
    if (!has_data) return;
    mode = COST;
    show_min_path();
}

void Image::show_path_tree() {
    if (!has_data) return;
    if (!has_seed && fixed.size()==0) return;
    mode = PATH;
    show_min_path();
}

void Image::show_grad() {
    if (!has_data) return;
    if (!seed_snap_) return;
    mode = GRAD;
    show_min_path();
}

void Image::draw_stored(Contours& contours,
                        cv::Mat img_draw,
                        bool do_scale,
                        cv::Scalar color,
                        int thickness) {

    Contours::iterator i;
    Contour::iterator j;
    for (i=contours.begin(); i!=contours.end(); ++i) {
        if (i->size() < 2) continue;
        for (j=i->begin()+1; j!=i->end(); ++j) {
            cv::Point ctmp = do_scale?
                        real_to_raw((j-1)->x, (j-1)->y):
                        *(j-1);
            cv::Point ntmp = do_scale?
                        real_to_raw(j->x, j->y):
                        *j;
            int curr_x_s = ctmp.x;
            int curr_y_s = ctmp.y;
            int next_x_s = ntmp.x;
            int next_y_s = ntmp.y;

            cv::line(img_draw,
                     cv::Point(curr_y_s, curr_x_s),
                     cv::Point(next_y_s, next_x_s),
                     color,
                     thickness);
        }
    }
}

void Image::draw_pending(int x, int y) {

    cv::Point ptmp = raw_to_real(x, y);
    x = ptmp.x;
    y = ptmp.y;

    if (is_finish_contour(x, y)) {
        x = get_start_seed().x;
        y = get_start_seed().y;
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

        cv::line(rendered,
                 cv::Point(curr_y_s, curr_x_s),
                 cv::Point(next_y_s, next_x_s),
                 cv::Scalar(255, 0, 0),
                 3);

        curr_x = next_x;
        curr_y = next_y;
    }
}

void Image::show_min_path(int x, int y, bool do_scale) {
    if (!has_data) return;

    if (do_scale)
        switch (mode) {
        case GRAD:
            cv::resize(grad_img, rendered,
                   cv::Size(ws(), hs()),
                   scale, scale, cv::INTER_CUBIC);
            break;
        case IMG:
            cv::resize(img, rendered,
                   cv::Size(ws(), hs()),
                   scale, scale, cv::INTER_CUBIC);
            break;
        case PIX:
            rendered = pixel_node.clone(); break;
        case COST:
            rendered = cost_graph.clone(); break;
        case PATH:
            rendered = path_tree.clone(); break;
        }
    else
        rendered = img.clone();

    draw_stored(fixed, rendered, do_scale);
    draw_stored(active, rendered, do_scale);
    if (x!=-1 && do_scale) draw_pending(x, y);

    if (do_scale) {
        QPixmap pixmap = QPixmap::fromImage(
                    QImage(rendered.data,
                           rendered.cols,
                           rendered.rows,
                           rendered.step,
                           QImage::Format_RGB888));
        label->setPixmap(pixmap);
    }
}

void Image::save_with_contour(const std::string &filename) {
    show_min_path(-1, -1, false);
    cv::cvtColor(rendered, rendered, CV_RGB2BGR);
    cv::imwrite(filename, rendered);
}

void Image::save_mask(const std::string &filename,
                      bool do_write) {
    mask = cv::Mat::zeros(h(), w(), CV_8UC1);
    draw_stored(fixed, mask, false, cv::Scalar(255), 1);

    cv::copyMakeBorder(mask, mask, 1,1,1,1,
                       cv::BORDER_CONSTANT, 0);
    // Floodfill from point (0, 0)
    cv::Mat im_floodfill = mask.clone();
    cv::floodFill(im_floodfill,
                  cv::Point(0,0),
                  cv::Scalar(255));

    // Invert floodfilled image
    cv::Mat im_floodfill_inv;
    cv::bitwise_not(im_floodfill, im_floodfill_inv);

    // Combine the two images to get the foreground.
    mask = (mask | im_floodfill_inv);

    mask = mask(cv::Rect(1,1, w(),h()));

    if (do_write) cv::imwrite(filename, mask);
}

void Image::save_with_alpha(const std::string &filename) {
    save_mask("", false);
    cv::Mat channels[4];
    cv::Mat out;
    cv::split(img, channels);
    channels[3] = mask;
    cv::merge(channels, 4, out);
    cv::cvtColor(out, out, CV_RGBA2BGRA);
    cv::imwrite(filename, out);
}

void Image::act_zoom_in() {
    if (!has_data) return;
    scale += .2;
    show_min_path();
}

void Image::act_zoom_out() {
    if (!has_data) return;
    scale -= .2;
    show_min_path();
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
    if (mode == IMG || mode == GRAD) {
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
    else if (mode == IMG || mode == GRAD) {
        p.x = int(x * scale);
        p.y = int(y * scale);
    }
    return p;
}

void Image::get_path_tree(int x, int y, bool is_raw) {
    if (pred != nullptr) delete[] pred;
    pred = new int[h()*w()];

    cv::Point ptmp = raw_to_real(x, y);
    seed_x = is_raw? ptmp.x : x;
    seed_y = is_raw? ptmp.y : y;

    shortest(adj, h(), w(), seed_x, seed_y, pred);

    path_tree = cv::Mat::zeros(3*h(), 3*w(), CV_8UC3);
    for (int i=0; i<h(); i++)
        for (int j=0; j<w(); j++) {
            if (i == seed_x && j == seed_y) continue;
            path_tree.ptr<uchar>(3*i+1, 3*j+1)[1] = 255;
            int p = pred[I2(i, j)];
            path_tree.ptr<uchar>(3*i+1+di[p], 3*j+1+dj[p])[1] = 255;
        }
}

void Image::act_del_seed() {
    if (!has_data) return;
    if (!has_seed && fixed.size()==0) return;
    if (has_seed) {
        if (active.size()==0) {
            has_seed = false;
        }
        else if (active.size()==1) {
            get_path_tree(start_seed_x, start_seed_y, false);
            active.pop_back();
        }
        else if (active.size()>1) {
            get_path_tree(
                        (active.rbegin()+1)->begin()->x,
                        (active.rbegin()+1)->begin()->y,
                        false
                        );
            active.pop_back();
        }
    }
    else {
        fixed.pop_back();
    }
    show_min_path();
}

void Image::act_start_contour(int x, int y) {
    image->has_seed = true;

    cv::Point ptmp = raw_to_real(x, y);
    if(this->seed_snap_) clip(ptmp.x, ptmp.y);
    start_seed_x = ptmp.x;
    start_seed_y = ptmp.y;

    get_path_tree(ptmp.x, ptmp.y, false);

    show_min_path();
}

void Image::act_add_interm(int x, int y) {
    cv::Point ptmp = raw_to_real(x, y);
    if(this->seed_snap_) clip(ptmp.x, ptmp.y);
    int curr_x = ptmp.x;
    int curr_y = ptmp.y;

    int next_x, next_y;

    Contour new_path;
    while (curr_x != seed_x || curr_y != seed_y) {
        new_path.push_back(cv::Point(curr_x, curr_y));
        next_x = curr_x + di[pred[I2(curr_x, curr_y)]];
        next_y = curr_y + dj[pred[I2(curr_x, curr_y)]];
        curr_x = next_x;
        curr_y = next_y;
    }
    active.push_back(new_path);

    get_path_tree(ptmp.x, ptmp.y, false);

    show_min_path();
}

void Image::act_complete_contour() {
    Contours::iterator i;
    Contour::reverse_iterator j;

    cv::Point start_seed = get_start_seed();
    int curr_x = start_seed.x;
    int curr_y = start_seed.y;
    int next_x, next_y;

    Contour new_path;
    while (curr_x != seed_x || curr_y != seed_y) {
        new_path.push_back(cv::Point(curr_x, curr_y));
        next_x = curr_x + di[pred[I2(curr_x, curr_y)]];
        next_y = curr_y + dj[pred[I2(curr_x, curr_y)]];
        curr_x = next_x;
        curr_y = next_y;
    }
    active.push_back(new_path);

    new_path.erase(new_path.begin(), new_path.end());
    for (i=active.begin(); i!=active.end(); ++i)
        for (j=i->rbegin(); j!=i->rend(); ++j) {
            new_path.push_back(*j);
        }
    if (new_path.size() >= 3)
        fixed.push_back(new_path);

    active.erase(active.begin(), active.end());

    image->has_seed = false;

    show_min_path();
}

bool Image::is_finish_contour(int x, int y) {
    cv::Point start_seed = get_start_seed();
    cv::Point ptmp = real_to_raw(start_seed.x, start_seed.y);
    double d = cv::norm(ptmp-cv::Point(x,y));
    return d < 10.;
}

cv::Point Image::get_start_seed() {
    return cv::Point(start_seed_x, start_seed_y);
}

void Image::seed_snap(){
    seed_snap_=!seed_snap_;
}

Gradient* Image::get_gradient(int width, int height) {
    Gradient* gradient = new Gradient(height,std::vector<std::complex<double>>(width,std::complex<double>(0,0)));
    grad_img = cv::Mat::zeros(img.size(), CV_8UC3);
    for (int y = 1; y < height - 1; y++) {
        //gradient[y]=new complex<double>[width];
        for (int x = 1; x < width - 1; x++) {
            (*gradient)[y][x] = std::complex<double>
                (grey_img.at<uchar>(y-1,x+1) * 1.0 + grey_img.at<uchar>(y,x+1) * 2.0
                    + grey_img.at<uchar>(y+1,x+1) * 1.0 - grey_img.at<uchar>(y-1,x-1) * 1.0
                    - grey_img.at<uchar>(y,x-1) * 2.0 - grey_img.at<uchar>(y+1,x-1) * 1.0,
                    grey_img.at<uchar>(y+1,x-1) * 1.0 + grey_img.at<uchar>(y+1,x) * 2.0
                    + grey_img.at<uchar>(y+1,x+1) * 1.0 - grey_img.at<uchar>(y-1,x-1) * 1.0
                    - grey_img.at<uchar>(y-1,x) * 2.0 - grey_img.at<uchar>(y-1,x+1) * 1.0);
            grad_img.ptr<uchar>(y, x)[0] =
                grad_img.ptr<uchar>(y, x)[1] =
                grad_img.ptr<uchar>(y, x)[2] =
                    255 * (std::norm((*gradient)[y][x])>40000);
        }
    }
    return gradient;
}
/*
void Image::clip(int& x, int& y){
    for(int i=0; i<10; ++i){
        for(int j=0; j<10; ++j){
            if(x+i<img.rows && y+j<img.cols && std::norm(((*gradient)[x+i])[x+j])>40000){
                x+=i;
                y+=j;
                return;
            }
            if(x-i>0 && y+j<img.cols && std::norm(((*gradient)[x+i])[x+j])>40000){
                x-=i;
                y+=j;
                return;
            }
            if(x+i<img.rows && y-j>0 && std::norm(((*gradient)[x+i])[x+j])>40000){
                x+=i;
                y-=j;
                return;
            }
            if(x-i>0 && y-j>0 && std::norm(((*gradient)[x+i])[x+j])>40000){
                x-=i;
                y-=j;
                return;
            }
        }
    }
}*/
void Image::clip(int &x, int &y) {
    std::deque<cv::Point> q;
    q.push_back(cv::Point(x, y));
    bool *v = new bool[h()*w()];
    memset(v, 0, h()*w()*sizeof(bool));
    while (!q.empty()) {
        cv::Point c = q[0];
        q.pop_front();
        v[I2(c.x, c.y)] = true;
        if (grad_img.ptr<uchar>(c.x, c.y)[0]) {
            x = c.x; y = c.y;
            delete v;
            return;
        }
        for (int i=0; i<8; i++) {
            int nx = c.x+di[i];
            int ny = c.y+dj[i];
            if (nx<0 || nx>=h()) continue;
            if (ny<0 || ny>=w()) continue;
            if (v[I2(nx, ny)]) continue;
            if (cv::norm(real_to_raw(nx, ny)-real_to_raw(x, y)) > 10) continue;
            q.push_back(cv::Point(nx, ny));
        }
    }
    delete v;
}
