#define _POSIX_C_SOURCE 200809L

#include "strbuf.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

// 初始化 sb 结构体，容量为 alloc
void strbuf_init(struct strbuf *sb, size_t alloc)
{
    if (sb == NULL) return;
    sb->len = 0;
    sb->alloc = 0;
    sb->buf = NULL;
    
    if (alloc == 0) return;
    if (alloc > (size_t)INT_MAX) return;
    
    sb->buf = (char *)malloc(alloc);
    if (sb->buf == NULL) {
        sb->alloc = 0;
        return;
    }
    sb->buf[0] = '\0';
    sb->alloc = (int)alloc;
}

// 将字符串填充到 sb 中，长度为 len，容量为 alloc
void strbuf_attach(struct strbuf *sb, void *str, size_t len, size_t alloc)
{
    if (sb == NULL) return;
    if (str == NULL && len > 0) return;
    
    // 释放旧内存，防止内存泄漏
    free(sb->buf); 
    
    // 直接接管指针（0 拷贝）
    sb->buf = (char *)str;
    sb->len = (int)len;
    sb->alloc = (int)alloc;
}

// 释放 sb 结构体的内存
void strbuf_release(struct strbuf *sb)
{
    if (sb == NULL) return;
    free(sb->buf);
    sb->buf = NULL;
    sb->len = 0;
    sb->alloc = 0;
}

// 交换两个 strbuf
void strbuf_swap(struct strbuf *a, struct strbuf *b)
{
    if (a == NULL || b == NULL || a == b) return;
    struct strbuf tmp = *a;
    *a = *b;
    *b = tmp;
}

// 将 sb 中的原始内存取出，并将 sz 设置为其 alloc 大小
char *strbuf_detach(struct strbuf *sb, size_t *sz)
{
    if (sb == NULL) {
        if (sz != NULL) *sz = 0;
        return NULL;
    }
    char *ret = sb->buf;
    if (sz != NULL) *sz = (size_t)sb->alloc;
    
    sb->buf = NULL;
    sb->len = 0;
    sb->alloc = 0;
    return ret;
}

// 比较两个 strbuf 的内存是否相同
int strbuf_cmp(const struct strbuf *first, const struct strbuf *second)
{
    if (first == NULL || second == NULL) {
        if (first == second) return 0;
        return first == NULL ? -1 : 1;
    }
    
    size_t min_len = first->len < second->len ? (size_t)first->len : (size_t)second->len;
    int cmp = min_len == 0 ? 0 : memcmp(first->buf, second->buf, min_len);
    
    if (cmp != 0) return cmp;
    if (first->len < second->len) return -1;
    if (first->len > second->len) return 1;
    return 0;
}

// 清空 sb
void strbuf_reset(struct strbuf *sb)
{
    if (sb == NULL) return;
    sb->len = 0;
    if (sb->buf != NULL && sb->alloc > 0) {
        sb->buf[0] = '\0';
    }
}

// 确保在 len 之后 strbuf 中至少有 extra 个字节的空闲空间可用
void strbuf_grow(struct strbuf *sb, size_t extra)
{
    if (sb == NULL) return;
    
    // 防止内存越界
    if (extra > (size_t)INT_MAX - (size_t)sb->len - 1) {
        abort();
    }
    
    size_t needed = (size_t)sb->len + extra + 1;
    if (needed <= (size_t)sb->alloc) return;
    
    size_t new_alloc = sb->alloc > 0 ? (size_t)sb->alloc * 2 : 0;
    if (new_alloc < needed) new_alloc = needed;
    if (new_alloc > (size_t)INT_MAX) new_alloc = (size_t)INT_MAX;
    
    char *new_buf = (char *)realloc(sb->buf, new_alloc);
    if (new_buf == NULL) abort();
    
    sb->buf = new_buf;
    sb->alloc = (int)new_alloc;
    if (sb->len == 0) {
        sb->buf[0] = '\0';
    }
}

// 向 sb 追加长度为 len 的数据 data
void strbuf_add(struct strbuf *sb, const void *data, size_t len)
{
    if (sb == NULL || data == NULL || len == 0) return;
    
    if (len > (size_t)INT_MAX - (size_t)sb->len) return;
    
    bool inside = false;
    size_t offset = 0;
    if (sb->buf != NULL && sb->alloc > 0) {
        uintptr_t begin = (uintptr_t)(void*)sb->buf;
        uintptr_t addr = (uintptr_t)data;
        if (addr >= begin && addr < begin + (uintptr_t)(size_t)sb->alloc) {
            inside = true;
            offset = (size_t)(addr - begin);
        }
    }
    
    strbuf_grow(sb, len);
    const char *src = inside ? sb->buf + offset : (const char*)data;
    memmove(sb->buf + sb->len, src, len);
    sb->len += (int)len;
    sb->buf[sb->len] = '\0';
}

