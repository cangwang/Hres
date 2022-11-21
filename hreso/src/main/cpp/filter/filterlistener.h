//
// Created by zengjiale on 2022/10/28.
//

#ifndef HRES_FILTERLISTENER_H
#define HRES_FILTERLISTENER_H

#include <src/main/cpp/bean/ioptions.h>

class FilterListener {
public:
    virtual void filterRenderComplete(IOptions* options) = 0;
    virtual void filterRenderFail(IOptions* options, string errorMsg) = 0;
};


#endif //HRES_FILTERLISTENER_H
