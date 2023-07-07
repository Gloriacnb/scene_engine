/**
 * @defgroup
 * @{
 * @copyright Copyright (c) 2023, Haier.Co, Ltd.
 * @file scheduler_inner.h
 * @author your name (you@haier.com)
 * @brief 
 * @date 2023-06-27
 * 
 * @par History:
 * <table>
 * <tr><th>Date         <th>version <th>Author  <th>Description
 * <tr><td>2023-06-27   <td>1.0     <td>        <td>
 * </table>
 */

#pragma once

#include "se_defines.h"
#include <stdbool.h>

typedef enum {
    UNPAIRED,
    PAIRED,
    SYNCED,
}PairState;
typedef struct _PairStatus {
    DeviceId PairDev;
    uint8_t Role;
    PairState state;
    struct _PairStatus* next;
} PairStatus;

typedef enum {
    IDLE,
    READY,
    WORKING,
}WorkState;
typedef struct _scheduler {
    // 调度器结构体定义
    WorkState state;
    DeviceId LocalDev;
    uint8_t LocalAbility;
    uint16_t SceneId;
    uint16_t SceneVer;
    SceneInfo TemplateData;
    PairStatus* PairDevList;
}__scheduler;

static DeviceId chooseWhereToCreate(Scheduler* scheduler, const DeviceInfo* pairDeviceInfo);

static SE_ERR getPresettingSceneConfig(const DeviceInfo* pairDeviceInfo, SceneInfo* sinfo);

static SE_ERR isSceneMatch(Scheduler* scheduler, SceneInfo* sinfo);

static SE_ERR fillWithTemplateData(Scheduler* scheduler, SceneInfo* sinfo);

static bool HaveBeenPaired(Scheduler* scheduler, const DeviceId* devid);
static SE_ERR addPairStatus(PairStatus** head, const DeviceId* deviceId, uint8_t Role, PairState state);
static PairStatus* findPairStatusByDeviceId(const PairStatus* head, const DeviceId* deviceId);
static bool isDeviceIdInList(const PairStatus* head, const DeviceId* deviceId);
// static PairStatus* removePairStatusByDeviceId(PairStatus** head, const DeviceId* deviceId);

static SE_ERR parseTLV(const uint8_t* data, uint16_t length, SceneInfo* sceneInfo);

// static void freeSceneInfo(SceneInfo* sceneInfo);