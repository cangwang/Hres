//
// Created by zengjiale on 2022/10/14.
//

#ifndef HRES_IMAGEHRESTRANSFORMER_H
#define HRES_IMAGEHRESTRANSFORMER_H

#include <transform/ioptions.h>
#include <list>
#include <filter/ifilter.h>


using namespace std;
class ImageHresTransformer {
public:
    ImageHresTransformer();
    ~ImageHresTransformer();
    void transformOption(IOptions* options);
    void release();

private:
    list<IFilter*> filterList;
};

#endif //HRES_IMAGEHRESTRANSFORMER_H
