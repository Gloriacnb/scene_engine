#include "se_scheduler.h"
#include "scheduler_inner.h"
#include "se_port.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

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
    if( HaveBeenPaired(scheduler, &pairDeviceInfo->PairDev) ) {
        return SE_HAVE_BEEN_PARIED;
    }
    //需要输出的执行器配置数据
    SceneInfo* pSceneConfig = &executorInfo->TemplateInfo;

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

    addPairStatus(&sch->PairDevList, &pairDeviceInfo->PairDev, PAIRING);
    return SE_SUCCESS;
}

SE_ERR pairTriggerDevice(Scheduler* scheduler, const DeviceInfo* deviceInfo, TriggerInfo* triggerInfo) {
    return pairExecutorDevice(scheduler, deviceInfo, triggerInfo);
}
SE_ERR executorConfigResultNotification(Scheduler* scheduler, const configResult* configResult) {
    if (scheduler == NULL || configResult == NULL) {
        return SE_FAILED;
    }
    __scheduler* sch = (__scheduler*)scheduler;
    PairStatus* st = findPairStatusByDeviceId(sch->PairDevList, &configResult->ConfiguredDev);
    if(st && (st->state == PAIRING) ) {
        st->state = ALREADYPAIRED;
    }
    return SE_FAILED;
}

