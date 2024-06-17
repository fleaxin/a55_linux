/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <linux/fs.h>
#include <asm/uaccess.h>
#include "ot_osal.h" /* because of ioctl redefine, ot_osal.h should not be the first included file */

struct file *klib_fopen(const char *filename, int flags, int mode)
{
    struct file *filp = filp_open(filename, flags, mode);
    return (IS_ERR(filp)) ? NULL : filp;
}

void klib_fclose(struct file *filp)
{
    if (filp != NULL) {
        filp_close(filp, NULL);
    }
    return;
}

int klib_fwrite(const char *buf, int len, struct file *filp)
{
    if (filp == NULL) {
        return -ENOENT;
    }

    return __kernel_write(filp, buf, len, &filp->f_pos);
}

int klib_fread(char *buf, unsigned int len, struct file *filp)
{
    if (filp == NULL) {
        return -ENOENT;
    }

    return kernel_read(filp, (void __user*)buf, len, &filp->f_pos);
}

void *osal_klib_fopen(const char *filename, int flags, int mode)
{
    return (void *)klib_fopen(filename, flags, mode);
}
EXPORT_SYMBOL(osal_klib_fopen);

void osal_klib_fclose(void *filp)
{
    klib_fclose((struct file *)filp);
}
EXPORT_SYMBOL(osal_klib_fclose);

int osal_klib_fwrite(const char *buf, int len, void *filp)
{
    return klib_fwrite(buf, len, (struct file *)filp);
}
EXPORT_SYMBOL(osal_klib_fwrite);

int osal_klib_fread(char *buf, unsigned int len, void *filp)
{
    return klib_fread(buf, len, (struct file *)filp);
}
EXPORT_SYMBOL(osal_klib_fread);

