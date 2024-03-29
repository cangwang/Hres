//
// Created by cangwang on 2022/4/18.
//

#include <bean/pointrect.h>

#ifndef YYEVA_TEXCOORDSUTIL_H
#define YYEVA_TEXCOORDSUTIL_H

class TexCoordsUtil {
public:

    static float* create(int width, int height, PointRect* rect, float* array);

    static float* rotate90(float* array);

};

#endif //YYEVA_TEXCOORDSUTIL_H
