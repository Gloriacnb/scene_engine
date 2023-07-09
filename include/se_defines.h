/**
 * @defgroup
 * @{
 * @copyright Copyright (c) 2023, Haier.Co, Ltd.
 * @file se_defines.h
 * @author shiqingpeng (shiqingpeng@haier.com)
 * @brief 
 * @date 2023-06-26
 * 
 * @par History:
 * <table>
 * <tr><th>Date         <th>version <th>Author  <th>Description
 * <tr><td>2023-06-26   <td>1.0     <td>        <td>
 * </table>
 */

#ifndef SE_DEFINES_H
#define SE_DEFINES_H

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SE_SUCCESS = 0,
    SE_FAILED = -1,
    SE_HAVE_BEEN_PARIED = -2,
    SE_ALLOCATE_MEMORY = -3,
    SE_ERR_INVALID_ARGUMENT = -4,
    SE_ERR_OUT_OF_MEMORY = -5,

}SE_ERR;

/******************* 调度器数据结构定义 *********************/

typedef struct _scheduler Scheduler;
//条件数据定义
typedef struct {
    uint8_t CondId;
    uint8_t OptType;
    uint8_t CondType;
    uint8_t FrameType;
    uint16_t StatusCmd;
    uint8_t CaeType;
    uint8_t StartWord;
    uint8_t StartBit;
    uint8_t Length;
    uint8_t Operation;
    uint16_t nBytesValue; //value中字节数
    char* value;  //不是字符串，是字节数组
}ConditionInfo;
//动作数据定义
typedef struct {
    uint8_t ActId;
    uint8_t execute_type;
    uint16_t nBtesActCmd; //ActCmd中字节数
    char* ActCmd; //不是字符串，是字节数组
}ActionInfo;

typedef struct {
    uint8_t ActsNum;
    ActionInfo *actions;
}ActionGroup;
//规则信息定义
typedef struct {
    uint8_t RuleId;
    uint16_t salience;
    uint8_t status;
    uint8_t exprType;
    char *expression;
    uint8_t ConditionNum;
    ConditionInfo* Conditions;
//    uint8_t ActionNum;
//    ActionInfo* Actions;
    uint8_t ActionGNum;
    ActionGroup *ActionGs;
}RuleInfo;
//场景数据定义
typedef struct {
    uint16_t Id;
    uint16_t Version;
    uint8_t type;
    uint8_t sceneType;
    uint8_t triggerType;
    uint8_t status;
    uint64_t createTime;
    uint64_t updateTime;
    uint8_t protocol_ver;
    uint8_t RuleNum;
    RuleInfo* Rules;

    void *tlv_ctx;  //for tlv
}SceneInfo;

//预置场景数据块
typedef struct {
    uint16_t len;
    uint8_t* data;
}PresettingSceneData;
//预置场景块定义
typedef union
{
    SceneInfo Tinfo;
    PresettingSceneData TData;

}PresettingBlock;

//设备ID MAC TempID
typedef struct {
    char id[33];
    char TempId[16];
    char BleMac[6];
}DeviceId;

// 待配对设备信息
typedef struct {
    // 设备信息字段
    DeviceId PairDev;
    uint8_t devAbility;         //bit0 Trigger; bit1 Scheduler; bit2 Executor
    uint8_t presetting_type;    // 0 data block,  1 struct info
    PresettingBlock block;      // 场景信息有两种提供方式，一种是预置数据块需要解析；一种是已经解析好的数据
} DeviceInfo;

typedef struct {
    DeviceId Executor;
}DetermineResult;
typedef struct {
    DeviceId NotifiedDev;
    DeviceId ConfiguredDev;
    SE_ERR Result;  
} configResult;

// 删除结果结构体
typedef struct {
    // 删除结果字段
    // ...
} DeletionResult;

// 同步数据列表结构体
typedef struct {
    // 同步数据列表字段
    // ...
} SyncDataList;

typedef struct 
{
    uint32_t TemplateId;
    uint32_t TemplateSize;
    uint16_t TemplateType;
    DeviceId LocalDevId;
    uint8_t LocalAbility;
}TemplateInfo;



typedef struct 
{
    char SceneName[16];
    uint16_t TriggerDevCnt;
    uint16_t ExecutorDevCnt;
}DeviceInfoList;

typedef struct 
{
    /* data */
}SceneExecutionResult;


/************************ 执行器数据结构定义 ****************************/

typedef struct _executor Executor;


typedef struct
{
    /* data */
}ControlResult;

typedef struct {
    uint16_t Length;
    char* Cmd;
}CtrlCommand;
typedef struct 
{
    /* data */
    DeviceId ExecutorDev;
    DeviceId Scheduler;
    CtrlCommand* cmd;
}ExecutionResult;


typedef struct {
    //配对同步数据
    uint8_t optype; //0 新增; 1 修改
    uint8_t role; //0 Trigger; 1 Executor
    DeviceId Obj;
    DeviceId ObjDev;
    DeviceId Scheduler;
    SceneInfo TemplateInfo;    
} SceneObject;

typedef SceneObject ExecutorInfo;
/************************** 触发器数据结构定义 ****************************/

typedef struct _trigger Trigger;

typedef SceneObject TriggerInfo;
typedef struct 
{
    /* data */
    //@todo
}DevProperties;

// 触发器状态枚举
typedef enum {
    TRIGGER_STATE_TRIGGERED,  // 触发
    TRIGGER_STATE_UNTRIGGERED   // 未触发
} TriggerState;

typedef struct {
    DeviceId TriggerDev;
    TriggerState state;
}TriggerStatus;


#ifdef DEBUG_PRINT
#define DebugPrint(format, ...) printf( format, ##__VA_ARGS__)
#else
#define DebugPrint(format, ...)
#endif


#ifdef __cplusplus
}
#endif



#endif  /* SE_DEFINES_H */
