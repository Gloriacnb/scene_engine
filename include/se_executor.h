#ifndef SE_EXECUTOR_H
#define SE_EXECUTOR_H

#include "se_scheduler.h"

typedef struct {
    
    // 执行器结构体定义
    // 执行设备信息
    // 对应场景信息
} Executor;

/**
 * @brief 
 * @param[out] executor 
 * @param config 
 * @return SE_ERR 
 */
SE_ERR configureExecutor(Executor* executor, const ExecutorInfo* config);

/**
 * @brief 
 * @param executor 
 * @param config 
 * @return int 
 */
int deleteExecutorData(Executor* executor, const ExecutorConfig* config);


// 场景执行
void sceneExecution(Executor* executor, int sceneId);
/* 
    功能描述：场景执行
    输入：
        - executor：执行器实例
        - sceneId：场景ID
*/

// 设备控制
DeviceControlResult deviceControl(Executor* executor, int deviceId, const ControlCommand* command);
/* 
    功能描述：设备控制
    输入：
        - executor：执行器实例
        - deviceId：设备ID
        - command：控制命令
    输出：设备控制结果
*/

// 控制结果上报
void controlResultReport(Executor* executor, const ControlResult* result);
/* 
    功能描述：控制结果上报
    输入：
        - executor：执行器实例
        - result：控制结果
*/
#endif /* EXECUTOR_H */