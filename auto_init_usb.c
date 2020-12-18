/*
 * Copyright (C) 2018 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup     sys_auto_init
 * @{
 * @file
 * @brief       initializes USBUS, usb devices and handlers
 *
 * This auto initialization for USBUS is designed to cover the common use case
 * of a single USB peripheral. An USBUS instance is started with USB function
 * handlers based on which module is compiled in.
 *
 * If this doesn't suit your use case, a different initialization function can
 * to be created based on this initialization sequence.
 *
 * @author  Koen Zandberg <koen@bergzand.net>
 * @}
 */

#define USB_H_USER_IS_RIOT_INTERNAL

#include "usb/usbus.h"
#include "usb/usbopt.h"
#include "periph/usbdev.h"
#include "stdio.h"

#ifdef MODULE_USBUS_CDC_ECM
#include "usb/usbus/cdc/ecm.h"
usbus_cdcecm_device_t cdcecm;
#endif
#ifdef MODULE_USBUS_CDC_ACM
#include "usb/usbus/cdc/acm.h"
#endif

static char _stack[USBUS_STACKSIZE];
static usbus_t usbus;
usbus_t *usbus_ptr = &usbus;
extern void usb_cdc_acm_bootload_init(usbus_t *usbus);

void auto_init_usb(void)
{
    /* Get driver context */
    usbdev_t *usbdev = usbdev_get_ctx(0);
    assert(usbdev);

    /* Initialize basic usbus struct, don't start the thread yet */
    usbus_init(&usbus, usbdev);

    /* USBUS function handlers initialization */
#ifdef MODULE_STDIO_CDC_ACM
    void usb_cdc_acm_stdio_init(usbus_t *usbus);
    usb_cdc_acm_stdio_init(&usbus);
#endif

#ifdef MODULE_USBUS_CDC_ECM
    usbus_cdcecm_init(&usbus, &cdcecm);
#endif
    /* Bootloader endpoints */
    // usb_cdc_acm_bootload_init(&usbus);

    /* Finally initialize USBUS thread */
    usbus_create(_stack, USBUS_STACKSIZE, USBUS_PRIO, USBUS_TNAME, &usbus);
    usbus_ptr = &usbus;
    // int max_idx = 0;
    // for(usbus_interface_t *iface = usbus.iface; iface; iface = iface->next ) {
    //   max_idx = iface->idx;
    // }

    // usbus_cdcacm_device_t *usb_stdio = (usbus_cdcacm_device_t *) usbus.handlers;
    // usbus_interface_t *boot_if1 = usbus.iface->next->next;
    // usbus_interface_t *boot_if2 = boot_if1->next;
    // usbus_endpoint_t *one = usbus_interface_find_endpoint(boot_if1, USB_EP_TYPE_INTERRUPT, USB_EP_DIR_IN);
    // usbus_endpoint_t *two = usbus_interface_find_endpoint(boot_if2, USB_EP_TYPE_BULK, USB_EP_DIR_IN);
    // usbus_endpoint_t *three = usbus_interface_find_endpoint(boot_if2, USB_EP_TYPE_BULK, USB_EP_DIR_OUT);
    // usbus_disable_endpoint(one);
    // usbus_disable_endpoint(two);
    // usbus_disable_endpoint(three);

    // static const usbopt_enable_t _enable = USBOPT_ENABLE;
    // usbdev_set(usbus.dev, USBOPT_ATTACH, &_enable,
    //             sizeof(usbopt_enable_t));

    // #ifdef MODULE_STDIO_CDC_ACM
    // printf("Max if: %d\n", max_idx);
    // #endif
    // printf("if1: %d\nif2: %d\n", (int)boot_if1->idx, (int)boot_if2->idx);
    // printf("one: %d\ntwo: %d\nthree: %d\n", (int) one, (int)two, (int)three);

}
