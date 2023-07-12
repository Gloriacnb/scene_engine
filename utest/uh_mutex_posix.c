/**
 * @copyright Copyright (c) 2021, Haier.Co, Ltd.
 * @file uh_mutex_posix.c
 * @author shiqingpeng (shiqingpeng@haier.com)
 * @brief
 * @date 2021-10-19
 *
 * @par History:
 * <table>
 * <tr><th>Date         <th>version <th>Author  <th>Description
 * <tr><td>2021-10-19   <td>1.0     <td>        <td>
 * </table>
 */
#ifndef __UH_MUTEX_POSIX_H__
#define __UH_MUTEX_POSIX_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "uh_mutex.h"
#include <pthread.h>
#include <stdlib.h>

uhos_s32 uhos_mutex_create(uhos_mutex_t *mutex)
{
    pthread_mutex_t *   posix_mutex;
    pthread_mutexattr_t attr;

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

    posix_mutex = malloc(sizeof(pthread_mutex_t));
    if (posix_mutex && pthread_mutex_init(posix_mutex, &attr) == 0)
    {
        *mutex = (uhos_mutex_t)posix_mutex;
        return UHOS_SUCCESS;
    }
    return UHOS_FAILURE;
}

uhos_s32 uhos_mutex_delete(uhos_mutex_t mutex)
{
    pthread_mutex_destroy((pthread_mutex_t *)mutex);
    free((pthread_mutex_t *)mutex);
    return UHOS_SUCCESS;
}

uhos_s32 uhos_mutex_wait(uhos_mutex_t mutex, uhos_u32 millisec)
{
    if (millisec == UHOS_MUTEX_WAIT_FOREVER)
        return pthread_mutex_lock((pthread_mutex_t *)mutex);
    else if (millisec == UHOS_MUTEX_WAIT_NONE)
        return pthread_mutex_trylock((pthread_mutex_t *)mutex);
    return UHOS_FAILURE;
}

uhos_s32 uhos_mutex_release(uhos_mutex_t mutex)
{
    return pthread_mutex_unlock((pthread_mutex_t *)mutex);
}

#ifdef __cplusplus
}
#endif

#endif // __UH_MUTEX_POSIX_H__