#include "se_scheduler.h"
#include "scheduler_inner.h"
#include "se_port.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "uhos_ctlv.h"

Scheduler* makeNullSchedulerHandle(void) {
    __scheduler* sch = calloc(1, sizeof(__scheduler));
    assert(sch);
    sch->state = IDLE;
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
    uint8_t* TemplateData = calloc(templateFile->TemplateSize, 1);
    assert(TemplateData);
    int32_t LoadBytes = uhos_scene_data_load(templateFile->TemplateId, templateFile->TemplateType, 0, TemplateData, templateFile->TemplateSize);
    assert(LoadBytes > 0);
    if( parseTLV(TemplateData, LoadBytes, &sch->TemplateData) != SE_SUCCESS ) {
        return SE_FAILED;
    }
    sch->SceneId = sch->TemplateData.Id;
    sch->SceneVer = sch->TemplateData.Version;
    sch->LocalAbility = templateFile->LocalAbility;
    sch->LocalDev = templateFile->LocalDevId;
    sch->PairDevList = NULL;
    sch->state = IDLE;
    return SE_SUCCESS;
}


SE_ERR isPairingDeviceMatchScene(Scheduler* scheduler, uint16_t sceneId, uint16_t sceneVer) {
    if( scheduler == NULL ) {
        return SE_FAILED;
    }
    __scheduler* sch = (__scheduler*)scheduler;
    assert(sch);
    if( (sch->SceneId == sceneId) && (sch->SceneVer == sceneVer) ) {
        return SE_SUCCESS;
    } 
    return SE_FAILED;    
}

static SE_ERR pairDevice(Scheduler* scheduler, const DeviceInfo* pairDeviceInfo, uint8_t Role, SceneObject* executorInfo) {    
    // 检查调度器和设备信息是否为空
    if (scheduler == NULL || pairDeviceInfo == NULL || executorInfo == NULL) {
        return SE_FAILED;
    }
    __scheduler* sch = (__scheduler*)scheduler;
    // 配对执行
    if( haveBeenPaired(scheduler, &pairDeviceInfo->PairDev) ) {
        DebugPrint("$$$$$$$ERR-%d", __LINE__);
        return SE_HAVE_BEEN_PARIED;
    }
    //需要输出的执行器配置数据
    SceneInfo* pSceneConfig = &executorInfo->TemplateInfo;
    assert(pSceneConfig);
    //首先使用执行设备预置数据初始化 配置数据
    SE_ERR rst = getPresettingSceneConfig(pairDeviceInfo, Role, pSceneConfig);
    if(rst != SE_SUCCESS) {
        DebugPrint("$$$$$$$ERR-%d", __LINE__);
        return rst;
    }
    //判断预置信息与模板信息是否匹配
    if( isSceneMatch(scheduler, pSceneConfig) != SE_SUCCESS ) {

        DebugPrint("$$$$$$$ERR-%d\n", __LINE__);
        return SE_FAILED;
    }
    //读取模板内配置数据 填充执行器配置
    rst = fillWithTemplateData(scheduler, pSceneConfig);
    if(rst != SE_SUCCESS) {
        DebugPrint("$$$$$$$ERR-%d", __LINE__);
        return rst;
    }

    // 其它executor配置信息
    executorInfo->optype = 0;
    executorInfo->role = Role;
    executorInfo->Obj = chooseWhereToCreate(scheduler, pairDeviceInfo);
    executorInfo->ObjDev = pairDeviceInfo->PairDev;
    executorInfo->Scheduler = sch->LocalDev;

    return addPairStatus(&sch->PairDevList, &pairDeviceInfo->PairDev, Role, PAIRED);
}

SE_ERR pairExecutorDevice(Scheduler* scheduler, const DeviceInfo* pairDeviceInfo, ExecutorInfo* executorInfo) {
    return pairDevice(scheduler, pairDeviceInfo, 1, executorInfo);
}

SE_ERR pairTriggerDevice(Scheduler* scheduler, const DeviceInfo* deviceInfo, TriggerInfo* triggerInfo) {
    return pairDevice(scheduler, deviceInfo, 0, triggerInfo);
}

