#include "se_executor.h"
#include "executor_inner.h"
#include <stdlib.h>

Executor* makeNullExecutorHandle(void) {
    __executor* exec = calloc(sizeof(__executor), 1);
    return (Executor*)exec;       
}

SE_ERR configureExecutor(Executor* executor, const ExecutorInfo* config) {
    if( executor == NULL || config == NULL ) {
        return SE_FAILED;
    }
    __executor* exec = (__executor*)executor;
    exec->Scheduler = config->Scheduler;
    exec->Executor = config->Executor;
    exec->ExecutorDev = config->ExecutorDev;
    exec->SceneData = config->TemplateInfo; //@todo 这里只做了浅拷贝，如果使用有问题再修改
    return SE_SUCCESS;
}

SE_ERR sceneExecution(Executor* executor, ControlCommand* cmd) {
    return SE_FAILED;
}