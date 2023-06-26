#ifndef TRIGGER_H
#define TRIGGER_H

#include "se_scheduler.h"

typedef struct {
    // 触发器结构体定义
    // 触发设备ID
} Trigger;

// 触发器配置信息结构体
typedef struct {
    // 触发器配置信息字段
    // ...
} TriggerConfig;

// 触发器状态枚举
typedef enum {
    TRIGGER_STATUS_SUCCESS,  // 成功
    TRIGGER_STATUS_FAILURE   // 失败
} TriggerStatus;

/**
 * @brief 
 * @param trigger 
 * @param config 
 * @return int 
 */
int configureTrigger(Trigger* trigger, const TriggerConfig* config);

// 删除触发器数据
int deleteTriggerData(Trigger* trigger, const TriggerConfig* config);
/* 
    功能描述：删除触发器数据
    输入：
        - trigger：触发器实例
        - config：触发器配置信息
    输出：删除结果，0表示成功，-1表示失败
*/

// 设备触发条件判定
TriggerStatus evaluateDeviceTriggerConditions(const Trigger* trigger, const EppProperties* properties);
/* 
    功能描述：设备触发条件判定
    输入：
        - trigger：触发器实例
        - properties：E++全量属性
    输出：触发器状态，成功或失败
*/

#endif /* TRIGGER_H */