SE_ERR determineSceneExecution(Scheduler* scheduler, const TriggerStatus* triggerStatus, DetermineResult* DetRst) {
    return SE_FAILED;
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

static RuleInfo* getTemplateRule(SceneInfo* tmplate, uint8_t ruleID) {
    assert(tmplate);
    for (size_t i = 0; i < tmplate->RuleNum; i++)
    {
        if( tmplate->Rules[i].RuleId == ruleID ) {
            return &tmplate->Rules[i];
        }
    }
    return (RuleInfo*)NULL;
}

static ConditionInfo* getRuleCondition(RuleInfo* Rule, uint8_t CondId) {
    for (size_t i = 0; i < Rule->ConditionNum; i++)
    {
        if(Rule->Conditions[i].CondId == CondId ) {
            return &Rule->Conditions[i];
        }
    }
    return NULL;
}
static ActionInfo* getRuleAction(RuleInfo* Rule, uint8_t ActId) {
    for (size_t i = 0; i < Rule->ActionNum; i++)
    {
        if(Rule->Actions[i].ActId == ActId ) {
            return &Rule->Actions[i];
        }
    }
    return NULL;
}

static SE_ERR fillWithTemplateData(Scheduler* scheduler, SceneInfo* sinfo) {

    if( scheduler == NULL || sinfo == NULL ) {
        return SE_FAILED;
    }
    __scheduler* sch = (__scheduler*)scheduler;
    for (size_t i = 0; i < sinfo->RuleNum; i++)
    {
        RuleInfo* fillinfo = &sinfo->Rules[i];
        RuleInfo* sourceinfo = getTemplateRule(&sch->TemplateData, fillinfo->RuleId);
        if( sourceinfo ) {
            for (size_t j = 0; j < fillinfo->ConditionNum; j++)
            {
                ConditionInfo* fillCond = &fillinfo->Conditions[j];
                ConditionInfo* srcCond = getRuleCondition(sourceinfo, fillCond->CondId);
                if( srcCond ) {
                    *fillCond = *srcCond; //@todo 注意这里是浅拷贝，value字段所指向内存没有复制。
                }
                else {
                    //@todo 模板数据中无匹配的 条件ID，需要错误处理
                }
            }
            for (size_t k = 0; k < fillinfo->ActionNum; k++)
            {
                ActionInfo* fillAction = &fillinfo->Actions[k];
                ActionInfo* srcAction = getRuleAction(sourceinfo, fillAction->ActId);
                if( srcAction ) {
                    *fillAction = *srcAction; //@todo 注意这里是浅拷贝，value字段所指向内存没有复制。
                }
                else {
                    //@todo 模板数据中无匹配的 动作ID，需要错误处理
                }
            }
            
        }
    }
    

    return SE_SUCCESS;
}

static bool HaveBeenPaired(Scheduler* scheduler, const DeviceId* devid) {
    __scheduler* sch = (__scheduler*)scheduler;
    return isDeviceIdInList(sch->PairDevList, devid);
}

static SE_ERR addPairStatus(PairStatus** head, const DeviceId* deviceId, State state) {
    PairStatus* newPairStatus = (PairStatus*)malloc(sizeof(PairStatus));
    if (newPairStatus == NULL) {
        return SE_ALLOCATE_MEMORY;
    }

    newPairStatus->PairDev = *deviceId;
    newPairStatus->state = state;
    newPairStatus->next = NULL;

    if (*head == NULL) {
        *head = newPairStatus;
    } else {
        PairStatus* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newPairStatus;
    }
    return SE_SUCCESS;
}


static PairStatus* findPairStatusByDeviceId(const PairStatus* head, const DeviceId* deviceId) {
    const PairStatus* current = head;

    while (current != NULL) {
        if (strcmp(current->PairDev.id, deviceId->id) == 0) {
            return (PairStatus*)current;  // 强制转换为非 const 指针，以便进行修改
        }
        current = current->next;
    }

    return NULL;  // 没有找到匹配的节点
}

static bool isDeviceIdInList(const PairStatus* head, const DeviceId* deviceId) {
    const PairStatus* current = head;

    while (current != NULL) {
        if (strcmp(current->PairDev.id, deviceId->id) == 0) {
            return true;  // 找到匹配的节点
        }
        current = current->next;
    }

    return false;  // 没有找到匹配的节点
}

/*
static PairStatus* removePairStatusByDeviceId(PairStatus** head, const DeviceId* deviceId) {
    PairStatus* current = *head;
    PairStatus* prev = NULL;

    while (current != NULL) {
        if (strcmp(current->PairDev.id, deviceId->id) == 0) {
            if (prev != NULL) {
                prev->next = current->next;  // 将前一个节点的 next 指针指向当前节点的下一个节点
            } else {
                *head = current->next;  // 如果要删除的节点是头节点，则更新链表头指针
            }
            free(current);  // 释放内存，删除节点
            return *head;  // 返回更新后的链表头指针
        }
        prev = current;
        current = current->next;
    }

    return *head;  // 没有找到匹配的节点，返回原链表头指针
}

static void parseTLV(const uint8_t* data, uint16_t length, SceneInfo* sceneInfo) {
    uint16_t index = 0;

    // 解析 TLV 数据
    while (index < length) {
        uint8_t tag = data[index++];
        uint8_t length = data[index++];
        const uint8_t* value = &data[index];

        // 根据 tag 解析对应的字段
        switch (tag) {
            case 0x01: {
                sceneInfo->Id = (value[0] << 8) | value[1];
                break;
            }
            case 0x02: {
                sceneInfo->Version = (value[0] << 8) | value[1];
                break;
            }
            case 0x03: {
                sceneInfo->RuleNum = value[0];
                sceneInfo->Rules = (RuleInfo*)malloc(sizeof(RuleInfo) * sceneInfo->RuleNum);

                uint8_t ruleIndex = 0;
                uint16_t ruleOffset = 1;
                while (ruleIndex < sceneInfo->RuleNum) {
                    RuleInfo* rule = &(sceneInfo->Rules[ruleIndex]);

                    rule->RuleId = value[ruleOffset++];
                    rule->ConditionNum = value[ruleOffset++];
                    rule->Conditions = (ConditionInfo*)malloc(sizeof(ConditionInfo) * rule->ConditionNum);

                    // 解析条件信息
                    for (uint8_t condIndex = 0; condIndex < rule->ConditionNum; condIndex++) {
                        ConditionInfo* condition = &(rule->Conditions[condIndex]);

                        condition->CondId = value[ruleOffset++];
                        // 解析其他字段，请根据实际情况进行修改和完善
                    }

                    rule->ActionNum = value[ruleOffset++];
                    rule->Actions = (ActionInfo*)malloc(sizeof(ActionInfo) * rule->ActionNum);

                    // 解析动作信息
                    for (uint8_t actIndex = 0; actIndex < rule->ActionNum; actIndex++) {
                        ActionInfo* action = &(rule->Actions[actIndex]);

                        action->ActId = value[ruleOffset++];
                        // 解析其他字段，请根据实际情况进行修改和完善
                    }

                    ruleIndex++;
                }
                break;
            }
            default:
                break;
        }

        index += length;
    }
}

static void freeSceneInfo(SceneInfo* sceneInfo) {
    if (sceneInfo == NULL) {
        return;
    }

    for (uint8_t ruleIndex = 0; ruleIndex < sceneInfo->RuleNum; ruleIndex++) {
        RuleInfo* rule = &(sceneInfo->Rules[ruleIndex]);

        free(rule->Conditions);
        free(rule->Actions);
    }

    free(sceneInfo->Rules);
}
*/