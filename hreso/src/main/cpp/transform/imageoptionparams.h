//
// Created by zengjiale on 2022/10/14.
//

#ifndef HRES_IMAGEOPTIONPARAMS_H
#define HRES_IMAGEOPTIONPARAMS_H

#include <transform/ioptions.h>

using namespace std;
class ImageOptionParams : public IOptions {
public:
    string getName() override;

    int getType() override;

    string getAddress() override;

    int getScaleType() override;

    float getScaleRatio() override;

    float getScaleWidth() override;

    float getScaleHeight() override;

    bool getAsync() override;
};


#endif //HRES_IMAGEOPTIONPARAMS_H
