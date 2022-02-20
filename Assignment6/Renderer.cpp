//
// Created by goksu on 2/25/20.
//

#include <fstream>
#include "Scene.hpp"
#include "Renderer.hpp"


inline float deg2rad(const float& deg) { return deg * M_PI / 180.0; }

const float EPSILON = 0.00001;

// The main render function. This where we iterate over all pixels in the image,
// generate primary rays and cast these rays into the scene. The content of the
// framebuffer is saved to a file.
void Renderer::Render(const Scene& scene)
{
    std::vector<Vector3f> framebuffer(scene.width * scene.height);

    float scale = tan(deg2rad(scene.fov * 0.5));
    float imageAspectRatio = scene.width / (float)scene.height;
    Vector3f eye_pos(-80, 150, 10);
    //Vector3f eye_pos(0, 0, 10);
    //Vector3f eye_pos(0, 0, 0);
    //Vector3f eye_pos(-1, 20, 20);
    int m = 0;
    for (uint32_t j = 0; j < scene.height; ++j) {
        for (uint32_t i = 0; i < scene.width; ++i) {
			// generate primary ray direction
			float x;
			float y;
			// TODO: Find the x and y positions of the current pixel to get the direction
			// vector that passes through it.
			// Also, don't forget to multiply both of them with the variable *scale*, and
			// x (horizontal) variable with the *imageAspectRatio*            

			//从初始的raster space变化到NDC(Normalize Device Coordinates) space
			float pixel_ndc_x = (i + 0.5) / scene.width;
			float pixel_ndc_y = (j + 0.5) / scene.height;

			//从NDC space 变化到 Screen space
			float pixel_screen_x = pixel_ndc_x * 2 - 1;
			float pixel_screen_y = 1 - pixel_ndc_y * 2;

			//从Screen space变化到Camera space
			float pixel_camera_x = pixel_screen_x * imageAspectRatio * scale;
			float pixel_camera_y = pixel_screen_y * scale;

			//camera space的坐标即最终的坐标（如果相机有进行平移变换的话，需要再乘上变化矩阵）
			x = pixel_camera_x;
			y = pixel_camera_y;
            Vector3f origin = eye_pos;
			//Vector3f dir = Vector3f(x, y, -1)-origin; // Don't forget to normalize this direction!，这里理论上方向应该是这么算的，但发现这么加上是错的
            Vector3f dir = Vector3f(x, y, -1);
            dir = normalize(dir);
            Ray ray(origin,dir);//生成光线
            framebuffer[m++] = scene.castRay(ray, 0);//朝该位置射出光线,0递归深度
        }
        UpdateProgress(j / (float)scene.height);
    }
    UpdateProgress(1.f);

    // save framebuffer to file
    FILE* fp = fopen("binary.ppm", "wb");
    (void)fprintf(fp, "P6\n%d %d\n255\n", scene.width, scene.height);
    for (auto i = 0; i < scene.height * scene.width; ++i) {
        static unsigned char color[3];
        color[0] = (unsigned char)(255 * clamp(0, 1, framebuffer[i].x));
        color[1] = (unsigned char)(255 * clamp(0, 1, framebuffer[i].y));
        color[2] = (unsigned char)(255 * clamp(0, 1, framebuffer[i].z));
        fwrite(color, 1, 3, fp);
    }
    fclose(fp);    
}
