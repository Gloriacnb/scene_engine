#ifndef SE_EXECUTOR_H
#define SE_EXECUTOR_H

#include "se_defines.h"

Executor* makeNullExecutorHandle(void);

/**
 * @brief 
 * 
 * @param executor 
 * @param config 
 * @param[out] result 
 * @return SE_ERR 
 */
SE_ERR configureExecutor(Executor* executor, const ExecutorInfo* config, configResult* result);

/**
 * @brief 
 * @param executor 
 * @param config 
 * @return int 
 */
// int deleteExecutorData(Executor* executor);


/**
 * @brief 
 * 
 * @param executor 
 * @param[out] cmd 
 * @return SE_ERR 
 */
SE_ERR sceneExecution(Executor* executor, ControlCommand* cmd);
/* 
    功能描述：场景执行
    输入：
        - executor：执行器实例
        - sceneId：场景ID
*/

// 控制结果上报
SE_ERR controlResultReport(Executor* executor, const ControlResult* result);
/* 
    功能描述：控制结果上报
    输入：
        - executor：执行器实例
        - result：控制结果
*/
#endif /* EXECUTOR_H */