//
// Created by asus on 2022/12/7.
//

#ifndef HRES_FSRVULKANFILTER_H
#define HRES_FSRVULKANFILTER_H


#include <vulkan/filter/VulkanFilter.h>

using namespace std;
class FsrVulkanFilter : public VulkanFilter {
public:
    const char* fsrShader =
            "#version 450\n"
            "#extension GL_ARB_separate_shader_objects : enable\n"
            "#extension GL_ARB_shading_language_420pack : enable\n"
            "precision highp float;\n"
            "layout (binding = 0) uniform sampler2D uTexture;\n"
            "layout (location = 0) in vec2 v_TexCoordinate;\n"
            "layout (location = 0) out vec4 glFragColor;\n"
            "layout (push_constant) uniform fsr {\n"
            " float w;\n"
            " float h;\n"
            "} pushVals;\n"

            "void main() {\n"
            // Normalized pixel coordinates (from 0 to 1)
            "   vec2 uv = v_TexCoordinate;\n"

            // Time varying pixel color
            "   vec3 col = texture(uTexture, uv).xyz;\n"

            // CAS algorithm
            "    float max_g = col.y;\n"
            "    float min_g = col.y;\n"
            "    vec4 uvoff = vec4(1,0,1,-1)/vec4(pushVals.w, pushVals.w, pushVals.h, pushVals.h);\n"
            "    vec3 colw;\n"
            "    vec3 col1 = texture(uTexture, uv+uvoff.yw).xyz;\n"
            "    max_g = max(max_g, col1.y);\n"
            "    min_g = min(min_g, col1.y);\n"
            "    colw = col1;\n"
            "    col1 = texture(uTexture, uv+uvoff.xy).xyz;\n"
            "    max_g = max(max_g, col1.y);\n"
            "    min_g = min(min_g, col1.y);\n"
            "    colw += col1;\n"
            "    col1 = texture(uTexture, uv+uvoff.yz).xyz;\n"
            "    max_g = max(max_g, col1.y);\n"
            "    min_g = min(min_g, col1.y);\n"
            "    colw += col1;\n"
            "    col1 = texture(uTexture, uv-uvoff.xy).xyz;\n"
            "    max_g = max(max_g, col1.y);\n"
            "    min_g = min(min_g, col1.y);\n"
            "    colw += col1;\n"
            "    float d_min_g = min_g;\n"
            "    float d_max_g = 1.-max_g;\n"
            //    "    float A;\n"
            //    "    if (d_max_g < d_min_g) {\n"
            //    "        A = d_max_g / max_g;\n"
            //    "    } else {\n"
            //    "        A = d_min_g / max_g;\n"
            //    "    }\n"
            "    float A = step(d_min_g, d_max_g) / max_g;"
            "    A = sqrt(A);\n"
            "    A *= mix(-.125, -.2, 1.0);\n"
            "    vec3 col_out = (col + colw * A) / (1.+4.*A);\n"
            "    glFragColor = vec4(col_out,1);\n"
            "}";

    FsrVulkanFilter(): VulkanFilter() {
        pFragShader = fsrShader;
        pushConstant.resize(2);
        pushConstant[0] = w;
        pushConstant[1] = h;
    }

    virtual void setOption(IOptions *option) override;

private:
    float w;
    float h;
};


#endif //HRES_FSRVULKANFILTER_H
