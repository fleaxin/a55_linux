/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "ot_osal.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/string.h>
#include <linux/version.h>

int osal_strcmp(const char *cs, const char *ct)
{
    return strcmp(cs, ct);
}
EXPORT_SYMBOL(osal_strcmp);
int osal_strncmp(const char *cs, const char *ct, int count)
{
    return strncmp(cs, ct, count);
}
EXPORT_SYMBOL(osal_strncmp);
char *osal_strstr(const char *s1, const char *s2)
{
    return strstr(s1, s2);
}
EXPORT_SYMBOL(osal_strstr);
int osal_strlen(const char *s)
{
    return strlen(s);
}
EXPORT_SYMBOL(osal_strlen);
int osal_strnlen(const char *s, int count)
{
    return strnlen(s, count);
}
EXPORT_SYMBOL(osal_strnlen);
char *osal_strsep(char **s, const char *ct)
{
    return strsep(s, ct);
}
EXPORT_SYMBOL(osal_strsep);
int osal_memcmp(const void *cs, const void *ct, int count)
{
    return memcmp(cs, ct, count);
}
EXPORT_SYMBOL(osal_memcmp);
long osal_strtol(const char *cp, char **endp, unsigned int base)
{
    return simple_strtol(cp, endp, base);
}
EXPORT_SYMBOL(osal_strtol);
