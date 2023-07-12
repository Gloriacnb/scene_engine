#ifndef SE_TRIGGER_H
#define SE_TRIGGER_H


#include "se_defines.h"
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

Trigger* makeNullTriggerHandle(void);

/**
 * @brief 配置触发器
 * @param trigger 
 * @param config 
 * @return int 
 */
SE_ERR configureTrigger(Trigger* trigger, const TriggerInfo* config, configResult* result);

// 删除触发器数据
SE_ERR deleteTriggerData(Trigger* trigger, const TriggerInfo* config);
/* 
    功能描述：删除触发器数据
    输入：
        - trigger：触发器实例
        - config：触发器配置信息
    输出：删除结果，0表示成功，-1表示失败
*/

/**
 * @brief 触发条件判定
 * 
 * @param trigger 
 * @param properties 
 * @param status 
 * @return SE_ERR 
 */
SE_ERR evaluateDeviceTriggerConditions(Trigger* trigger, const DevProperties* properties, TriggerStatus* status);
/* 
    功能描述：设备触发条件判定
    输入：
        - trigger：触发器实例
        - properties：E++全量属性
    输出：触发器状态，成功或失败
*/

//增加获取trigger对应scheduler device id

#ifdef __cplusplus
}
#endif
#endif /* TRIGGER_H */
