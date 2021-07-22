//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"


void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    // TO DO Implement Path Tracing Algorithm here
    Intersection cur_hit=this->intersect(ray);//该光线与场景求交
    if(cur_hit.happened){//存在交点，则对该点进行着色
        Vector3f wo= -normalize(ray.direction);
        return cur_hit.emit+shade(cur_hit,wo);
    }else{
        return Vector3f();
    }
}
Vector3f Scene::shade( Intersection &p, Vector3f wo) const {//着色函数
    Vector3f L_dir=0.0,L_indir=0.0;
    Intersection x;//光源上的采样点
    float ep=0.001;
    float pdf_light=0.0;
    sampleLight(x,pdf_light);//对光源采样
    Ray p_to_x(p.coords,x.coords-p.coords);//生成从p点发射到光源采样点x的光源
    Intersection temp_hit= this->intersect(p_to_x);//从p点发射一根光线到光源采样点x
    if(abs(distance(p.coords,temp_hit.coords)- distance(p.coords,x.coords))<ep){//如果p点到光源采样点x之间没有其他物体
        Vector3f wi=p_to_x.direction;
        Vector3f Np=p.normal;
        Vector3f Nx=x.normal;
        L_dir=x.emit*p.m->eval(wi.normalized(),wo.normalized(),Np)* dotProduct(Np,wi.normalized())* dotProduct(Nx,-wi.normalized())/ distance_2(p.coords,x.coords)/pdf_light;
    }
    float p_rr=get_random_float();
    if(p_rr<RussianRoulette) {
        Vector3f wi=p.m->sample(-wo.normalized(),p.normal);//在半球上均匀采样
        Ray ray(p.coords,wi.normalized());
        Intersection hit=this->intersect(ray);
        Vector3f temp;
        if(hit.happened&&hit.emit==temp){
            //to do
            Vector3f Np=p.normal;
            L_indir=p.m->eval(wi.normalized(),wo.normalized(),Np)* shade(hit,-wi.normalized())* dotProduct(Np,wi.normalized())/p.m->pdf(wi.normalized(),wo.normalized(),Np)/RussianRoulette;
        }
    }
    return L_dir+L_indir;
}