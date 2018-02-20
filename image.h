#ifndef IMAGE_H
#define IMAGE_H

#include <QPixmap>
#include <QLabel>

#include <core.hpp>

#include <cstring>
#include <cstdlib>

#define IMG 0
#define PIX 1
#define COST 2
#define PATH 3

typedef std::vector<std::vector<cv::Point>> Contours;
typedef std::vector<cv::Point> Contour;

class Image
{
public:
    bool has_data;
    bool has_seed;
    cv::Mat img;
    cv::Mat pixel_node;
    cv::Mat cost_graph;
    cv::Mat path_tree;
    double scale;
    QLabel *label;
    double *adj;
    int seed_x;
    int seed_y;
    int mode;
    int *pred;
    Contours fixed;
    Contours active;

    Image();
    ~Image();

    void open(const std::string&);
    void zoom_in();
    void zoom_out();
    void show_img();
    void show_pixel_node();
    void show_cost_graph();
    void show_path_tree();
    void show_min_path(int, int);
    void show_stored(cv::Mat, Contours&);
    void show_pending(cv::Mat, int, int);
    int h();
    int hs();
    int w();
    int ws();
    void set_label(QLabel*);
    void get_edges();
    void get_pixel_node();
    void get_cost_graph();
    void get_path_tree(int, int);
    cv::Point raw_to_real(int, int);
    cv::Point real_to_raw(int, int);
    void del_seed();
    void start_contour(int, int);
    void add_interm(int, int);
    void complete_contour();
    bool is_finish_contour(int, int);
};

extern Image *image;
extern cv::Mat filt_kernel[8];

#endif // IMAGE_H
