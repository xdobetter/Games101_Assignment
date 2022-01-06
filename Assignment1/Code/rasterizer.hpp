//
// Created by goksu on 4/6/19.
//

#pragma once

#include "Triangle.hpp"
#include <algorithm>
#include <eigen3/Eigen/Eigen>
using namespace Eigen;

namespace rst {
enum class Buffers
{
    Color = 1,
    Depth = 2
};

inline Buffers operator|(Buffers a, Buffers b)
{
    return Buffers((int)a | (int)b);
}

inline Buffers operator&(Buffers a, Buffers b)
{
    return Buffers((int)a & (int)b);
}

enum class Primitive //图元
{
    Line,
    Triangle
};

/*
 * For the curious : The draw function takes two buffer id's as its arguments. //对于好奇的人:draw函数将两个缓冲区id作为它的参数。
 * These two structs make sure that if you mix up with their orders, the //这两个结构确保了如果混淆了它们的顺序，编译器将不会编译它。Aka:类型安全
 * compiler won't compile it. Aka : Type safety
 * */
struct pos_buf_id
{
    int pos_id = 0;
};

struct ind_buf_id
{
    int ind_id = 0;
};

class rasterizer
{
  public:
    rasterizer(int w, int h);
    pos_buf_id load_positions(const std::vector<Eigen::Vector3f>& positions);
    ind_buf_id load_indices(const std::vector<Eigen::Vector3i>& indices);

    void set_model(const Eigen::Matrix4f& m);
    void set_view(const Eigen::Matrix4f& v);
    void set_projection(const Eigen::Matrix4f& p);

    void set_pixel(const Eigen::Vector3f& point, const Eigen::Vector3f& color);

    void clear(Buffers buff);

    void draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, Primitive type);

    std::vector<Eigen::Vector3f>& frame_buffer() { return frame_buf; }

  private:
    void draw_line(Eigen::Vector3f begin, Eigen::Vector3f end);
    void rasterize_wireframe(const Triangle& t);

  private:
    Eigen::Matrix4f model;
    Eigen::Matrix4f view;
    Eigen::Matrix4f projection;

    std::map<int, std::vector<Eigen::Vector3f>> pos_buf; //每个pos的,buffer
    std::map<int, std::vector<Eigen::Vector3i>> ind_buf;//每个indice

    std::vector<Eigen::Vector3f> frame_buf;
    std::vector<float> depth_buf;
    int get_index(int x, int y); //?

    int width, height;

    int next_id = 0;
    int get_next_id() { return next_id++; }//?
};
} // namespace rst
