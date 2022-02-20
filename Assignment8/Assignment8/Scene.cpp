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

void Scene::sampleLight(Intersection &pos, float &pdf) const//采样光源
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
   
	auto shading = [&](const Intersection& p_inter, const Vector3f& wo) {
        Vector3f L_dir, L_indir;
		//###sample light###
		Intersection inter;
		//float pdf_light = get_random_float();//错误，不是自己生成的，是通过sampleLight函数来获得该值
        float pdf_light = 0.0f;
		sampleLight(inter, pdf_light);
		//Get x, ws , N , emit from inter
		Vector3f x = inter.coords;
		Vector3f ws = normalize(x - p_inter.coords);
        Vector3f nn = inter.normal;
		Vector3f n = p_inter.normal;
		Vector3f emit = inter.emit;
		Ray p2x_ray(p_inter.coords, normalize(x - p_inter.coords));//生成一根打向光源的射线，判断是否会与光源相交
		Intersection temp_inter = this->intersect(p2x_ray);
		if (distance(temp_inter.coords, p_inter.coords) - inter.distance < 10e-5) {//如果之间没有被遮挡，计算直接光照的结果
            L_dir += emit * p_inter.m->eval(ws, wo, n) * dotProduct(ws, n) * dotProduct(ws, nn) / distance(p_inter.coords, x) / pdf_light;
		}
        //###sample hamisphere###
        //Test Russian Roulette with probability RussianRoulette
        float p_rr = get_random_float();
        if (p_rr > RussianRoulette) return Vector3f();
        Vector3f wi = p_inter.m->sample(wo, n);//对该点sample一个方向


		return Vector3f();
	};

    //计算当前交点
    Intersection inter = this->intersect(ray);
    //根据交点结果进行shading
    if (inter.happened) {
        return shading(inter,ray.direction);
    }
    else {
        return Vector3f();
    } 
}
