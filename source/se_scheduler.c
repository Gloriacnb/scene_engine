#include "se_scheduler.h"
#include "scheduler_inner.h"
#include <assert.h>

static SE_ERR parseSceneBaseInfo(templateInfo* temp, SceneBaseInfo* info);

/**
 * @brief       从电脑版预置文件中加载场景模板文件
 * @param[out]  scheduler 调度器实例
 * @param       templateFile 模板文件路径
 */
SE_ERR loadSceneTemplateFile(Scheduler* scheduler, templateInfo* templateFile) {
    __scheduler* sh = (__scheduler*)scheduler;
    assert(sh);
    assert(templateFile);
    SceneBaseInfo BaseInfo;
    if( parseSceneBaseInfo(templateFile, &BaseInfo) == SUCCESS ) {
        sh->SceneId = BaseInfo.id;
    }

    return SUCCESS;
}



SE_ERR parseSceneBaseInfo(templateInfo* temp, SceneBaseInfo* info) {
    //调用 scene组件 load 方法获取模板内容
    info->id = 121; //@todo 临时测试
    return SUCCESS;
}