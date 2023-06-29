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
    PAIRING,
    ALREADYPAIRED,
}State;
typedef struct _PairStatus {
    DeviceId PairDev;
    State state;
    struct _PairStatus* next;
} PairStatus;
typedef struct _scheduler {
    // 调度器结构体定义
    DeviceId LocalDev;
    uint8_t LocalAbility;
    uint16_t SceneId;
    uint16_t SceneVer;
    SceneInfo TemplateData;
    PairStatus* PairDevList;
}__scheduler;

static SE_ERR parseSceneBaseInfo(TemplateInfo* temp, SceneInfo* info);

static DeviceId chooseWhereToCreate(Scheduler* scheduler, const DeviceInfo* pairDeviceInfo);

static SE_ERR getPresettingSceneConfig(const DeviceInfo* pairDeviceInfo, SceneInfo* sinfo);

static SE_ERR isSceneMatch(Scheduler* scheduler, SceneInfo* sinfo);

static SE_ERR fillWithTemplateData(Scheduler* scheduler, SceneInfo* sinfo);

static bool HaveBeenPaired(Scheduler* scheduler, const DeviceId* devid);
static SE_ERR addPairStatus(PairStatus** head, const DeviceId* deviceId, State state);
static PairStatus* findPairStatusByDeviceId(const PairStatus* head, const DeviceId* deviceId);
static bool isDeviceIdInList(const PairStatus* head, const DeviceId* deviceId);
static PairStatus* removePairStatusByDeviceId(PairStatus** head, const DeviceId* deviceId);

static void parseTLV(const uint8_t* data, uint16_t length, SceneInfo* sceneInfo);

static void freeSceneInfo(SceneInfo* sceneInfo);