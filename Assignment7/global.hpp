#pragma once
#include <iostream>
#include <cmath>
#include <random>

#undef M_PI
#define M_PI 3.141592653589793f

extern const float  EPSILON;
const float kInfinity = std::numeric_limits<float>::max();

inline float clamp(const float &lo, const float &hi, const float &v)//???
{ return std::max(lo, std::min(hi, v)); }

inline  bool solveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1)//求解二次方程式
{
    float discr = b * b - 4 * a * c;
    if (discr < 0) return false;
    else if (discr == 0) x0 = x1 = - 0.5 * b / a;
    else {
        float q = (b > 0) ?
                  -0.5 * (b + sqrt(discr)) :
                  -0.5 * (b - sqrt(discr));
        x0 = q / a;
        x1 = c / q;
    }
    if (x0 > x1) std::swap(x0, x1);
    return true;
}

inline float get_random_float()
{
    std::random_device dev;//标准把随机数抽象成随机数引擎和分布两部分.引擎用来产生随机数,分布产生特定分布的随机数
    std::mt19937 rng(dev());//mt19937能够对随机数的范围无限制
    std::uniform_real_distribution<float> dist(0.f, 1.f); // distribution in range [1, 6] 平均分布，感觉这里写错了吧，应该写成[0,1]吧

    return dist(rng);
}

inline void UpdateProgress(float progress)//进度条
{
    int barWidth = 70;

    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();//清空缓冲区
};
