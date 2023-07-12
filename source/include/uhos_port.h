#ifndef UHOS_PORT_H
#define UHOS_PORT_H
#ifdef __cplusplus
extern "C" {
#endif

// NO LOG out if defined
//#define NO_DEBUG_LOG

// NOT in uhos if defined
//#define NOT_BUILD_IN_UHOS


#ifndef NOT_BUILD_IN_UHOS
//In UHOS as default

#include "uh_types.h"
//#include "uh_libc_mem.h"
#include "uh_libc.h"
// #include "uh_k_list.h"  //list
#include "uh_log.h"

#ifdef NO_DEBUG_LOG 
#define UHPT_LOGD(...)
#define UHPT_LOGI(...)
#define UHPT_LOGW(...)
#define UHPT_LOGE(...)
#define MYDUMP(...)
#define UHPT_HEX_DUMP(...)
#else
#define UHPT_LOGD UHOS_LOGD
#define UHPT_LOGI UHOS_LOGI
#define UHPT_LOGW UHOS_LOGW
#define UHPT_LOGE UHOS_LOGE
#define MYDUMP(...)
#define UHPT_HEX_DUMP(...)
#endif

#ifndef offsetof
#define offsetof(t,m) ((uhos_size_t) &((t*)0)->m)
#endif

#else  //NOT_BUILD_IN_UHOS
//Not in UHOS

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



#ifdef NO_DEBUG_LOG
#define UHPT_LOGD(...)
#define UHPT_LOGI(...)
#define UHPT_LOGW(...)
#define UHPT_LOGE(...)
#else
#define UHPT_LOGD printf("\n%s(%d)[D]",__func__, __LINE__);printf
#define UHPT_LOGI printf("\n%s(%d)[I]",__func__, __LINE__);printf
#define UHPT_LOGW printf("\n%s(%d)[W]",__func__, __LINE__);printf
#define UHPT_LOGE printf("\n%s(%d)[E]",__func__, __LINE__);printf
#endif

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

#ifndef NO_DEBUG_LOG
#define MYDUMP(T,B,L) do {\
    int i; \
    for(i=0; i<(L) && (B); i++) { \
        printf("%02x,", ((uhos_u8*)(B))[i]); \
        if(15 == i%16) printf("\n"); \
    } \
    printf("\n"); \
} while(0)

#define UHPT_HEX_DUMP(t,b,l) ("\n%s(%d)[DUMP]",__func__, __LINE__);MYDUMP((t),(b),(l))
#else
#define MYDUMP(...)
#define UHPT_HEX_DUMP(...)
#endif //NO_DEBUG_LOG

#endif //NOT_BUILD_IN_UHOS


#ifdef __cplusplus
}
#endif
#endif