// 向 sb 追加一个字符 c
void strbuf_addch(struct strbuf *sb, int c)
{
    unsigned char ch = (unsigned char)c;
    strbuf_add(sb, &ch, 1);
}

// 向 sb 追加一个字符串 s
void strbuf_addstr(struct strbuf *sb, const char *s)
{
    if (sb == NULL || s == NULL) return;
    strbuf_add(sb, s, strlen(s));
}

// 向一个 sb 追加另一个 strbuf 的数据
void strbuf_addbuf(struct strbuf *sb, const struct strbuf *sb2)
{
    if (sb == NULL || sb2 == NULL || sb2->len <= 0) return;
    strbuf_add(sb, sb2->buf, (size_t)sb2->len);
}

// 设置 sb 的长度 len
void strbuf_setlen(struct strbuf *sb, size_t len)
{
    if (sb == NULL) return;
    if (sb->alloc <= 0) {
        if (len == 0) sb->len = 0;
        return;
    }
    if (len > (size_t)sb->alloc - 1) return;
    
    sb->len = (int)len;
    if (sb->buf != NULL) {
        sb->buf[sb->len] = '\0';
    }
}

// 计算 sb 目前仍可以向后追加的字符串长度
size_t strbuf_avail(const struct strbuf *sb)
{
    if (sb == NULL || sb->buf == NULL || sb->alloc <= 0) return 0;
    if (sb->len < 0 || sb->len >= sb->alloc) return 0;
    return (size_t)(sb->alloc - sb->len - 1);
}

// 向 sb 内存坐标为 pos 位置插入长度为 len 的数据 data
void strbuf_insert(struct strbuf *sb, size_t pos, const void *data, size_t len)
{
    if (sb == NULL || data == NULL || len == 0) return;
    if (len > (size_t)INT_MAX - (size_t)sb->len) return;
    if (pos > (size_t)sb->len) pos = (size_t)sb->len;
    
    char *tmp = NULL;
    const char *src;
    bool inside = false;
    
    if (sb->buf != NULL && sb->alloc > 0) {
        uintptr_t begin = (uintptr_t)(void*)sb->buf;
        uintptr_t addr = (uintptr_t)data;
        if (addr >= begin && addr < begin + (uintptr_t)(size_t)sb->alloc) {
            inside = true;
        }
    }
    
    if (inside) {
        tmp = (char *)malloc(len);
        if (tmp == NULL) return;
        memcpy(tmp, data, len);
        src = tmp;
    } else {
        src = (const char*)data;
    }
    
    strbuf_grow(sb, len);
    memmove(sb->buf + pos + len, sb->buf + pos, (size_t)sb->len - pos + 1);
    memmove(sb->buf + pos, src, len);
    sb->len += (int)len;
    sb->buf[sb->len] = '\0';
    
    free(tmp);
}

// 去除 sb 缓冲区左端的所有空格、制表符和'\t'字符
void strbuf_ltrim(struct strbuf *sb)
{
    if (sb == NULL || sb->buf == NULL || sb->len <= 0) return;
    
    size_t pos = 0;
    while (pos < (size_t)sb->len && isspace((unsigned char)sb->buf[pos])) {
        pos++;
    }
    if (pos == 0) return;
    
    size_t remaining = (size_t)sb->len - pos;
    memmove(sb->buf, sb->buf + pos, remaining + 1);
    sb->len = (int)remaining;
}

// 去除 sb 缓冲区右端的所有空格、制表符和'\t'字符
void strbuf_rtrim(struct strbuf *sb)
{
    if (sb == NULL || sb->buf == NULL || sb->len <= 0) return;
    
    int i = sb->len - 1;
    while (i >= 0 && isspace((unsigned char)sb->buf[i])) {
        i--;
    }
    sb->len = i + 1;
    sb->buf[sb->len] = '\0';
}

// 删除 sb 缓冲区从 pos 坐标开始长度为 len 的内容
void strbuf_remove(struct strbuf *sb, size_t pos, size_t len)
{
    if (sb == NULL || sb->buf == NULL || sb->len <= 0 || len == 0) return;
    if (pos >= (size_t)sb->len) return;
    
    size_t remaining = (size_t)sb->len - pos;
    if (len > remaining) len = remaining;
    
    memmove(sb->buf + pos, sb->buf + pos + len, remaining - len + 1);
    sb->len -= (int)len;
}

