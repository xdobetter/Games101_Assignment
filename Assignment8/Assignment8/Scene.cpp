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
//Vector3f Scene::shading(const Intersection& p_inter, Vector3f wo) const {//wo入射方向，从p点向外射出
//    Vector3f L_dir = 0.0, L_indir = 0.0;
//    //###sample light###
//	Intersection x_inter;
//	//float pdf_light = get_random_float();//错误，不是自己生成的，是通过sampleLight函数来获得该值
//	float pdf_light = 0.0f;
//	sampleLight(x_inter, pdf_light);//sample light
//	//Get x, ws , nn , emit from x_inter
//	Vector3f x = x_inter.coords;
//	Vector3f ws = x - p_inter.coords;
//	Vector3f nn = x_inter.normal;
//	Vector3f n = p_inter.normal;
//	Vector3f emit = x_inter.emit;
//    wo= wo.normalized();//wo是射入p点
//    ws= ws.normalized();//ws是从p点射出
//	Ray p2x_ray(p_inter.coords, ws);//生成一根打向光源的射线，判断是否会与光源相交
//	Intersection temp_inter = this->intersect(p2x_ray);
//	if (abs(distance(temp_inter.coords, p_inter.coords) - distance(x,p_inter.coords)) < EPSILON) {//如果之间没有被遮挡，计算直接光照的结果
//		Vector3f eval=p_inter.m->eval(ws, wo, n);
//        L_dir = emit * eval * dotProduct(ws, n) * dotProduct(-ws, nn) / std::pow(distance(p_inter.coords, x),2) / (pdf_light+EPSILON);
//	}
//	//###sample hamisphere###
//	float p_rr = get_random_float();//Test Russian Roulette with probability RussianRoulette
//	if (p_rr < RussianRoulette) {
//		Vector3f wi = p_inter.m->sample(-wo, n);//对该点sample一个方向,-wo表示射入到p点的方向,wi则表示从p点打出
//		wi=wi.normalized();
//        Ray ray(p_inter.coords, wi);
//		Intersection inter = this->intersect(ray);
//		Vector3f temp_vec = Vector3f();
//		if (inter.happened && inter.emit == temp_vec) {//如果能够有交点，且相交的不是光源
//            Vector3f eval=p_inter.m->eval(wi,wo,n);
//			L_indir = eval * shading(inter, -wi) * dotProduct(n, wi) / (p_inter.m->pdf(wi, wo, n)+EPSILON) / RussianRoulette;//这里wi方向需要取反，表示从inter点发出
//		}
//	}
//	return L_indir+L_dir;
//}

//原来的shading函数
Vector3f Scene::shading( const Intersection &p, Vector3f wo) const {//着色函数
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
        L_dir=x.emit*p.m->eval(wi.normalized(),wo.normalized(),Np)* dotProduct(Np,wi.normalized())* dotProduct(Nx,-wi.normalized())/ std::pow(distance(p.coords,x.coords),2)/pdf_light;
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
            L_indir=p.m->eval(wi.normalized(),wo.normalized(),Np)* shading(hit,-wi.normalized())* dotProduct(Np,wi.normalized())/p.m->pdf(wi.normalized(),wo.normalized(),Np)/RussianRoulette;
        }
    }
    return L_dir+L_indir;
}


// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    // TO DO Implement Path Tracing Algorithm here
    //计算当前交点
    Intersection inter = this->intersect(ray);
    //根据交点结果进行shading
    if (inter.happened) {
        Vector3f wo=-ray.direction;
        return inter.emit+shading(inter,wo);//Le+Lr
    }
    else {
        return Vector3f();
    } 
}
   