//
// Created by asus on 2022/10/16.
//

#include "optionparser.h"
#include <transform/imageoptionparams.h>

IOptions* OptionParser::parseOptions(string options) {
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

        //解析descript
        const int type = (int)json_object_get_number(jsonObject, "type");
        IOptions *option = nullptr;
        if (type == 1) {
            option = new ImageOptionParams();
        }
        option->setOptions(options);
        return option;
    } else {
        HLOGE("not json object");
    }
    return nullptr;
}