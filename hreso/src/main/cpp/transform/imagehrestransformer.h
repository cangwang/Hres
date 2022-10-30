//
// Created by cangwang on 2022/10/14.
//

#ifndef HRES_IMAGEHRESTRANSFORMER_H
#define HRES_IMAGEHRESTRANSFORMER_H

#include <transform/ioptions.h>
#include <list>
#include <filter/filtercontroller.h>
#include <filter/filterlistener.h>

using namespace std;
class ImageHresTransformer {
public:
    ImageHresTransformer();
    ~ImageHresTransformer();
    void setWindow(ANativeWindow* window);
    void transformOption(IOptions* options);
    void setListener(FilterListener* listener);
    void transform();
    void release();

private:
    shared_ptr<FilterController> filterController;

    shared_ptr<IOptions> option;
};

#endif //HRES_IMAGEHRESTRANSFORMER_H
