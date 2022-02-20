#include <algorithm>
#include <cassert>
#include <chrono>
#include "BVH.hpp"

BVHAccel::BVHAccel(std::vector<Object*> p, int maxPrimsInNode,
                   SplitMethod splitMethod)
    : maxPrimsInNode(std::min(255, maxPrimsInNode)), splitMethod(splitMethod),
      primitives(std::move(p))
{
    //time_t start, stop;//���ʱ����㲻��ȷ����Ϊchrono
    //time(&start);
    auto start_time = std::chrono::steady_clock::now();
    if (primitives.empty())
        return;

    root = recursiveBuild(primitives);//�ݹ鹹��BVH

    auto end_time = std::chrono::steady_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    //time(&stop);
    //double diff = difftime(stop, start);
    //int hrs = (int)diff / 3600;
    //int mins = ((int)diff / 60) - (hrs * 60);
    //int secs = (int)diff - (hrs * 3600) - (mins * 60);

    /*printf(
        "\rBVH Generation complete: \nTime Taken: %i hrs, %i mins, %i secs\n\n",
        hrs, mins, secs);*/
    //int secs = microseconds / (1000 * 1000);
    //double msec = microseconds/1000 - secs*1000;
    //printf("\rBVH Generation complete: \nTime taken: %i secs, %.3f ms \n",secs,msec);
    printf("\rBVH Generation complete: \nTime taken: %.3f ms \n", microseconds/1000.0f);
}

BVHBuildNode* BVHAccel::recursiveBuild(std::vector<Object*> objects)
{
    BVHBuildNode* node = new BVHBuildNode();

    // Compute bounds of all primitives in BVH node
    Bounds3 bounds;
    for (int i = 0; i < objects.size(); ++i)//����ȫ��ͼԪ��bounds
        bounds = Union(bounds, objects[i]->getBounds());
    if (objects.size() == 1) {//����ÿ��Ҷ�ӽ���1��
        // Create leaf _BVHBuildNode_
        node->bounds = objects[0]->getBounds();
        node->object = objects[0];
        node->left = nullptr;
        node->right = nullptr;
        return node;
    }
    else if (objects.size() == 2) {
        node->left = recursiveBuild(std::vector{objects[0]});
        node->right = recursiveBuild(std::vector{objects[1]});

        node->bounds = Union(node->left->bounds, node->right->bounds);
        return node;
    }
    else {
        Bounds3 centroidBounds;//���ĵ�Bounds
        for (int i = 0; i < objects.size(); ++i)
            centroidBounds =
                Union(centroidBounds, objects[i]->getBounds().Centroid());
        int dim = centroidBounds.maxExtent();
        //������ά���Ͻ�������
        switch (dim) {
        case 0://��xά��
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().x <
                       f2->getBounds().Centroid().x;
            });
            break;
        case 1://��yά��
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().y <
                       f2->getBounds().Centroid().y;
            });
            break;
        case 2://��zά��
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().z <
                       f2->getBounds().Centroid().z;
            });
            break;
        }

        auto beginning = objects.begin();
        auto middling = objects.begin() + (objects.size() / 2);
        auto ending = objects.end();

        auto leftshapes = std::vector<Object*>(beginning, middling);
        auto rightshapes = std::vector<Object*>(middling, ending);

        assert(objects.size() == (leftshapes.size() + rightshapes.size()));

        node->left = recursiveBuild(leftshapes);
        node->right = recursiveBuild(rightshapes);

        node->bounds = Union(node->left->bounds, node->right->bounds);
    }

    return node;
}

Intersection BVHAccel::Intersect(const Ray& ray) const
{
    Intersection isect;
    if (!root)
        return isect;
    isect = BVHAccel::getIntersection(root, ray);
    return isect;
}

Intersection BVHAccel::getIntersection(BVHBuildNode* node, const Ray& ray) const
{

    // TODO Traverse the BVH to find intersection
    
    Intersection left_ins,right_ins;
    std::array<int,3> dirIsNeg;
    dirIsNeg[0] = int(ray.direction.x >= 0);
	dirIsNeg[1] = int(ray.direction.y >= 0);
	dirIsNeg[2] = int(ray.direction.z >= 0);

    //miss nodes.bbox
    bool flag = node->bounds.IntersectP(ray,ray.direction_inv,dirIsNeg);
    if (!flag) {
        return left_ins;
    } 
    //bbox��ray���ڽ��㣬��node is leaf���˴�Ĭ��ÿ��leaf����ֻ��һ����㣩����node�е�ÿ�����������
    if (!node->left && !node->right) return node->object->getIntersection(ray);
    left_ins=getIntersection(node->left, ray);//����box��
    right_ins=getIntersection(node->right, ray);//����box��
    return left_ins.distance < right_ins.distance ? left_ins: right_ins;//�����������
}