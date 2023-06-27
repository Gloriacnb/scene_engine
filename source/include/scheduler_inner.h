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
#include <stdint.h>

typedef struct  {
    // 调度器结构体定义
    // ...
    int SceneId;
} __scheduler;

typedef struct {
    uint16_t id;
    uint16_t version;

} SceneBaseInfo;