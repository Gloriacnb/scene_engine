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

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SE_SUCCESS = 0,
    SE_FAILED = -1,
    
    
}SE_ERR;

/* 调度器数据结构定义 */

typedef struct __scheduler Scheduler;
typedef char TLV;
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
    char* value;
}ConditionInfo;

typedef struct {
    uint8_t ActId;
    char* ActCmd;
}ActionInfo;
typedef struct {
    uint8_t RuleId;
    uint8_t ConditionNum;
    ConditionInfo* Conditions;
    uint8_t ActionNum;
    ActionInfo* Actions;
}RuleInfo;

typedef struct {
    uint16_t Id;
    uint16_t Version;
    uint8_t RuleNum;
    RuleInfo* Rules;
}SceneInfo;

typedef struct {
    uint16_t len;
    uint8_t* data;
}PresettingSceneData;
typedef union
{
    SceneInfo Tinfo;
    PresettingSceneData TData;

}PresettingBlock;

typedef struct {
    char id[32];
}DeviceId;

// 配对设备信息结构体
typedef struct {
    // 设备信息字段
    DeviceId PairDev;
    uint8_t devAbility;         //bit0 Trigger; bit1 Scheduler; bit2 Executor
    uint8_t presetting_type;    // 0 data block,  1 struct info
    PresettingBlock block;      // 场景信息有两种提供方式，一种是预置数据块需要解析；一种是已经解析好的数据
} DeviceInfo;

typedef struct 
{
    /* data */
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

typedef struct {
    //触发器同步数据
} TriggerInfo;

typedef struct 
{
    /* data */
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


/* 执行器数据结构定义 */

typedef struct __executor Executor;


typedef struct
{
    /* data */
}ControlResult;

typedef struct 
{
    /* data */
}ControlCommand;
typedef struct {
    //执行器同步数据
    uint8_t optype; //0 新增; 1 修改
    uint8_t role; //0 Trigger; 1 Executor
    DeviceId Executor;
    DeviceId ExecutorDev;
    DeviceId Scheduler;
    SceneInfo TemplateInfo;    
} ExecutorInfo;


/* 触发器数据结构定义 */

typedef struct __trigger Trigger;


// 触发器配置信息结构体
typedef struct {
    // 触发器配置信息字段
    // ...
} TriggerConfig;

typedef struct 
{
    /* data */
}DevProperties;

// 触发器状态枚举
typedef enum {
    TRIGGER_STATUS_SUCCESS,  // 成功
    TRIGGER_STATUS_FAILURE   // 失败
} TriggerStatus;




#ifdef __cplusplus
}
#endif



#endif  /* SE_DEFINES_H */