/**
 * @addtogroup grp_uhosos  
 * @{
 * @copyright Copyright (c) 2021, Haier.Co, Ltd.
 * @file uh_mutex.h
 * @author shiqingpeng (shiqingpeng@haier.com)
 * @brief
 * @date 2021-10-18
 *
 * @par History:
 * <table>
 * <tr><th>Date         <th>version <th>Author  <th>Description
 * <tr><td>2021-10-18   <td>1.0     <td>        <td>
 * </table>
 */
#ifndef __UH_MUTEX_H__
#define __UH_MUTEX_H__

#include "uh_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UHOS_MUTEX_WAIT_FOREVER 0xfffffffful
#define UHOS_MUTEX_WAIT_NONE    0u

struct uhos_mutex_s;
typedef struct uhos_mutex_s *uhos_mutex_t;

/**
 * @brief Create and Initialize a Mutex object
 *
 * @param [out] mutex mutex ID for reference by other functions
 * @return uhos_s32 errno
 */
uhos_s32 uhos_mutex_create(uhos_mutex_t *mutex);

/**
 * @brief Delete a Mutex that was created by uhos_mutex_creat
 *
 * @param [in] mutex mutex ID obtained by uhos_mutex_creat
 * @return uhos_s32
 */
uhos_s32 uhos_mutex_delete(uhos_mutex_t mutex);

/**
 * @brief Wait until a Mutex becomes available.
 *
 * @param mutex     mutex ID obtained by uhos_mutex_creat.
 * @param millisec  timeout value
 *                  or
 *                      UHOS_MUTEX_WAIT_FOREVER in case of no time-out
 *                      UHOS_MUTEX_WAIT_NONE in case of non-block
 * @return uhos_s32
 */
uhos_s32 uhos_mutex_wait(uhos_mutex_t mutex, uhos_u32 millisec);

/**
 * @brief Release a Mutex that was obtained by uhos_mutex_wait
 *
 * @param mutex mutex ID obtained by uhos_mutex_creat.
 * @return uhos_s32
 */
uhos_s32 uhos_mutex_release(uhos_mutex_t mutex);
#ifdef __cplusplus
}
#endif

#endif // __UH_MUTEX_H__
/**@}*/