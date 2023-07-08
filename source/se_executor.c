#include "se_executor.h"
#include "executor_inner.h"
#include <stdlib.h>

Executor* makeNullExecutorHandle(void) {
    __executor* exec = calloc(sizeof(__executor), 1);
    return (Executor*)exec;       
}

SE_ERR configureExecutor(Executor* executor, const ExecutorInfo* config, configResult* result) {
    if( executor == NULL || config == NULL ) {
        return SE_FAILED;
    }
    __executor* exec = (__executor*)executor;
    exec->Scheduler = config->Scheduler;
    exec->Executor = config->Obj;
    exec->ExecutorDev = config->ObjDev;
    exec->SceneData = config->TemplateInfo; //@todo 这里只做了浅拷贝，如果使用有问题再修改

    result->ConfiguredDev = exec->Executor;
    result->NotifiedDev = exec->Scheduler;
    result->Result = SE_SUCCESS;
    return SE_SUCCESS;
}

//SE_ERR sceneExecution(Executor* executor, ExecutionResult* cmd) {
SE_ERR sceneExecution(Executor* executor, const DetermineResult* DetRst, ExecCmds_t *cmds)
{
    return SE_FAILED;
}
