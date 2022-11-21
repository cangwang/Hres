//
// Created by asus on 2022/11/8.
//

#ifndef HRES_VKDEFINE_H
#define HRES_VKDEFINE_H

struct Vertex {
    float pos[3];
    float uv[2];
};

struct UniformBufferObject {
    float projection[16];
    float rotation[16];
    float scale[16];
};

struct RGBBufferObject {
    float red;
    float green;
    float blue;
};

#endif //HRES_VKDEFINE_H
