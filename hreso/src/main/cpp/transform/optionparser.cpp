//
// Created by cangwang on 2022/10/16.
//

#include "optionparser.h"
#include <src/main/cpp/bean/imageoptionparams.h>

IOptions* OptionParser::parseOptions(string options, jobject op) {
    if (options.empty()) {
        HLOGE("options is empty");
        return nullptr;
    }

    char g_log_info[200];

    JSON_Value *root_value = json_parse_string(options.c_str());
    // 获取根类型 object/array
    JSON_Value_Type jsonValueType = json_value_get_type(root_value);
    if (jsonValueType == JSONObject) {
        JSON_Object *jsonObject = json_value_get_object(root_value);

        // type类型
        const int type = (int)json_object_get_number(jsonObject, "type");
        IOptions *option = nullptr;
        if (type == 1) {
            option = new ImageOptionParams();
        }
        option->setOptions(options, op);
        return option;
    } else {
        HLOGE("not json object");
    }
    return nullptr;
}