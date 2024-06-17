/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef PCI_TRANS_H
#define PCI_TRANS_H

#include <linux/ioctl.h>
#include "ot_type.h"

#define OT_PCIT_MAX_BUS     2
#define OT_PCIT_MAX_SLOT    5

#define OT_PCIT_DMA_READ    0
#define OT_PCIT_DMA_WRITE   1

#define OT_PCIT_HOST_BUSID  0
#define OT_PCIT_HOST_SLOTID 0

#define OT_PCIT_NOSLOT    (-1)

#define OT_PCIT_INVAL_SUBSCRIBER_ID (-1)

#define ot_pcit_dev_to_bus(dev)  ((dev) >> 16)
#define ot_pcit_dev_to_slot(dev) ((dev) & 0xff)
#define ot_pcit_mkdev(bus, slot) ((slot) | ((bus) << 16))

#define PCIT_MAX_SUBSCRIBER_NAME_SIZE 16

struct pcit_dma_req {
    td_u32 bus;   /* bus and slot will be ignored on device */
    td_u32 slot;
    td_ulong dest;
    td_ulong source;
    td_u32 len;
};

struct pcit_dev_cfg {
    td_u32 slot;
    td_u16 vendor_id;
    td_u16 device_id;

    td_ulong np_phys_addr;
    td_ulong np_size;

    td_ulong pf_phys_addr;
    td_ulong pf_size;

    td_ulong cfg_phys_addr;
    td_ulong cfg_size;
};

struct pcit_bus_dev {
    td_u32 bus_nr; /* input argument */
    struct pcit_dev_cfg devs[OT_PCIT_MAX_SLOT];
};

#define OT_PCIT_EVENT_DOORBELL_0 0
#define OT_PCIT_EVENT_DOORBELL_1 1
#define OT_PCIT_EVENT_DOORBELL_2 2
#define OT_PCIT_EVENT_DOORBELL_3 3
#define OT_PCIT_EVENT_DOORBELL_4 4
#define OT_PCIT_EVENT_DOORBELL_5 5
#define OT_PCIT_EVENT_DOORBELL_6 6
#define OT_PCIT_EVENT_DOORBELL_7 7
#define OT_PCIT_EVENT_DOORBELL_8 8
#define OT_PCIT_EVENT_DOORBELL_9 9
#define OT_PCIT_EVENT_DOORBELL_10 10
#define OT_PCIT_EVENT_DOORBELL_11 11
#define OT_PCIT_EVENT_DOORBELL_12 12
#define OT_PCIT_EVENT_DOORBELL_13 13
#define OT_PCIT_EVENT_DOORBELL_14 14
#define OT_PCIT_EVENT_DOORBELL_15 15 /* reserved by mmc */

#define OT_PCIT_EVENT_DMARD_0 16
#define OT_PCIT_EVENT_DMAWR_0 17

#define OT_PCIT_PCI_NP  1
#define OT_PCIT_PCI_PF  2
#define OT_PCIT_PCI_CFG 3
#define OT_PCIT_AHB_PF  4

struct pcit_event {
    td_u32      event_mask;
    td_ulong    pts;
};

#define    OT_IOC_PCIT_BASE    'H'

/* Only used in host, you can get information of all devices on each bus. */
#define    OT_IOC_PCIT_INQUIRE _IOR(OT_IOC_PCIT_BASE, 1, struct pcit_bus_dev)

/* Only used in device, these tow command will block until DMA compeleted. */
#define    OT_IOC_PCIT_DMARD  _IOW(OT_IOC_PCIT_BASE, 2, struct pcit_dma_req)
#define    OT_IOC_PCIT_DMAWR  _IOW(OT_IOC_PCIT_BASE, 3, struct pcit_dma_req)

/*
 * Only used in host, you can bind a fd returned by open() to a device,
 * then all operation by this fd is orient to this device.
 */
#define    OT_IOC_PCIT_BINDDEV  _IOW(OT_IOC_PCIT_BASE, 4, int)

/*
 * Used in host and device.
 * on host, you should specify which device doorbell will be send to
 * by the parameter, but in device, the parameters will be ignored,and
 * a doorbell will be send to host.
 */
#define    OT_IOC_PCIT_DOORBELL  _IOW(OT_IOC_PCIT_BASE, 5, int)

/*
 * Used in host and device.
 * you can subscribe more than one event using this command.
 * on host, fd passed to ioctl() indicates target device. so you should
 * use "OT_IOC_PCIT_BINDDEV" bind a fd first, otherwise an error will
 * be met. but on device, all events are triggered by host, so it NOT
 * needed to bind a fd.
 */
#define    OT_IOC_PCIT_SUBSCRIBE  _IOW(OT_IOC_PCIT_BASE, 6, int)

/* If nscribled all events, diriver will return NONE event to all listeners. */
#define    OT_IOC_PCIT_UNSUBSCRIBE  _IOW(OT_IOC_PCIT_BASE, 7, int)

/* On host, this command will listen the device specified by parameter. */
#define    OT_IOC_PCIT_LISTEN  _IOR(OT_IOC_PCIT_BASE, 8, struct pcit_event)

#ifdef __KERNEL__
#include <linux/list.h>

struct pcit_dma_task {
    struct list_head list;   /* internal data, don't touch */

    td_u32      state;      /* internal data, don't touch. 0: todo, 1: doing, 2: finished */
    td_u32      dir; /* read or write */
    td_ulong    src;
    td_ulong    dest;
    td_u32      len;
    td_void        *private_data;
    td_void        (*finish)(struct pcit_dma_task *task);
};

/* only used in device */
td_u32 pcit_create_task(struct pcit_dma_task *task);

/* only used in host */
struct pcit_subscriber {
    td_char name[PCIT_MAX_SUBSCRIBER_NAME_SIZE];
    td_void (*notify)(td_u32 bus, td_u32 slot, struct pcit_event *, td_void *);
    td_void *data;
};

td_u32 pcit_subscriber_register(struct pcit_subscriber *user);
td_u32 pcit_subscriber_deregister(td_u32 id);
td_u32 pcit_subscribe_event(td_u32 id, td_u32 bus, td_u32 slot, struct pcit_event *event);
td_u32 pcit_unsubscribe_event(td_u32 id, td_u32 bus, td_u32 slot, struct pcit_event *event);
td_void ss_doorbell_triggle(td_u32 addr, td_u32 value);
td_ulong get_pf_window_base(td_u32 slot);

#endif /* #ifdef __KERNEL__ */
#endif /* #ifndef PCI_TRANS_H */
