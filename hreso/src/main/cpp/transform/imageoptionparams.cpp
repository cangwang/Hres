//
// Created by cangwang on 2022/10/14.
//

#include "imageoptionparams.h"


void ImageOptionParams::setOptions(string options, jobject op) {
    if (options.empty()) {
        HLOGE("option is null");
        return;
    }
    this->options = options;
    char g_log_info[200];

    JSON_Value *root_value = json_parse_string(options.c_str());
    // 获取根类型 object/array
    JSON_Value_Type jsonValueType = json_value_get_type(root_value);
    if (jsonValueType == JSONObject) {
        JSON_Object *jsonObject = json_value_get_object(root_value);
        //解析descript
        type = (int)json_object_get_number(jsonObject, "type");
        name = json_object_get_string(jsonObject, "name");
        filterType = json_object_get_string(jsonObject, "filterType");
        address = json_object_get_string(jsonObject, "address");
        saveAddress = json_object_get_string(jsonObject, "saveAddress");
        scaleType = (int)json_object_get_number(jsonObject, "type");
        scaleRatio = (float)json_object_get_number(jsonObject, "scaleRatio");
        scaleWidth = (int)json_object_get_number(jsonObject, "scaleWidth");
        scaleHeight = (int)json_object_get_number(jsonObject, "scaleHeight");
        front = json_object_get_boolean(jsonObject, "front");
        async = json_object_get_boolean(jsonObject, "async");
    }
    this->op = op;
}