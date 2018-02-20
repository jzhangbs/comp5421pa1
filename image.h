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
typedef std::vector<std::vector<std::complex<double>>> Gradient;

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
    int start_seed_x;
    int start_seed_y;
    int mode;
    int *pred;
    Contours fixed;
    Contours active;
    bool seed_snap_;
    Gradient* gradient;

    Image();
    ~Image();

    void act_open(const std::string&);
    void act_zoom_in();
    void act_zoom_out();
    void show_img();
    void show_pixel_node();
    void show_cost_graph();
    void show_path_tree();
    void show_min_path(int=-1, int=-1);
    void draw_stored(cv::Mat, Contours&);
    void draw_pending(cv::Mat, int, int);
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
    void act_del_seed();
    void act_start_contour(int, int);
    void act_add_interm(int, int);
    void act_complete_contour();
    bool is_finish_contour(int, int);
    cv::Point get_start_seed();
    void seed_snap();
    Gradient* get_gradient(int, int);
    void clip(int&, int&);
};

extern Image *image;
extern cv::Mat filt_kernel[8];

#endif // IMAGE_H
