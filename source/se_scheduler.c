#include "se_scheduler.h"
#include "scheduler_inner.h"
#include "se_port.h"
#include <assert.h>
#include <stdlib.h>


Scheduler* makeNullSchedulerHandle(void) {
    __scheduler* sch = calloc(sizeof(__scheduler), 1);
    return (Scheduler*)sch;
}
/**
 * @brief       从电脑版预置文件中加载场景模板文件
 * @param[out]  scheduler 调度器实例
 * @param       templateFile 模板文件路径
 */
SE_ERR loadSceneTemplateFile(Scheduler* scheduler, TemplateInfo* templateFile) {
    __scheduler* sch = (__scheduler*)scheduler;
    assert(sch);
    assert(templateFile);
    SceneInfo BaseInfo;
    if( parseSceneBaseInfo(templateFile, &BaseInfo) == SE_SUCCESS ) {
        sch->SceneId = BaseInfo.Id;
        sch->SceneVer = BaseInfo.Id;
        sch->LocalAbility = 7;
        getLocakDeviceId(&sch->LocalDev);
    }

    return SE_SUCCESS;
}





SE_ERR pairExecutorDevice(Scheduler* scheduler, const DeviceInfo* pairDeviceInfo, ExecutorInfo* executorInfo) {
    
    // 检查调度器和设备信息是否为空
    if (scheduler == NULL || pairDeviceInfo == NULL || executorInfo == NULL) {
        return SE_FAILED;
    }
    __scheduler* sch = (__scheduler*)scheduler;
    // 配对执行

    //需要输出的执行器配置数据
    SceneInfo* pSceneConfig = &executorInfo->TemplateInfo;
    assert(pSceneConfig);

    //首先使用执行设备预置数据初始化 配置数据
    SE_ERR rst = getPresettingSceneConfig(pairDeviceInfo, pSceneConfig);
    if(rst != SE_SUCCESS) {
        return rst;
    }
    //判断预置信息与模板信息是否匹配
    if( isSceneMatch(scheduler, pSceneConfig) != SE_SUCCESS ) {
        return SE_FAILED;
    }
    //读取模板内配置数据 填充执行器配置
    rst = fillWithTemplateData(scheduler, pSceneConfig);
    if(rst != SE_SUCCESS) {
        return rst;
    }

    // 其它executor配置信息
    executorInfo->optype = 0;
    executorInfo->role = 1;
    executorInfo->Executor = chooseWhereToCreate(scheduler, pairDeviceInfo);
    executorInfo->ExecutorDev = pairDeviceInfo->PairDev;
    executorInfo->Scheduler = sch->LocalDev;

    return SE_SUCCESS;
}



/* inner functions */

static SE_ERR parseSceneBaseInfo(TemplateInfo* temp, SceneInfo* info) {
    //调用 scene组件 load 方法获取模板内容
    info->Id = 121; //@todo 临时测试
    info->Version = 1;
    info->RuleNum = 2;
    return SE_SUCCESS;
}

static DeviceId chooseWhereToCreate(Scheduler* scheduler, const DeviceInfo* pairDeviceInfo) {
    //@todo 后期会实现选择逻辑，当前默认与调度器在一起
    __scheduler* sch = (__scheduler*)scheduler;
    assert(sch);
    return sch->LocalDev;
}

static SE_ERR getPresettingSceneConfig(const DeviceInfo* pairDeviceInfo, SceneInfo* sinfo) {
    assert(pairDeviceInfo);
    if( pairDeviceInfo->presetting_type == 1 ) {
        *sinfo = pairDeviceInfo->block.Tinfo;
        return SE_SUCCESS;
    }
    //@todo 如果是块数据，需要解析并转化为SceneInfo 再输出
    return SE_FAILED;
}

static SE_ERR isSceneMatch(Scheduler* scheduler, SceneInfo* sinfo) {
    if( scheduler == NULL || sinfo == NULL ) {
        return SE_FAILED;
    }
    __scheduler* sch = (__scheduler*)scheduler;
    assert(sch);
    if( (sch->SceneId == sinfo->Id) && (sch->SceneVer == sinfo->Version) ) {
        return SE_SUCCESS;
    } 
    return SE_FAILED;
}
static SE_ERR fillWithTemplateData(Scheduler* scheduler, SceneInfo* sinfo) {
    //@todo 需要确认 scheduler内的场景模板数据后 再做数据提取
    return SE_FAILED;
}