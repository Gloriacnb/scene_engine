/**
 *
 * @defgroup grp_uhostypes 类型定义
 * @{
 * @copyright Copyright (c) 2021, Haier.Co, Ltd.
 * @file uh_types.h
 * @author  tangjinqi@haier.com
 * @brief
 * @date 2021-09-27
 *
 * @par History:
 * <table>
 * <tr><th>Date         <th>version <th>Author  <th>Description
 * <tr><td>2021-09-27   <td>1.0     <td>tangjinqi        <td>init version
 * </table>
 */

#ifndef __UH_TYPES_H__
#define __UH_TYPES_H__

#ifdef __cplusplus
#define UHOS_NULL 0L
#else
#define UHOS_NULL ((void *)0)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define UHOS_CPU_BIT_32 (__SIZEOF_POINTER__ == 4)
#define UHOS_CPU_BIT_64 (__SIZEOF_POINTER__ == 8)

/* Largest integral types.  */
#if UHOS_CPU_BIT_32
#define UHOS_SWORD_TYPE int
#define UHOS_UWORD_TYPE unsigned int
#elif UHOS_CPU_BIT_64
#define UHOS_SWORD_TYPE long int
#define UHOS_UWORD_TYPE unsigned long int
#else
#error the cpu is error
#endif

typedef unsigned char      uhos_u8;
typedef signed char        uhos_s8;
typedef unsigned short     uhos_u16;
typedef signed short       uhos_s16;
typedef signed int         uhos_s32;
typedef unsigned int       uhos_u32;
typedef unsigned long long uhos_u64;
typedef signed long long   uhos_s64;
typedef char               uhos_char;
typedef void               uhos_void;
typedef unsigned char      uhos_bool;
typedef float              uhos_float;
typedef double             uhos_double;
typedef UHOS_SWORD_TYPE    uhos_ssize_t;
typedef UHOS_UWORD_TYPE    uhos_size_t;
typedef long int           uhos_intptr;
typedef unsigned long int  uhos_uintptr;

#define UHOS_TRUE  1
#define UHOS_FALSE 0

#define uhos_register register
#define UHOS_SUCCESS  0
#define UHOS_FAILURE  (-1)


#ifndef INT16_MAX
#define INT16_MAX   (32767)
#endif
#ifndef UINT16_MAX
#define UINT16_MAX  (65535U)
#endif
#ifndef INT32_MAX
#define INT32_MAX   (2147483647)
#endif
#ifndef UINT32_MAX
#define UINT32_MAX  (4294967295U)
#endif

typedef struct
{
    const uhos_char *name;
    const uhos_char *value;
} uhos_pair_t;

/* Definitions for byte order, according to significance of bytes,
   from low addresses to high addresses.  The value is what you get by
   putting '4' in the most significant byte, '3' in the second most
   significant byte, '2' in the second least significant byte, and '1'
   in the least significant byte, and then writing down one digit for
   each byte, starting with the byte at the lowest address at the left,
   and proceeding to the byte with the highest address at the right.  */

#define UHOS_LITTLE_ENDIAN 1234
#define UHOS_BIG_ENDIAN    4321
#define UHOS_PDP_ENDIAN    3412

/* i386/x86_64 are little-endian.  */
#define UHOS_BYTE_ORDER UHOS_LITTLE_ENDIAN

#ifdef __cplusplus
}
#endif

#endif /*__UH_TYPES_H__*/
       /**@} grp_uhostypes end */