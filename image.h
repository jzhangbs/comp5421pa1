#ifndef IMAGE_H
#define IMAGE_H

#include <QPixmap>
#include <QLabel>

#include <core.hpp>
#include <cstring>

#define IMG 0
#define PIX 1
#define COST 2
#define PATH 3

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
    int h();
    int hs();
    int w();
    int ws();
    void set_label(QLabel*);
    void get_edges();
    void get_pixel_node();
    void get_cost_graph();
    void get_path_tree(int, int);
    cv::Point get_raw_pos(int, int);
};

extern Image *image;
extern cv::Mat filt_kernel[8];

#endif // IMAGE_H