SE_ERR executorConfigResultNotification(Scheduler* scheduler, const configResult* configResult) {
    if (scheduler == NULL || configResult == NULL) {
        return SE_FAILED;
    }
    __scheduler* sch = (__scheduler*)scheduler;
    PairStatus* st = findPairStatusByDeviceId(sch->PairDevList, &configResult->ConfiguredDev);
    if( st ) {
        st->state = SYNCED;
        return SE_SUCCESS;
    }
    return SE_FAILED;
}

SE_ERR triggerConfigResultNotification(Scheduler* scheduler, const configResult* configResult) {
    return executorConfigResultNotification(scheduler, configResult);
}
SE_ERR determineSceneExecution(Scheduler* scheduler, const TriggerStatus* triggerStatus, DetermineResult* DetRst) {
    if( scheduler == NULL || triggerStatus == NULL || DetRst == NULL ) {
        return SE_FAILED;
    }
    // __scheduler* sch = (__scheduler*)scheduler;

    return SE_FAILED;
}

DeviceInfoList getPairedDeviceList(const Scheduler* scheduler) {


    DeviceInfoList deviceList;
    memset(&deviceList, 0, sizeof(DeviceInfoList));
    __scheduler* sch = (__scheduler*)scheduler;
    if( sch == NULL ) {
        return deviceList;
    }
    const PairStatus* current = sch->PairDevList;

    while (current != NULL) {
        if (current->state == SYNCED) {
            if (current->Role == 0) {
                deviceList.TriggerDevCnt++;
            }
            else {
                deviceList.ExecutorDevCnt++;
            }
        }
        current = current->next;
    }

    return deviceList;
}

SE_ERR unpairDevice(Scheduler* scheduler, DeviceId dev) {
    __scheduler* sch = (__scheduler*)scheduler;
    if( !haveBeenPaired(scheduler, &dev) ) {
        return SE_ERR_INVALID_ARGUMENT;
    }
    
    return SE_FAILED;
}

/* inner functions */


static DeviceId chooseWhereToCreate(Scheduler* scheduler, const DeviceInfo* pairDeviceInfo) {
    //@todo 后期会实现选择逻辑，当前默认与调度器在一起
    __scheduler* sch = (__scheduler*)scheduler;
    assert(sch);
    return sch->LocalDev;
}

void copyConditionsId(SceneInfo* info) {
    for (size_t i = 0; i < info->RuleNum; i++)
    {
        if(info->Rules[i].Conditions == NULL) {
            info->Rules[i].Conditions = calloc(info->Rules[i].ConditionNum, sizeof(ConditionInfo));
            assert(info->Rules[i].Conditions);
        }
        for (size_t j = 0; j < info->Rules[i].ConditionNum; j++) {
            info->Rules[i].Conditions[j].CondId = info->Rules[i].Cids[j];

        }
        
    }
    
}
void copyActionsId(SceneInfo* info) {
    for (size_t i = 0; i < info->RuleNum; i++)
    {
        for (size_t j = 0; j < info->Rules[i].ActionGNum; j++)
        {
            if( info->Rules[i].ActionGs[j].actions == NULL ) {
                info->Rules[i].ActionGs[j].actions = calloc(info->Rules[i].ActionGs[j].ActsNum, sizeof(ActionInfo));
                assert(info->Rules[i].ActionGs[j].actions);
            }
            for (size_t k = 0; k < info->Rules[i].ActionGs[j].ActsNum; k++)
            {
                info->Rules[i].ActionGs[j].actions[k].ActId = info->Rules[i].ActionGs[j].actionIds[k];
            }
            
        }
        
    }
    
}

