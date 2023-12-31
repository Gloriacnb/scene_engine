/**
 * @defgroup
 * @{
 * @copyright Copyright (c) 2023, Haier.Co, Ltd.
 * @file trigger_inner.h
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

typedef struct _trigger {
    // 触发器结构体定义
    DeviceId Trigger;
    DeviceId TriggerDev;
    DeviceId Scheduler;
    // 对应场景信息
    SceneInfo SceneData;
} __trigger;