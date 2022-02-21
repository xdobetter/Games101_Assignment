//
// Created by goksu on 2/25/20.
//

#include <fstream>
#include "Scene.hpp"
#include "Renderer.hpp"
#include <omp.h>

inline float deg2rad(const float& deg) { return deg * M_PI / 180.0; }//转为弧度

const float EPSILON = 0.00001;

// The main render function. This where we iterate over all pixels in the image,
// generate primary rays and cast these rays into the scene. The content of the
// framebuffer is saved to a file.
void Renderer::Render(const Scene& scene)
{
    std::vector<Vector3f> framebuffer(scene.width * scene.height);

    float scale = tan(deg2rad(scene.fov * 0.5));
    float imageAspectRatio = scene.width / (float)scene.height;
    Vector3f eye_pos(278, 273, -800);//origin
    int m = 0;

    // change the spp value to change sample ammount
    int spp = 100;//spp数
    int count=0;
    std::cout << "SPP: " << spp << "\n";
#pragma omp parallel for //简易并行，每个线程任务量为一个width
    for (uint32_t j = 0; j < scene.height; ++j) {
        //std::cout<<"j="<<j<<std::endl;
        auto idx=omp_get_thread_num();
        for (uint32_t i = 0; i < scene.width; ++i) {
            // generate primary ray direction
            //Uniformly choose N sample positions within the pixel
            //这里每次采样位置都是同样的位置
            float x = (2 * (i + 0.5) / (float)scene.width - 1) *
                      imageAspectRatio * scale;
            float y = (1 - 2 * (j + 0.5) / (float)scene.height) * scale;

            Vector3f dir = normalize(Vector3f(-x, y, 1));//这里x为什么取了一个-?
            for (int k = 0; k < spp; k++){//这里对每条光路都取同样的贡献权值
                framebuffer[j*scene.width+i] += scene.castRay(Ray(eye_pos, dir), 0) / spp;
                //framebuffer[m]+=scene.castRay(Ray(eye_pos,dir),0)/spp;
            }
            //m++;
        }
        if(idx==0){
            count++;
            //UpdateProgress(j / (float)scene.height);
            //std::cout<<"omp_get_num_threads()="<<omp_get_num_threads()<<"  count="<<count<<std::endl;
            //std::cout<<"count/(float)scene.height/(float)omp_get_num_threads()="<<count/((float)scene.height/(float)omp_get_num_threads())<<std::endl;
            UpdateProgress(count/((float)scene.height/(float)omp_get_num_threads()));//只通过线程0，处理进度条，通过统计线程0处理的任务数，来更新进度条
        }
    }
    UpdateProgress(1.f);
    // save framebuffer to file
    FILE* fp = fopen("binary.ppm", "wb");
    (void)fprintf(fp, "P6\n%d %d\n255\n", scene.width, scene.height);
    for (auto i = 0; i < scene.height * scene.width; ++i) {
        static unsigned char color[3];
//        std::cout<<"framebuffer[i].x="<<framebuffer[i].x<<std::endl;
//        std::cout<<"framebuffer[i].y="<<framebuffer[i].y<<std::endl;
//        std::cout<<"framebuffer[i].z="<<framebuffer[i].z<<std::endl;
        color[0] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].x), 0.6f));
        color[1] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].y), 0.6f));
        color[2] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].z), 0.6f));
        fwrite(color, 1, 3, fp);
    }
    fclose(fp);    
}
