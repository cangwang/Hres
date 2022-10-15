//
// Created by zengjiale on 2022/10/14.
//

#include "imageoptionparams.h"

string ImageOptionParams::getName() {
    return std::string();
}

int ImageOptionParams::getType() {
    return 0;
}

string ImageOptionParams::getAddress() {
    return std::string();
}

int ImageOptionParams::getScaleType() {
    return 0;
}

float ImageOptionParams::getScaleRatio() {
    return 0;
}

float ImageOptionParams::getScaleWidth() {
    return 0;
}

float ImageOptionParams::getScaleHeight() {
    return 0;
}

bool ImageOptionParams::getAsync() {
    return false;
}
