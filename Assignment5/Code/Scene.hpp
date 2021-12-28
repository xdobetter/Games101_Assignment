#pragma once

#include <vector>
#include <memory>
#include "Vector.hpp"
#include "Object.hpp"
#include "Light.hpp"

class Scene
{
public:
    // setting up options
    int width = 1280;
    int height = 960;
    double fov = 90;
    //Vector3f backgroundColor = Vector3f(0.235294, 0.67451, 0.843137); //背景色
    Vector3f backgroundColor = Vector3f(0.9, 0.67451, 0.843137);//修改后的背景色
    int maxDepth = 5;//最大深度
    float epsilon = 0.00001; //偏差量

    Scene(int w, int h) : width(w), height(h)
    {}

    void Add(std::unique_ptr<Object> object) { objects.push_back(std::move(object)); }
    void Add(std::unique_ptr<Light> light) { lights.push_back(std::move(light)); }

    [[nodiscard]] const std::vector<std::unique_ptr<Object> >& get_objects() const { return objects; } //当觉得返回值是非常有意义，需要调用者重视并处理的，使用该属性，如果返回值不重要，就不用这个属性
    [[nodiscard]] const std::vector<std::unique_ptr<Light> >&  get_lights() const { return lights; } //???

private:
    // creating the scene (adding objects and lights)
    std::vector<std::unique_ptr<Object> > objects;
    std::vector<std::unique_ptr<Light> > lights;
};