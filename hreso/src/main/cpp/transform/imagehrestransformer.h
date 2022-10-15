//
// Created by zengjiale on 2022/10/14.
//

#ifndef HRES_IMAGEHRESTRANSFORMER_H
#define HRES_IMAGEHRESTRANSFORMER_H

#include <transform/ioptions.h>

class ImageHresTransformer {
public:
    ImageHresTransformer();
    ~ImageHresTransformer();
    void transformOption(IOptions* option);
    void release();
};


#endif //HRES_IMAGEHRESTRANSFORMER_H
