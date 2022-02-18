//
// Created by LEI XU on 5/16/19.
//

#ifndef RAYTRACING_BVH_H
#define RAYTRACING_BVH_H

#include <atomic>
#include <vector>
#include <memory>
#include <ctime>
#include "Object.hpp"
#include "Ray.hpp"
#include "Bounds3.hpp"
#include "Intersection.hpp"
#include "Vector.hpp"

struct BVHBuildNode;
// BVHAccel Forward Declarations
struct BVHPrimitiveInfo;

// BVHAccel Declarations
inline int leafNodes, totalLeafNodes, totalPrimitives, interiorNodes;
class BVHAccel {

public:
    // BVHAccel Public Types
    enum class SplitMethod { NAIVE, SAH };

    // BVHAccel Public Methods
    BVHAccel(std::vector<Object*> p, int maxPrimsInNode = 1, SplitMethod splitMethod = SplitMethod::NAIVE);//默认为Naive方法
    Bounds3 WorldBound() const;//
    ~BVHAccel();

    Intersection Intersect(const Ray &ray) const;//计算具体的交点
    Intersection getIntersection(BVHBuildNode* node, const Ray& ray)const;//获取求交信息
    bool IntersectP(const Ray &ray) const;//判断是否与ray求交
    BVHBuildNode* root;//根节点

    // BVHAccel Private Methods
    BVHBuildNode* recursiveBuild(std::vector<Object*>objects);//?

    // BVHAccel Private Data
    const int maxPrimsInNode;//每个节点含有的图元数
    const SplitMethod splitMethod;//划分方式
    std::vector<Object*> primitives;//图元
};

struct BVHBuildNode {//BVH的结点
    Bounds3 bounds;
    BVHBuildNode *left;
    BVHBuildNode *right;
    Object* object;

public:
    int splitAxis=0, firstPrimOffset=0, nPrimitives=0;
    // BVHBuildNode Public Methods
    BVHBuildNode(){
        bounds = Bounds3();
        left = nullptr;right = nullptr;
        object = nullptr;
    }
};




#endif //RAYTRACING_BVH_H
