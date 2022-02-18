//
// Created by LEI XU on 5/16/19.
//

#ifndef RAYTRACING_BOUNDS3_H
#define RAYTRACING_BOUNDS3_H
#include "Ray.hpp"
#include "Vector.hpp"
#include <limits>
#include <array>

class Bounds3 //Bounding Box
{
  public:
    Vector3f pMin, pMax; // two points to specify the bounding box
    Bounds3()
    {
        double minNum = std::numeric_limits<double>::lowest();
        double maxNum = std::numeric_limits<double>::max();
        pMax = Vector3f(minNum, minNum, minNum);
        pMin = Vector3f(maxNum, maxNum, maxNum);
    }
    Bounds3(const Vector3f p) : pMin(p), pMax(p) {}
    Bounds3(const Vector3f p1, const Vector3f p2)
    {
        pMin = Vector3f(fmin(p1.x, p2.x), fmin(p1.y, p2.y), fmin(p1.z, p2.z));
        pMax = Vector3f(fmax(p1.x, p2.x), fmax(p1.y, p2.y), fmax(p1.z, p2.z));
    }

    Vector3f Diagonal() const { return pMax - pMin; } //对角矩阵，算距离
    int maxExtent() const//求在哪个维度上值会最大
    {
        Vector3f d = Diagonal();
        if (d.x > d.y && d.x > d.z)
            return 0;
        else if (d.y > d.z)
            return 1;
        else
            return 2;
    }

    double SurfaceArea() const
    {
        Vector3f d = Diagonal();
        return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);//x*y+x*z+y*z为3个面面积，再*2
    }

    Vector3f Centroid() { return 0.5 * pMin + 0.5 * pMax; } //包围盒中心点
    Bounds3 Intersect(const Bounds3& b)//包围盒求并
    {
        return Bounds3(Vector3f(fmax(pMin.x, b.pMin.x), fmax(pMin.y, b.pMin.y),
                                fmax(pMin.z, b.pMin.z)),
                       Vector3f(fmin(pMax.x, b.pMax.x), fmin(pMax.y, b.pMax.y),
                                fmin(pMax.z, b.pMax.z)));
    }

    Vector3f Offset(const Vector3f& p) const
    {
        Vector3f o = p - pMin;
        if (pMax.x > pMin.x)
            o.x /= pMax.x - pMin.x;
        if (pMax.y > pMin.y)
            o.y /= pMax.y - pMin.y;
        if (pMax.z > pMin.z)
            o.z /= pMax.z - pMin.z;
        return o;
    }

    bool Overlaps(const Bounds3& b1, const Bounds3& b2)//判断两个包围盒是否有交集
    {
        bool x = (b1.pMax.x >= b2.pMin.x) && (b1.pMin.x <= b2.pMax.x);
        bool y = (b1.pMax.y >= b2.pMin.y) && (b1.pMin.y <= b2.pMax.y);
        bool z = (b1.pMax.z >= b2.pMin.z) && (b1.pMin.z <= b2.pMax.z);
        return (x && y && z);
    }

    bool Inside(const Vector3f& p, const Bounds3& b)//判断p是否在包围盒内部
    {
        return (p.x >= b.pMin.x && p.x <= b.pMax.x && p.y >= b.pMin.y &&
                p.y <= b.pMax.y && p.z >= b.pMin.z && p.z <= b.pMax.z);
    }
    inline const Vector3f& operator[](int i) const//重载操作符
    {
        return (i == 0) ? pMin : pMax;
    }

    inline bool IntersectP(const Ray& ray) const;
};



inline bool Bounds3::IntersectP(const Ray& ray) const//与包围盒求交
{
    // invDir: ray direction(x,y,z), invDir=(1.0/x,1.0/y,1.0/z), use this because Multiply is faster that Division
    // dirIsNeg: ray direction(x,y,z), dirIsNeg=[int(x>0),int(y>0),int(z>0)], use this to simplify your logic
    // TODO test if ray bound intersects
    //待完成部分：ray和包围盒求交
    float x_tmin, x_tmax, y_tmin, y_tmax, z_tmin, z_tmax;
    Vector3f px_min(pMin.x, 0, 0);
    Vector3f dx(ray.direction.x, 0, 0);
    x_tmin = dotProduct((px_min - ray.origin),dx)/dotProduct(dx,dx);
    Vector3f px_max(pMax.x, 0, 0);
    x_tmax = dotProduct((px_max - ray.origin),dx)/dotProduct(dx,dx);

    Vector3f py_min(pMin.y, 0, 0);
    Vector3f dy(ray.direction.y, 0, 0);
    y_tmin = dotProduct((py_min - ray.origin), dy) / dotProduct(dy, dy);
    Vector3f py_max(pMax.y, 0, 0);
    y_tmax = dotProduct((py_max - ray.origin), dy) / dotProduct(dy, dy);

    Vector3f pz_min(pMin.z, 0, 0);
    Vector3f dz(ray.direction.z, 0, 0);
    z_tmin = dotProduct((pz_min - ray.origin), dz) / dotProduct(dz, dz);
    Vector3f pz_max(pMax.z, 0, 0);
    z_tmax = dotProduct((pz_max - ray.origin), dz) / dotProduct(dz, dz);

    float tnear, tfar;
    tnear = std::max(x_tmin, std::max(y_tmin, z_tmin));
    tfar = std::min(x_tmax, std::min(y_tmax, z_tmax));

    if (tnear < tfar && tfar >= 0) return true;
    return false;
}

inline Bounds3 Union(const Bounds3& b1, const Bounds3& b2)//两个包围盒求并
{
    Bounds3 ret;
    ret.pMin = Vector3f::Min(b1.pMin, b2.pMin);
    ret.pMax = Vector3f::Max(b1.pMax, b2.pMax);
    return ret;
}

inline Bounds3 Union(const Bounds3& b, const Vector3f& p)//包围盒和点求并
{
    Bounds3 ret;
    ret.pMin = Vector3f::Min(b.pMin, p);
    ret.pMax = Vector3f::Max(b.pMax, p);
    return ret;
}

#endif // RAYTRACING_BOUNDS3_H
