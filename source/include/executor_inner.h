/**
 * @defgroup
 * @{
 * @copyright Copyright (c) 2023, Haier.Co, Ltd.
 * @file excutor_inner.h
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
typedef struct _executor{
    
    // 执行器结构体定义
    // 执行设备信息
    DeviceId Executor;
    DeviceId ExecutorDev;
    DeviceId Scheduler;
    // 对应场景信息
    SceneInfo SceneData;
}__executor;