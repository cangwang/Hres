//
// Created by asus on 2022/11/18.
//

#include "engineoption.h"

void EngineOption::setOptions(string options) {
    if (options.empty()) {
        HLOGE("option is null");
        return;
    }

    JSON_Value *root_value = json_parse_string(options.c_str());
    // 获取根类型 object/array
    JSON_Value_Type jsonValueType = json_value_get_type(root_value);
    if (jsonValueType == JSONObject) {
        JSON_Object *jsonObject = json_value_get_object(root_value);
        useVulkan = json_object_get_boolean(jsonObject, "useVulkan");
        transformType = json_object_get_string(jsonObject, "transformType");
    }
}
