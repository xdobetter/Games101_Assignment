#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

constexpr double MY_PI = 3.1415926;


Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1,
        -eye_pos[2], 0, 0, 0, 1;

    view = translate * view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.
    float rad_angle=rotation_angle*MY_PI/180;//注意变为弧度制
    model<< cos(rad_angle),-sin(rad_angle),0,0,
         sin(rad_angle), cos(rad_angle),0,0,
         0,0,1,0,
         0,0,0,1;
    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{

    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.

    float rad_angle=eye_fov*MY_PI/180;//注意变为弧度制
    float t=tan(rad_angle/2)*zNear,b=-t;
    float r=t*aspect_ratio,l=-r;
    float n=-zNear,f=-zFar;//注意zNear,zFar这里给的初始是距离值，因此需要对其取负数

    //从perspective转换到orth
    Eigen::Matrix4f pers_to_orth=Eigen::Matrix4f::Identity();
    pers_to_orth<<n,0,0,0,
            0,n,0,0,
            0,0,n+f,-n*f,
            0,0,1,0;


    //orth变换
    //平移
    Eigen::Matrix4f orth_translate;
    orth_translate<<1,0,0,-(l+r)/2,
                    0,1,0,-(t+b)/2,
                    0,0,1,-(n+f)/2,
                    0,0,0,1;

    //缩放
    Eigen::Matrix4f orth=Eigen::Matrix4f::Identity();
    orth<<2/(r-l),0,0,0,
            0,2/(t-b),0,0,
            0,0,2/(n-f),0,
            0,0,0,1;


    projection=orth*orth_translate*pers_to_orth;//注意相乘的顺序

    return projection;
}

inline float clamp(const float&lo,const float&hi,const float &v){
    return std::max(lo,std::min(hi,v));
}

int main(int argc, const char** argv)
{
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.ppm";

    if (argc >= 2) {
        command_line = true;
        angle = std::stof(argv[1]); // -r by default
    }
    int width=700,height=700;
    rst::rasterizer r(width, height);

    Eigen::Vector3f eye_pos = {0, 0, 5};

    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);
        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));
        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        std::vector<Eigen::Vector3f > frame_buffer=r.frame_buffer();
        FILE* fp= fopen(filename.c_str(),"wb");//存储结果
        (void) fprintf(fp,"P6\n%d %d\n 255 \n",width,height);
        for(auto i=0;i<width*height;++i){
            static unsigned char color[3];
            //if(frame_buffer[i].x()||frame_buffer[i].y()||frame_buffer[i].z()){std::cout<<"i= "<<i;}
            color[0]=(char)(255* clamp(0,1,frame_buffer[i].x()));
            color[1]=(char)(255* clamp(0,1, frame_buffer[i].y()));
            color[2]=(char)(255* clamp(0,1,frame_buffer[i].z()));
            fwrite(color,sizeof(char),3,fp);
        }
      fclose(fp);
        return 0;


    return 0;
}
