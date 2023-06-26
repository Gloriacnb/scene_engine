#ifndef SE_SCHEDULER_H
#define SE_SCHEDULER_H

#include "se_executor.h"
#include "se_trigger.h"
#include "se_defines.h"

// 设备信息结构体
typedef struct {
    // 设备信息字段
    // devID
    // typeid
    // devAbility
    // 场景模板ID
    // 场景模板版本
    // 场景规则ID
    // 动作ID 或 触发条件ID
} DeviceInfo;

// 删除结果结构体
typedef struct {
    // 删除结果字段
    // ...
} DeletionResult;

// 同步数据列表结构体
typedef struct {
    // 同步数据列表字段
    // ...
} SyncDataList;

typedef struct {
    // 调度器结构体定义
    // ...
} Scheduler;

typedef struct {
    //触发器同步数据
} TriggerInfo;

typedef struct {
    //执行器同步数据
    //optype
    //role
    //
} ExecutorInfo;

typedef struct 
{
    /* data */
}templateInfo;

/**
 * @brief       从电脑版预置文件中加载场景模板文件
 * @param[out]  scheduler 调度器实例
 * @param       templateFile 模板文件路径
 */
SE_ERR loadSceneTemplateFile(Scheduler* scheduler, templateInfo* templateFile);


/**
 * @brief 发起设备配对
 * @param scheduler 
 * @param deviceInfo 
 * @param[out] executorInfo 
 * @return SE_ERR 
 */
SE_ERR pairExecutorDevice(Scheduler* scheduler, const DeviceInfo* deviceInfo, ExecutorInfo* executorInfo);

/**
 * @brief 执行器配置结果通知
 * @param scheduler 
 * @param configResult 
 * @return SE_ERR 
 */
void executorConfigResultNotification(Scheduler* scheduler, const configResult* configResult);

/**
 * @brief 执行器数据删除结果通知
 * @param scheduler 
 * @param deletionResult 
 */
void executorDataDeletionResultNotification(Scheduler* scheduler, const DeletionResult* deletionResult);

/**
 * @brief 配对触发器设备
 * @param scheduler 
 * @param deviceInfo 
 * @param[out] triggerInfo 
 * @return SE_ERR 
 */
SE_ERR pairTriggerDevice(Scheduler* scheduler, const DeviceInfo* deviceInfo, TriggerInfo* triggerInfo);

/**
 * @brief 触发器配置结果通知
 * @param scheduler 
 * @param configResult 
 * @return SE_ERR 
 */
void triggerConfigResultNotification(Scheduler* scheduler, const configResult* configResult);

/**
 * @brief 触发器数据删除结果通知
 * @param scheduler 
 * @param deletionResult 
 */
void triggerDataDeletionResultNotification(Scheduler* scheduler, const DeletionResult* deletionResult);


// 查询配对设备列表
DeviceInfoList getPairedDeviceList(const Scheduler* scheduler);
/* 
    功能描述：查询已配对的设备列表
    输入：
        - scheduler：调度器实例
    输出：已配对的设备列表
*/

// 设备绑定后更新场景数据
void updateSceneDataAfterDeviceBinding(Scheduler* scheduler, int tempId, int deviceId);
/* 
    功能描述：设备绑定后更新场景数据
    输入：
        - scheduler：调度器实例
        - tempId：临时ID
        - deviceId：设备ID
*/

// 查询需要同步的数据
SyncDataList querySyncData(const Scheduler* scheduler);
/* 
    功能描述：查询需要同步的数据
    输入：
        - scheduler：调度器实例
    输出：待同步的触发器和执行器数据
*/

// 模板变化通知
void templateChangeNotification(Scheduler* scheduler, int templateId);
/* 
    功能描述：模板变化通知
    输入：
        - scheduler：调度器实例
        - templateId：模板ID
*/

// 启用场景
void enableScene(Scheduler* scheduler, int templateId);
/* 
    功能描述：启用场景
    输入：
        - scheduler：调度器实例
        - templateId：场景模板ID
*/

// 停用场景
void disableScene(Scheduler* scheduler, int templateId);
/* 
    功能描述：停用场景
    输入：
        - scheduler：调度器实例
        - templateId：场景模板ID
*/

// 场景执行判定
SceneRule determineSceneExecution(const TriggerStatus* triggerStatus);
/* 
    功能描述：场景执行判定
    输入：
        - triggerStatus：触发条件满足状态
    输出：要执行的场景规则
*/

// 场景日志记录
void logSceneExecutionResult(const SceneExecutionResult* executionResult);
/* 
    功能描述：记录场景执行日志
    输入：
        - executionResult：场景执行结果
*/

#endif /* SE_SCHEDULER_H */
