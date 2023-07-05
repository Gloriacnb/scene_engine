#ifndef UHOS_CTLV_H
#define UHOS_CTLV_H

#ifdef __cplusplus
extern "C" {
#endif

//
// utlv_config.h
#ifndef NOT_BUILD_IN_UHOS
#include "uh_types.h"
#include "uh_libc_mem.h"

#define UTLV_LOGD(x, ...)
#define UTLV_LOGI(x, ...)
#define UTLV_LOGW(x, ...)
#define UTLV_LOGE(x, ...)
#define UTLV_HEX_DUMP(x, ...)

#ifndef offsetof
#define offsetof(t,m) ((uhos_size_t) &((t*)0)->m)
#endif

#else //NOT_BUILD_IN_UHOS means NOT_IN_UHOS
#include <stddef.h>

#ifdef __cplusplus
#define UHOS_NULL 0L
#else
#define UHOS_NULL ((void *)0)
#endif

#define UHOS_CPU_BIT_32      (__SIZEOF_POINTER__ == 4)
#define UHOS_CPU_BIT_64      (__SIZEOF_POINTER__ == 8)

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

#define UHOS_TRUE  1
#define UHOS_FALSE 0

#define uhos_register register
#define UHOS_SUCCESS  0
#define UHOS_FAILURE  (-1)

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#define UTLV_LOGD printf("\n%s(%d)[D]",__func__, __LINE__);printf
#define UTLV_LOGI printf("\n%s(%d)[I]",__func__, __LINE__);printf
#define UTLV_LOGW printf("\n%s(%d)[W]",__func__, __LINE__);printf
#define UTLV_LOGE printf("\n%s(%d)[E]",__func__, __LINE__);printf

#define uhos_libc_free free
#define uhos_libc_malloc malloc
#define uhos_libc_zalloc(x) calloc(1,(x))
#define uhos_libc_calloc calloc
#define uhos_libc_realloc realloc
#define uhos_libc_memcpy  memcpy
#define uhos_libc_memset  memset
#define uhos_libc_memcmp  memcmp
#define uhos_libc_memmove memmove
#define uhos_libc_strcpy  strcpy
#define uhos_libc_strncpy strncpy
#define uhos_libc_snprintf snprintf

//MYDUMP("buf", buf, len);
#define MYDUMP(T,B,L) do {\
    int i; \
    for(i=0; i<(L) && (B); i++) { \
        printf("%02x,", ((uhos_u8*)(B))[i]); \
        if(15 == i%16) printf("\n"); \
    } \
    printf("\n"); \
} while(0)

#define UTLV_HEX_DUMP(t,b,l) ("\n%s(%d)[DUMP]",__func__, __LINE__);MYDUMP((t),(b),(l))

#endif //NOT_BUILD_IN_UHOS

typedef enum {
    utlv_ctype_start_notused,

    utlv_ctype_uint8,
    utlv_ctype_sint8,
    utlv_ctype_uint16,
    utlv_ctype_sint16,
    utlv_ctype_uint32,
    utlv_ctype_sint32,
    utlv_ctype_uint64,
    utlv_ctype_sint64,
    utlv_ctype_enum,
    utlv_ctype_float,
    utlv_ctype_double,

    utlv_ctype_string,
    utlv_ctype_bindata,

    utlv_ctype_arr_uint8,
    utlv_ctype_arr_sint8,
    utlv_ctype_arr_uint16,
    utlv_ctype_arr_sint16,
    utlv_ctype_arr_uint32,
    utlv_ctype_arr_sint32,
    utlv_ctype_arr_uint64,
    utlv_ctype_arr_sint64,
    utlv_ctype_arr_enum,
    utlv_ctype_arr_float,
    utlv_ctype_arr_double,

    utlv_ctype_object,

    utlv_ctype_last,

    utlv_ctype_array_flag = 0x4000,
    utlv_ctype_mask = 0xBFFF

} utlv_tag_ctype_t;

// 
typedef enum {
  UTLV_S_TYPE_V1,

  UTLV_S_TYPE_LAST_NOT_USED
} utlv_s_type_t;

typedef struct utlv_ctx_ utlv_ctx_t;
utlv_ctx_t *utlv_get_ctx(utlv_s_type_t t);
uhos_void utlv_put_ctx(utlv_ctx_t *ctx);

typedef struct utlv_tag_info_ utlv_tag_info_t;
typedef struct utlv_tag_info_ {
    uhos_u32         tag;
    uhos_size_t      item_size;
    uhos_size_t      offset;
    utlv_tag_ctype_t ctype;
    uhos_u8          needtomalloc; //decoder to know if need to malloc for subtag
    utlv_tag_ctype_t num_ctype;    //for byte array, to define what type the numofbytes is.
                                    //it is 0, means not need this function
    uhos_size_t      num_offset;      //for some byte array, no such tag, but still need to keep num of array
    uhos_u32         array_num_tag; //only for array, the num tag in structure. 0 mean no
    uhos_u32         nsubtags;
    const utlv_tag_info_t  *psubtags;
} utlv_tag_info_t;

typedef struct utlv_data_ {
    const uhos_u8 *buf;
    uhos_u32 buf_l;
    uhos_u32 array_size;
} utlv_data_t;


uhos_s32 utlv_encode(
    const utlv_ctx_t *ctx,
    const utlv_tag_info_t *ptaginfo,
    uhos_void *in_param,
    utlv_data_t *pOut_data);

uhos_s32 utlv_decode(
    const utlv_ctx_t *ctx,
    const utlv_tag_info_t *ptaginfo,
    utlv_data_t *pIn_data,
    uhos_void *out_param);

uhos_s32 utlv_free(
    const utlv_ctx_t *ctx,
    const utlv_tag_info_t *ptaginfo,
    uhos_void *param,
    uhos_u32 num);

#ifdef __cplusplus
}
#endif
#endif //UHOS_CTLV_H
