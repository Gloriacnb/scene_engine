/**
 * @file uh_libc_str.c
 * @author wangshaolin  (wangshaolin@haier.com)
 * @brief
 * @version 0.1
 * @date 2022-02-21
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
//#include "uh_libc.h"
#include "uh_libc_str.h"

uhos_s32 uhos_libc_atoi(const uhos_char *nptr)
{
    return atoi(nptr);
}

uhos_s64 uhos_libc_atoll(const uhos_char *nptr)
{
    return atoll(nptr);
}

uhos_size_t uhos_libc_strlen(const uhos_char *s)
{
    return strlen(s);
}

uhos_char *uhos_libc_strchr(const uhos_char *s, int c)
{
    return strchr(s, c);
}

uhos_char *uhos_libc_strrchr(const uhos_char *s, int c)
{
    return strrchr(s, c);
}

uhos_char *uhos_libc_strstr(const uhos_char *haystack, const uhos_char *needle)
{
    return strstr(haystack, needle);
}

uhos_char *uhos_libc_strcpy(uhos_char *dest, const char *src)
{
    return strcpy(dest, src);
}

uhos_char *uhos_libc_strncpy(uhos_char *dest, const char *src, uhos_size_t n)
{
    return strncpy(dest, src, n);
}

uhos_s32 uhos_libc_strcmp(const uhos_char *s1, const uhos_char *s2)
{
    return strcmp(s1, s2);
}

uhos_s32 uhos_libc_strncmp(const uhos_char *s1, const uhos_char *s2, uhos_size_t n)
{
    return strncmp(s1, s2, n);
}

uhos_char *uhos_libc_strcat(uhos_char *dest, const char *src)
{
    return strcat(dest, src);
}

uhos_char *uhos_libc_strncat(uhos_char *dest, const char *src, uhos_size_t n)
{
    return strcat(dest, src);
}

uhos_char *uhos_libc_strtok(uhos_char *str, const uhos_char *delim)
{
    return strtok(str, delim);
}

uhos_char *uhos_libc_strtok_r(uhos_char *str, const uhos_char *delim, uhos_char **saveptr)
{
    return strtok_r(str, delim, saveptr);
}

uhos_s32 uhos_libc_snprintf(uhos_char *str, uhos_size_t size, const uhos_char *format, ...)
{
    uhos_s32 len = 0;
    va_list  argp;

    va_start(argp, format);
    len = vsnprintf(str, size, format, argp);
    len = len > size - 1 ? size - 1 : len;
    va_end(argp);

    return len;
}

uhos_s32 uhos_libc_sprintf(uhos_char *str, const uhos_char *format, ...)
{
    uhos_s32 len = 0;
    va_list  argp;

    va_start(argp, format);
    len = vsprintf(str, format, argp);
    va_end(argp);

    return len;
}

uhos_s32 uhos_libc_sscanf(uhos_char *str, const uhos_char *format, ...)
{
    uhos_s32 len = 0;
    va_list  argp;

    va_start(argp, format);
    len = vsscanf(str, format, argp);
    va_end(argp);

    return len;
}

uhos_char *uhos_libc_strdup(uhos_char *s)
{
    if (s == UHOS_NULL)
        return UHOS_NULL;
    return strdup(s);
}

#if 0
/*
 * @brief  生成随机数据,
 * @param[out] output   输出的随机数据。
 * @param[in] output_len  输出缓存的大小。
 */
uhos_u64 uhos_current_timestamp_ms(void); //临时调用SDK中函数
void     uhos_random_generate(uhos_u8 *output, uhos_u32 output_len)
{
    int ind;
    srand(uhos_current_timestamp_ms());
    for (ind = 0; ind < output_len; ind++)
    {
        output[ind] = rand();
    }
}
#endif
