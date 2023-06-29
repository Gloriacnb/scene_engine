#ifndef TRIGGER_H
#define TRIGGER_H

#include "se_defines.h"


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

/**
 * @brief 设备触发条件判定
 * @param trigger 
 * @param properties 
 * @return TriggerStatus 
 */
TriggerStatus evaluateDeviceTriggerConditions(Trigger* trigger, const DevProperties* properties);
/* 
    功能描述：设备触发条件判定
    输入：
        - trigger：触发器实例
        - properties：E++全量属性
    输出：触发器状态，成功或失败
*/

//增加获取trigger对应scheduler device id


#endif /* TRIGGER_H */
