#ifndef SE_EXECUTOR_H
#define SE_EXECUTOR_H

#include "se_defines.h"


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
int deleteExecutorData(Executor* executor);


// 场景执行
ControlCommand sceneExecution(Executor* executor, int sceneId);
/* 
    功能描述：场景执行
    输入：
        - executor：执行器实例
        - sceneId：场景ID
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