// 将文件描述符为 fd 的所有文件内容追加到 sb 中，sb 增长 hint ? hint : 8192 大小
ssize_t strbuf_read(struct strbuf *sb, int fd, size_t hint)
{
    if (sb == NULL || fd < 0) return -1;
    
    if (hint == 0) hint = 8192;
    if (hint > (size_t)UINT_MAX) hint = (size_t)UINT_MAX;
    size_t total = 0;
    
    for (;;) {
        strbuf_grow(sb, hint);
        ssize_t n = read(fd, sb->buf + sb->len, (unsigned int)hint);
        
        if (n < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (n == 0) break;
        
        if ((size_t)n > (size_t)INT_MAX - (size_t)sb->len) return -1;
        strbuf_setlen(sb, (size_t)sb->len + (size_t)n);
        total += (size_t)n;
    }
    return (ssize_t)total;
}

// 将文件句柄为 fp 的一行内容（抛弃换行符）读取到 sb
int strbuf_getline(struct strbuf *sb, FILE *fp)
{
    if (sb == NULL || fp == NULL) return -1;
    
    strbuf_reset(sb);
    int ch;
    bool got_any = false;
    
    while ((ch = fgetc(fp)) != EOF) {
        got_any = true;
        if (ch == '\n') {
            if (sb->len > 0 && sb->buf[sb->len - 1] == '\r') {
                strbuf_setlen(sb, (size_t)sb->len - 1);
            }
            return (int)sb->len;
        }
        strbuf_addch(sb, ch);
    }
    
    if (ferror(fp)) return -1;
    if (!got_any) return -1; 
    
    // 处理文件最后一行没换行符的情况
    if (sb->len > 0 && sb->buf[sb->len - 1] == '\r') {
        strbuf_setlen(sb, (size_t)sb->len - 1);
    }
    return (int)sb->len;
}

// 实现字符串切割
struct strbuf **strbuf_split_buf(const char *str, size_t len, int terminator, int max)
{
    struct strbuf **ret = NULL;
    size_t nr = 0;
    size_t alloc = 0;
    size_t start = 0;
    size_t cuts = 0;

    if (str == NULL && len > 0) return NULL;
    if (str == NULL) len = 0;

    // 空串保留一个空元素，和原 f.c 的 splitBuf1 行为一致
    if (len == 0) {
        struct strbuf *sb = (struct strbuf *)malloc(sizeof(*sb));
        if (sb == NULL) return NULL;
        ret = (struct strbuf **)malloc(2 * sizeof(*ret));
        if (ret == NULL) {
            free(sb);
            return NULL;
        }
        strbuf_init(sb, 1);
        ret[0] = sb;
        ret[1] = NULL;
        return ret;
    }

    while (start < len) {
        size_t end;
        size_t piece_len;
        struct strbuf *sb;

        // 跳过连续的分隔符，不产生空字符串
        while (start < len && str[start] == terminator) start++;
        if (start >= len) break;

        // max 表示“最多再切 max 刀”，所以最多得到 max + 1 块
        if (max > 0 && cuts >= (size_t)max) {
            end = len;
        } else {
            const char *p = (const char *)memchr(str + start, terminator, len - start);
            end = p != NULL ? (size_t)(p - str) : len;
        }

        piece_len = end - start;

        if (nr + 2 > alloc) {
            size_t new_alloc = alloc == 0 ? 4 : alloc * 2;
            while (new_alloc < nr + 2) new_alloc *= 2;

            struct strbuf **new_ret = (struct strbuf **)realloc(ret, new_alloc * sizeof(*ret));
            if (new_ret == NULL) goto cleanup;
            ret = new_ret;
            alloc = new_alloc;
        }

        sb = (struct strbuf *)malloc(sizeof(*sb));
        if (sb == NULL) goto cleanup;

        strbuf_init(sb, piece_len + 1);
        strbuf_add(sb, str + start, piece_len);
        ret[nr++] = sb;

        if (end >= len) break;
        start = end + 1;
        cuts++;
    }

    if (ret == NULL) {
        ret = (struct strbuf **)malloc(sizeof(*ret));
        if (ret == NULL) return NULL;
    }
    ret[nr] = NULL;
    return ret;

cleanup:
    if (ret != NULL) {
        for (size_t i = 0; i < nr; i++) {
            if (ret[i] != NULL) {
                strbuf_release(ret[i]);
                free(ret[i]);
            }
        }
        free(ret);
    }
    return NULL;
}

// 实现判断一个 strbuf 是否以指定字符串开头的功能
bool strbuf_begin_judge(char *target_str, const char *str, int target_len)
{
    size_t prefix_len;
    if (target_str == NULL || str == NULL || target_len < 0) return false;

    prefix_len = strlen(str);
    if (prefix_len > (size_t)target_len) return false;
    if (prefix_len == 0) return true;

    return memcmp(target_str, str, prefix_len) == 0;
}

// 获取字符串从坐标 [begin, end) 的所有内容
char *strbuf_get_mid_buf(char *target_buf, int begin, int end, int len)
{
    if (target_buf == NULL || begin < 0 || end < 0 || len < 0) return NULL;
    if (begin > end || end > len) return NULL;
    
    size_t n = (size_t)(end - begin);
    char *result = (char *)malloc(n + 1);
    if (result == NULL) return NULL;
    
    if (n > 0) {
        memcpy(result, target_buf + begin, n);
    }
    result[n] = '\0';
    return result;
}