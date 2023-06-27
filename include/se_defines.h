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

typedef enum {
    SUCCESS = 0,
    FAILED = -1,
    
    
}SE_ERR;

/* 调度器数据结构定义 */

typedef struct __scheduler Scheduler;

// 设备信息结构体
typedef struct {
    // 设备信息字段
    // devID
    // typeid
    // devAbility

    // 场景信息有两种提供方式，一种是预置数据块需要解析；一种是已经解析好的数据，包含如下字段
    // 场景模板ID
    // 场景模板版本
    // 场景规则ID
    // 动作ID 或 触发条件ID
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
}templateInfo;


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
    //optype
    //role
    //
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








#endif  /* SE_DEFINES_H */