/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */


#ifndef __MMZ_H__
#define __MMZ_H__

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
#define  mmz_map_down(mm)  down_read(&mm->mmap_lock)
#define  mmz_map_up(mm)    up_read(&mm->mmap_lock)
#else
#define  mmz_map_down(mm)   down_read(&mm->mmap_sem)
#define  mmz_map_up(mm)    up_read(&mm->mmap_sem)
#endif

#endif
