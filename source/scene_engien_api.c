#include "scene_engine_api.h"
#include <stdlib.h>

se_triger_config_data* getTrigerConfig(const se_device_info* info, const se_presetting_data* pd) {
    se_triger_config_data* result = malloc(sizeof(se_triger_config_data));
    /**
     * 解析预配置数据
     * 查询模板数据
     * 找到触发器配置数据
    */
    result->triger_devId = info->devId;
    return result;
}
