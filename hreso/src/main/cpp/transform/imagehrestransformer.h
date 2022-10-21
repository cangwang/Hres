//
// Created by zengjiale on 2022/10/14.
//

#ifndef HRES_IMAGEHRESTRANSFORMER_H
#define HRES_IMAGEHRESTRANSFORMER_H

#include <transform/ioptions.h>
#include <list>
#include <filter/ifilter.h>
#include <filter/simplefilter.h>


using namespace std;
class ImageHresTransformer {
public:
    ImageHresTransformer();
    ~ImageHresTransformer();
    void transformOption(IOptions* options);
    void transform();
    void release();

private:
    list<IFilter*> filterList;
    shared_ptr<IOptions> option;
};

#endif //HRES_IMAGEHRESTRANSFORMER_H
