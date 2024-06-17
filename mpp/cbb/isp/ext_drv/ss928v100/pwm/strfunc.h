/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef STRFUNC_H
#define STRFUNC_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define STRFMT_ADDR32   "%#010lX"
#define STRFMT_ADDR32_2 "0x%08lX"

extern int str_to_number(char *str, unsigned int *ul_value);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* STRFUNC_H */
