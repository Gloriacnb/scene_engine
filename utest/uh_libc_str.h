/**
 * @addtogroup grp_uhoslibc
 * @{
 * @copyright Copyright (c) 2021, Haier.Co, Ltd.
 * @file uh_libc_str.h
 * @author wangshaolin (wangshaolin@haier.com)
 * @brief
 * @version 0.1
 * @date 2022-02-21
 *
 *
 */
#ifndef __UH_LIBC_STR_H__
#define __UH_LIBC_STR_H__

#include "uh_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief converts the initial portion of the string pointed to by nptr to int
 * @param nptr
 * @return uhos_s32
 */
uhos_s32 uhos_libc_atoi(const uhos_char *nptr);

/**
 * @brief converts the initial portion of the string pointed to by nptr to long long
 * @param nptr
 * @return uhos_s64
 */
uhos_s64 uhos_libc_atoll(const uhos_char *nptr);

/**
 * @brief calculate the length of a string
 *
 * @param s string
 * @return uhos_size_t The length of string
 */
uhos_size_t uhos_libc_strlen(const uhos_char *s);

/**
 * @brief returns a pointer to the first occurrence of the character c in the string s.
 * @param s
 * @param c
 * @return uhos_char* return a pointer to the matched character or NULL if the  character  is  not  found.
       The  terminating  null byte is considered part of the string
 */
uhos_char *uhos_libc_strchr(const uhos_char *s, int c);

/**
 * @brief returns a pointer to the last occurrence of the character c in the string s.
 * @param s
 * @param c
 * @return uhos_char* return a pointer to the matched character or NULL if the  character  is  not  found.
       The  terminating  null byte is considered part of the string
 */
uhos_char *uhos_libc_strrchr(const uhos_char *s, int c);

/**
 * @brief  finds the first occurrence of the substring needle in the string haystack.  The terminating null bytes
       ('\0') are not compared.
 *
 * @param haystack C string to be scanned
 * @param needle C string containing the sequence of characters to match.
 * @return uhos_char* A pointer to the first occurrence in str1 of the entire sequence of characters specified in str2,
 * or a null pointer if the sequence is not present in str1
 */
uhos_char *uhos_libc_strstr(const uhos_char *haystack, const uhos_char *needle);

/**
 * @brief Copies the C string pointed by source into the array pointed by destination, including the terminating null character
 * @param dest Pointer to the destination array where the content is to be copied.
 * @param src C string to be copied.
 * @return uhos_char* destination is returned.
 */
uhos_char *uhos_libc_strcpy(uhos_char *dest, const uhos_char *src);

/**
 * @brief Copies the first num characters of source to destination
 * @param dest Pointer to the destination array where the content is to be copied.
 * @param src C string to be copied.
 * @param n Maximum number of characters to be copied from source
 * @return uhos_char*
 */
uhos_char *uhos_libc_strncpy(uhos_char *dest, const uhos_char *src, uhos_size_t n);

/**
 * @brief compares the two strings s1 and s2.  The locale is not taken into account.  It returns an integer less than, equal to, or greater than zero if s1 is
 * found, respectively, to be less than, to match, or be greater than s2.
 * The uhos_libc_strncmp() function is similar, except it compares only the first (at most) n bytes of s1 and s2.
 * @param s1
 * @param s2
 * @return uhos_s32* return an integer less than, equal to, or greater than zero if s1 (or the first n bytes thereof) is found, respectively, to be less than,
 * to match, or be greater than s2.
 */
uhos_s32 uhos_libc_strcmp(const uhos_char *s1, const uhos_char *s2);
uhos_s32 uhos_libc_strncmp(const uhos_char *s1, const uhos_char *s2, uhos_size_t n);

/**
 * @brief appends  the  src string to the dest string, overwriting the terminating null byte ('\0') at the end of dest, and then adds a terminating null byte.
 * The strings may not overlap, and the dest string must have enough space for the result.  If dest is not large enough, program behavior is unpredictable;
 * The uhos_libc_strncat() function is similar, except that it will use at most n bytes from src; and src does not need to be null-terminated if it contains n
 * or more bytes.
 * As with uhos_libc_strcat(), the resulting string in dest is always null-terminated.
 * If src contains n or more bytes, uhos_libc_strncat() writes n+1 bytes to dest (n from src plus the terminating null byte).  Therefore, the size of dest must
 * be at least strlen(dest)+n+1.
 * @param dest
 * @param src
 * @return uhos_char* return a pointer to the resulting string dest.
 */
uhos_char *uhos_libc_strcat(uhos_char *dest, const uhos_char *src);
uhos_char *uhos_libc_strncat(uhos_char *dest, const uhos_char *src, uhos_size_t n);

/**
 * @brief The uhos_libc_strtok() function breaks a string into a sequence of zero or more nonempty tokens.  On the first call to uhos_libc_strtok(),
       the string to be parsed should be specified in str.  In each subsequent call that should parse  the  same  string,  str
       must be NULL.
      The  uhos_libc_strtok_r()  function  is  a reentrant version of uhos_libc_strtok().  The saveptr argument is a pointer to a char * variable
       that is used internally by uhos_libc_strtok_r() in order to maintain context between successive calls that parse the same string.
 * @param str
 * @param delim
 * @return uhos_char* return a pointer to the next token, or NULL if there are no more tokens.
 */
uhos_char *uhos_libc_strtok(uhos_char *str, const uhos_char *delim);
uhos_char *uhos_libc_strtok_r(uhos_char *str, const uhos_char *delim, uhos_char **saveptr);

/**
 * @brief 将可变参数按照format的格式转化为字符串
 * @param str 转换后字符串
 * @param size str最大长度
 * @param format 转换格式
 * @param ... 可变参数
 * @return uhos_s32
 */
uhos_s32 uhos_libc_snprintf(uhos_char *str, uhos_size_t size, const uhos_char *format, ...);

/**
 * @brief 发送格式化输出到 str 所指向的字符串
 * @param str 字符串
 * @param format 格式
 * @param ... 可变参数
 * @return uhos_s32
 */
uhos_s32 uhos_libc_sprintf(uhos_char *str, const uhos_char *format, ...);

/**
 * @brief 读取格式化的字符串中的数据
 * @param str 原始数据
 * @param format 格式
 * @param ... 可变参数
 * @return uhos_s32
 */
uhos_s32 uhos_libc_sscanf(uhos_char *str, const uhos_char *format, ...);

/**
 * @brief duplicate of a string
 *
 * @param s string
 * @return uhos_char The duplicated string
 */
uhos_char *uhos_libc_strdup(uhos_char *s);

#ifdef __cplusplus
}
#endif

#endif // __UH_LIBC_STR_H__

/**@}*/