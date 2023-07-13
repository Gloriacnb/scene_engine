#ifndef SE_EXECUTOR_H
#define SE_EXECUTOR_H

#include "se_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

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
//SE_ERR sceneExecution(Executor* executor, ExecutionResult* cmd);
SE_ERR sceneExecution(Executor* executor, const DetermineResult* DetRst, ExecCmds_t *cmds);
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

#ifdef __cplusplus
}
#endif

#endif /* EXECUTOR_H */