static SE_ERR getPresettingSceneConfig(const DeviceInfo* pairDeviceInfo, uint8_t Role, SceneInfo* sinfo) {
    assert(pairDeviceInfo);
    if( pairDeviceInfo->presetting_type == 1 ) {
        return deepCopySceneInfo(&pairDeviceInfo->block.Tinfo, sinfo);
    }
    //@todo 如果是块数据，需要解析并转化为SceneInfo 再输出
    else {
        if( parseTLV(pairDeviceInfo->block.TData.data, pairDeviceInfo->block.TData.len, sinfo) == SE_SUCCESS ) {
            if( Role == 0 ) {
                copyConditionsId(sinfo);
            }
            else {
                copyActionsId(sinfo);
            }
            return SE_SUCCESS;
        }
        else {
            DebugPrint("$$$$$$$ERR-parseTLV");
        }
    }

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

    for (size_t i = 0; i < Rule->ActionGNum; i++)
    {
		if(Rule->ActionGs) {
            for (size_t j = 0; j < Rule->ActionGs[i].ActsNum; j++)
            {
                if(Rule->ActionGs[i].actions[j].ActId == ActId ) {
                    return &Rule->ActionGs[i].actions[j];
                }
            }
            
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

        /* 这里根据讨论结果，触发器配对时无规则信息，需要匹配所有规则 */
        // RuleInfo* sourceinfo = getTemplateRule(&sch->TemplateData, fillinfo->RuleId);
        for (size_t n = 0; n < sch->TemplateData.RuleNum; n++)
        {
            RuleInfo* sourceinfo = &sch->TemplateData.Rules[n];

            if( sourceinfo ) {
                for (size_t j = 0; j < fillinfo->ConditionNum; j++)
                {
                    ConditionInfo* fillCond = &fillinfo->Conditions[j];
                    assert(fillCond);
                    ConditionInfo* srcCond = getRuleCondition(sourceinfo, fillCond->CondId);
                    if( srcCond ) {
                        // *fillCond = *srcCond; //@todo 注意这里是浅拷贝，value字段所指向内存没有复制。
                        deepCopyConditionInfo(srcCond, fillCond);
                    }
                    else {
                        //@todo 模板数据中无匹配的 条件ID，需要错误处理
                    }
                }

                for (size_t m = 0; m < fillinfo->ActionGNum; m++)
                {
        
                    ActionGroup* ActGroup = &fillinfo->ActionGs[m];
                    for (size_t k = 0; k < ActGroup->ActsNum; k++)
                    {

                        ActionInfo* fillAction = &ActGroup->actions[k];
                        ActionInfo* srcAction = getRuleAction(sourceinfo, fillAction->ActId);
                        if( srcAction ) {
                            // *fillAction = *srcAction; //@todo 注意这里是浅拷贝，value字段所指向内存没有复制。
                            deepCopyActionInfo(srcAction, fillAction);
                        }
                        else {
                            //@todo 模板数据中无匹配的 动作ID，需要错误处理
                        }
                    }
                }
                
            }
        }
    }
    

    return SE_SUCCESS;
}

static bool haveBeenPaired(Scheduler* scheduler, const DeviceId* devid) {
    __scheduler* sch = (__scheduler*)scheduler;
    return isDeviceIdInList(sch->PairDevList, devid);
}

static SE_ERR addPairStatus(PairStatus** head, const DeviceId* deviceId, uint8_t Role, PairState state) {
    PairStatus* newPairStatus = (PairStatus*)malloc(sizeof(PairStatus));
    if (newPairStatus == NULL) {
        return SE_ALLOCATE_MEMORY;
    }

    newPairStatus->PairDev = *deviceId;
    newPairStatus->Role = Role;
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
        if (strcmp(current->PairDev.id, deviceId->id) == 0
            || strcmp(current->PairDev.TempId, deviceId->TempId) == 0 
            || strcmp(current->PairDev.BleMac, deviceId->BleMac) == 0 ) {
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
*/

static const utlv_tag_info_t gSceneConditionInfo_tags[] = {
 {0x2201, sizeof(uhos_u8), offsetof(ConditionInfo,CondId), utlv_ctype_uint8, 0,0,0,0,0,UHOS_NULL},
 {0x2202, sizeof(uhos_u8), offsetof(ConditionInfo,OptType), utlv_ctype_uint8, 0,0,0,0,0,UHOS_NULL},
 {0x2211, sizeof(uhos_u8), offsetof(ConditionInfo,CondType), utlv_ctype_uint8, 0,0,0,0,0,UHOS_NULL},
 {0x2212, sizeof(uhos_u8), offsetof(ConditionInfo,FrameType), utlv_ctype_uint8, 0,0,0,0,0,UHOS_NULL},
 {0x2213, sizeof(uhos_u16), offsetof(ConditionInfo,StatusCmd), utlv_ctype_uint16, 0,0,0,0,0,UHOS_NULL},
 {0x2214, sizeof(uhos_u8), offsetof(ConditionInfo,CaeType), utlv_ctype_uint8, 0,0,0,0,0,UHOS_NULL},
 {0x2215, sizeof(uhos_u8), offsetof(ConditionInfo,StartWord), utlv_ctype_uint8, 0,0,0,0,0,UHOS_NULL},
 {0x2216, sizeof(uhos_u8), offsetof(ConditionInfo,StartBit), utlv_ctype_uint8, 0,0,0,0,0,UHOS_NULL},
 {0x2217, sizeof(uhos_u8), offsetof(ConditionInfo,Length), utlv_ctype_uint8, 0,0,0,0,0,UHOS_NULL},
 {0x2204, sizeof(uhos_u8), offsetof(ConditionInfo,Operation), utlv_ctype_uint8, 0,0,0,0,0,UHOS_NULL},
 {0x2205, sizeof(uhos_u8), offsetof(ConditionInfo,value), utlv_ctype_arr_uint8, 1,
    utlv_ctype_uint16,offsetof(ConditionInfo,nBytesValue),0,0,UHOS_NULL},
};

static const utlv_tag_info_t gSceneConditionInfoObj_tags[] = {
  {0x2200, sizeof(ConditionInfo), 0, utlv_ctype_object, 0,0,0,0,
    sizeof(gSceneConditionInfo_tags)/sizeof(utlv_tag_info_t), gSceneConditionInfo_tags}
};

static const utlv_tag_info_t gSceneActionInfo_tags[] = {
  {0x2331, sizeof(uhos_u8), offsetof(ActionInfo,ActId), utlv_ctype_uint8, 0,0,0,0,0,UHOS_NULL},
  {0x2333, sizeof(uhos_u8), offsetof(ActionInfo,execute_type), utlv_ctype_uint8,0,0,0,0,0,UHOS_NULL},
  {0x2332, sizeof(uhos_u8), offsetof(ActionInfo,ActCmd), utlv_ctype_arr_uint8, 1,
        utlv_ctype_uint16,offsetof(ActionInfo,nBtesActCmd),0,0,UHOS_NULL}
};

static const utlv_tag_info_t gSceneActionInfoObj_tags[] = {
  {0x233E, sizeof(ActionInfo), 0, utlv_ctype_object, 0, 0,0,0,
    sizeof(gSceneActionInfo_tags)/sizeof(utlv_tag_info_t), gSceneActionInfo_tags}
};

static const utlv_tag_info_t gSceneActGrps_tags[] = {
  {0x233f, sizeof(uhos_u8), offsetof(ActionGroup, ActsNum), utlv_ctype_uint8,0,0,0,0,0,UHOS_NULL},
//   {0x2330, sizeof(uhos_u8), offsetof(ActionGroup,actionIds), utlv_ctype_arr_uint8,1,0,0,0x233f,0,UHOS_NULL},
  {0x2340, sizeof(ActionInfo), offsetof(ActionGroup,actions), utlv_ctype_array_flag,1,0,0,0x233f,
    1, gSceneActionInfoObj_tags}
};

static const utlv_tag_info_t gSceneActGrpsObj_tags[] = {
  {0x2300, sizeof(ActionInfo), 0, utlv_ctype_object, 0, 0,0,0,
    sizeof(gSceneActGrps_tags)/sizeof(utlv_tag_info_t), gSceneActGrps_tags}
};

static const utlv_tag_info_t gSceneRule_tags[] = {
  //rid
  {0x2001, sizeof(uhos_u8), offsetof(RuleInfo,RuleId), utlv_ctype_uint8, 0,0,0,0,0,UHOS_NULL },
  {0x200E, sizeof(uhos_u16),offsetof(RuleInfo,salience), utlv_ctype_uint16,0,0,0,0,0,UHOS_NULL},
  {0x200F, sizeof(uhos_u8), offsetof(RuleInfo,status), utlv_ctype_uint8,0,0,0,0,0,UHOS_NULL},
  {0x2008, sizeof(uhos_u8), offsetof(RuleInfo,exprType),utlv_ctype_uint8,0,0,0,0,0,UHOS_NULL},
  {0x2005, sizeof(uhos_u8), offsetof(RuleInfo,expression),utlv_ctype_string,1,0,0,0,0,UHOS_NULL},
  {0x2006, sizeof(uhos_u8),offsetof(RuleInfo,ConditionNum), utlv_ctype_uint8, 0,0,0,0,0,UHOS_NULL },
  {0x2003, sizeof(uhos_u8),offsetof(RuleInfo,Cids), utlv_ctype_arr_uint8, 1,0,0,0x2006, 0, UHOS_NULL},
  {0x2010, sizeof(ConditionInfo),offsetof(RuleInfo,Conditions),utlv_ctype_array_flag, 1,0,0,0x2006,
    1, gSceneConditionInfoObj_tags},
  {0x2007, sizeof(uhos_u8),offsetof(RuleInfo,ActionGNum), utlv_ctype_uint8, 0,0,0,0,0,UHOS_NULL },
  {0x2004, sizeof(ActionGroup),offsetof(RuleInfo,ActionGs), utlv_ctype_array_flag, 1,0,0, 0x2007,
    1, gSceneActGrpsObj_tags}
};

static const utlv_tag_info_t gSceneRuleObj_tags[] =
{
  {0x2000, sizeof(RuleInfo), 0, utlv_ctype_object, 0, 0,0,0,
    sizeof(gSceneRule_tags)/sizeof(utlv_tag_info_t),
    gSceneRule_tags}
};

static const utlv_tag_info_t gSceneInfo_sub_tags[] =
{
  //T item_size         offset,                ctype
  //Id
  {1, sizeof(uhos_u16), offsetof(SceneInfo,Id), utlv_ctype_uint16, 0,0,0,0,0,UHOS_NULL },
  //Ver
  {2, sizeof(uhos_u16), offsetof(SceneInfo,Version), utlv_ctype_uint16, 0,0,0,0,0,UHOS_NULL },
  //Type
  {3, sizeof(uhos_u8), offsetof(SceneInfo,type), utlv_ctype_uint8, 0,0,0,0,0,UHOS_NULL },
  //sceneType
  {4, sizeof(uhos_u8), offsetof(SceneInfo,sceneType), utlv_ctype_uint8, 0,0,0,0,0,UHOS_NULL },
  //triggerType
  {5, sizeof(uhos_u8), offsetof(SceneInfo,triggerType), utlv_ctype_uint8, 0,0,0,0,0,UHOS_NULL },
  //status
  {6, sizeof(uhos_u8), offsetof(SceneInfo,status), utlv_ctype_uint8, 0,0,0,0,0,UHOS_NULL },
  //createTime
  {7, sizeof(uhos_u64), offsetof(SceneInfo,createTime), utlv_ctype_uint64,0,0,0,0,0,UHOS_NULL },
  //updateTime
  {8, sizeof(uhos_u64), offsetof(SceneInfo,updateTime), utlv_ctype_uint64,0,0,0,0,0,UHOS_NULL },
  //protocol_ver
  {0xC, sizeof(uhos_u8), offsetof(SceneInfo,protocol_ver), utlv_ctype_uint8, 0,0,0,0,0,UHOS_NULL },
  //RuleNum
  {0xB, sizeof(uhos_u8), offsetof(SceneInfo,RuleNum), utlv_ctype_uint8, 0,0,0,0,0,UHOS_NULL },
  {0xA, sizeof(RuleInfo), offsetof(SceneInfo,Rules), utlv_ctype_array_flag,1,0,0,0xB,
    1, gSceneRuleObj_tags}
};

static const utlv_tag_info_t gSceneInfo_tag_info = {
  0,sizeof(SceneInfo),0,utlv_ctype_object,0,0,0,0,
    sizeof(gSceneInfo_sub_tags)/sizeof(utlv_tag_info_t),
    gSceneInfo_sub_tags
};

static SE_ERR parseTLV(const uint8_t* data, uint16_t length, SceneInfo* sceneInfo)
{
    utlv_ctx_t *ctx;
    utlv_data_t indata = {0};

    if(!data || !sceneInfo) {
        //input null
        return SE_FAILED;
    }
    if(sceneInfo->tlv_ctx) {
        //not clear
        return SE_FAILED;
    }

    ctx = utlv_get_ctx(UTLV_S_TYPE_V1);
    if(!ctx) {
        //FAIL
        return SE_FAILED;
    }

    indata.buf = data;
    indata.buf_l = length;

    utlv_decode(ctx, &gSceneInfo_tag_info, &indata, sceneInfo);
    sceneInfo->tlv_ctx = ctx;

    return SE_SUCCESS;
}

static void freeSceneInfo(SceneInfo* sceneInfo) {
    if(!sceneInfo || !(sceneInfo->tlv_ctx)) {
        return;
    }

    utlv_free(sceneInfo->tlv_ctx, &gSceneInfo_tag_info, sceneInfo, 0);
    utlv_put_ctx(sceneInfo->tlv_ctx);
	sceneInfo->tlv_ctx = UHOS_NULL;
}


SE_ERR deepCopyConditionInfo(const ConditionInfo* src, ConditionInfo* dst) {
    if (src == NULL || dst == NULL) {
        return SE_ERR_INVALID_ARGUMENT;
    }

    // Copy non-pointer members
    memcpy(dst, src, sizeof(ConditionInfo));

    // Allocate memory for value field if necessary
    if (src->value != NULL) {
        dst->value = (char*)calloc(src->nBytesValue, sizeof(char));
        if (dst->value == NULL) {
            return SE_ERR_OUT_OF_MEMORY;
        }
        memcpy(dst->value, src->value, src->nBytesValue);
    }

    return SE_SUCCESS;
}


// 深拷贝 ActionInfo 结构体
SE_ERR deepCopyActionInfo(const ActionInfo* src, ActionInfo* dst) {
    if (src == NULL || dst == NULL) {
        return SE_ERR_INVALID_ARGUMENT;
    }

    // Copy non-pointer members
    memcpy(dst, src, sizeof(ActionInfo));

    // Allocate memory for ActCmd field if necessary
    if (src->ActCmd != NULL) {
        dst->ActCmd = (char*)calloc(src->nBtesActCmd, sizeof(char));
        if (dst->ActCmd == NULL) {
            return SE_ERR_OUT_OF_MEMORY;
        }
        memcpy(dst->ActCmd, src->ActCmd, src->nBtesActCmd);
    }

    return SE_SUCCESS;
}

// 深拷贝 ActionGroup 结构体
SE_ERR deepCopyActionGroup(const ActionGroup* src, ActionGroup* dst) {
    if (src == NULL || dst == NULL) {
        return SE_ERR_INVALID_ARGUMENT;
    }

    // Copy non-pointer members
    dst->ActsNum = src->ActsNum;

    // Allocate memory for actionIds array if necessary
    if (src->ActsNum > 0 && src->actionIds != NULL) {
        dst->actionIds = (uint8_t*)calloc(src->ActsNum, sizeof(uint8_t));
        if (dst->actionIds == NULL) {
            return SE_ERR_OUT_OF_MEMORY;
        }
        memcpy(dst->actionIds, src->actionIds, src->ActsNum * sizeof(uint8_t));
    }

    // Allocate memory for actions array if necessary
    if (src->ActsNum > 0 && src->actions != NULL) {
        if( dst->actions == NULL ) {
            dst->actions = (ActionInfo*)calloc(src->ActsNum, sizeof(ActionInfo));
            if (dst->actions == NULL) {
                // Clean up previously allocated memory
                if (dst->actionIds != NULL) {
                    free(dst->actionIds);
                    dst->actionIds = NULL;
                }
                return SE_ERR_OUT_OF_MEMORY;
            }
        }

        for (int i = 0; i < src->ActsNum; i++) {
            SE_ERR result = deepCopyActionInfo(&(src->actions[i]), &(dst->actions[i]));
            if (result != SE_SUCCESS) {
                // Clean up previously allocated memory
                free(dst->actionIds);
                free(dst->actions);
                dst->actionIds = NULL;
                dst->actions = NULL;
                return result;
            }
        }
    }

    return SE_SUCCESS;
}


// 深拷贝 RuleInfo 结构体
SE_ERR deepCopyRuleInfo(const RuleInfo* src, RuleInfo* dst) {
    if (src == NULL || dst == NULL) {
        return SE_ERR_INVALID_ARGUMENT;
    }

    // Copy non-pointer members
    dst->RuleId = src->RuleId;
    dst->salience = src->salience;
    dst->status = src->status;
    dst->exprType = src->exprType;

    // Allocate memory for expression if necessary
    if (src->expression != NULL) {
        size_t exprLen = strlen(src->expression) + 1;
        dst->expression = (char*)calloc(exprLen, sizeof(char));
        if (dst->expression == NULL) {
            return SE_ERR_OUT_OF_MEMORY;
        }
        memcpy(dst->expression, src->expression, exprLen);
    } else {
        dst->expression = NULL;
    }

    dst->ConditionNum = src->ConditionNum;
    dst->ActionGNum = src->ActionGNum;

    // Allocate memory for Cids array if necessary
    if (src->ConditionNum > 0 && src->Cids != NULL) {
        dst->Cids = (uint8_t*)calloc(src->ConditionNum, sizeof(uint8_t));
        if (dst->Cids == NULL) {
            return SE_ERR_OUT_OF_MEMORY;
        }
        memcpy(dst->Cids, src->Cids, src->ConditionNum * sizeof(uint8_t));
    } else {
        dst->Cids = NULL;
    }

    // Deep copy Conditions if necessary
    if (src->ConditionNum > 0 && src->Conditions != NULL) {
        dst->Conditions = (ConditionInfo*)calloc(src->ConditionNum, sizeof(ConditionInfo));
        if (dst->Conditions == NULL) {
            // Clean up previously allocated memory
            if (dst->expression != NULL) {
                free(dst->expression);
                dst->expression = NULL;
            }
            if (dst->Cids != NULL) {
                free(dst->Cids);
                dst->Cids = NULL;
            }
            return SE_ERR_OUT_OF_MEMORY;
        }
        for (int i = 0; i < src->ConditionNum; i++) {
            SE_ERR result = deepCopyConditionInfo(&(src->Conditions[i]), &(dst->Conditions[i]));
            if (result != SE_SUCCESS) {
                // Clean up previously allocated memory
                free(dst->expression);
                free(dst->Cids);
                free(dst->Conditions);
                dst->expression = NULL;
                dst->Cids = NULL;
                dst->Conditions = NULL;
                return result;
            }
        }
    } else {
        dst->Conditions = NULL;
    }

    // Deep copy ActionGs if necessary
    if (src->ActionGNum > 0 && src->ActionGs != NULL) {
        dst->ActionGs = (ActionGroup*)calloc(src->ActionGNum, sizeof(ActionGroup));
        if (dst->ActionGs == NULL) {
            // Clean up previously allocated memory
            if (dst->expression != NULL) {
                free(dst->expression);
                dst->expression = NULL;
            }
            if (dst->Cids != NULL) {
                free(dst->Cids);
                dst->Cids = NULL;
            }
            if (dst->Conditions != NULL) {
                free(dst->Conditions);
                dst->Conditions = NULL;
            }
            return SE_ERR_OUT_OF_MEMORY;
        }
        for (int i = 0; i < src->ActionGNum; i++) {
            SE_ERR result = deepCopyActionGroup(&(src->ActionGs[i]), &(dst->ActionGs[i]));
            if (result != SE_SUCCESS) {
                // Clean up previously allocated memory
                free(dst->expression);
                free(dst->Cids);
                free(dst->ActionGs);
                dst->expression = NULL;
                dst->Cids = NULL;
                dst->ActionGs = NULL;
                return result;
            }
        }
    } else {
        dst->ActionGs = NULL;
    }

    return SE_SUCCESS;
}


// 深拷贝 SceneInfo 结构体
SE_ERR deepCopySceneInfo(const SceneInfo* src, SceneInfo* dst) {
    if (src == NULL || dst == NULL) {
        return SE_ERR_INVALID_ARGUMENT;
    }

    // Copy non-pointer members
    dst->Id = src->Id;
    dst->Version = src->Version;
    dst->type = src->type;
    dst->sceneType = src->sceneType;
    dst->triggerType = src->triggerType;
    dst->status = src->status;
    dst->createTime = src->createTime;
    dst->updateTime = src->updateTime;
    dst->protocol_ver = src->protocol_ver;
    dst->RuleNum = src->RuleNum;

    // Allocate memory for Rules array if necessary
    if (src->RuleNum > 0 && src->Rules != NULL) {
        bool needFree = false;
        if( dst->Rules == NULL ) {
            needFree = true;
            dst->Rules = (RuleInfo*)calloc(src->RuleNum, sizeof(RuleInfo));
            if (dst->Rules == NULL) {
                return SE_ERR_OUT_OF_MEMORY;
            }
        }
        for (int i = 0; i < src->RuleNum; i++) {
            SE_ERR result = deepCopyRuleInfo(&(src->Rules[i]), &(dst->Rules[i]));
            if (result != SE_SUCCESS) {
                // Clean up previously allocated memory
                if( needFree ) {
                    free(dst->Rules);
                    dst->Rules = NULL;
                }
                return result;
            }
        }
    } else {
        dst->Rules = NULL;
    }

    // Perform other member copying or allocation if necessary

    return SE_SUCCESS;
}
