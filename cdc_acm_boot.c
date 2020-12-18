/*
 * Copyright (C) 2018 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys
 * @{
 *
 * @file
 * @brief CDC ACM stdio implementation for USBUS CDC ACM
 *
 * This file implements a USB CDC ACM callback and read/write functions.
 *
 *
 * @}
 */

#define USB_H_USER_IS_RIOT_INTERNAL

#include "log.h"
#include "isrpipe.h"

#include "usb/usbus.h"
#include "usb/usbus/cdc/acm.h"

static usbus_cdcacm_device_t cdcacm_bootload;
static uint8_t _cdc_tx_bootload_buf_mem[CONFIG_USBUS_CDC_ACM_STDIO_BUF_SIZE];
static uint8_t _cdc_rx_bootload_buf_mem[CONFIG_USBUS_CDC_ACM_STDIO_BUF_SIZE];
static isrpipe_t _cdc_bootload_isrpipe = ISRPIPE_INIT(_cdc_rx_bootload_buf_mem);
isrpipe_t *_cdc_bootload_isrpipe_ptr = &_cdc_bootload_isrpipe;

size_t _cdc_acm_bootload_rx_pipe(usbus_cdcacm_device_t *cdcacm,
                             uint8_t *data, size_t len)
{
    (void)cdcacm;
    gpio_set(LED0_PIN);
    size_t i;
    for (i = 0; i < len; i++) {
        if(isrpipe_write_one(&_cdc_bootload_isrpipe, data[i]) < 0)
          break;
    }
    return i;
}

void usb_cdc_acm_bootload_init(usbus_t *usbus)
{
    (void) _cdc_bootload_isrpipe_ptr;
    usbus_cdc_acm_init(usbus, &cdcacm_bootload, _cdc_acm_bootload_rx_pipe, NULL,
                       _cdc_tx_bootload_buf_mem, sizeof(_cdc_tx_bootload_buf_mem));
}
