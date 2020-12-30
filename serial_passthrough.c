/*
 * Copyright (C) 2020 Eric Bassett
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       Application controller serial passthrough to allow comms with NINA-W10x
 *
 * @author      Eric Bassett <eric.bassett@outlook.com>
 *
 * @}
 */

#include "xtimer.h"
#include <stdio.h>
#include "stdio_base.h"
#include "periph/uart.h"
#include "periph/usbdev.h"
#include <string.h>
#include "isrpipe.h"
#include "usb/usbus.h"
#include "usb/usbus/cdc/acm.h"
#include "periph/gpio.h"
// #include "usb/usbus/cdc/acm.h"
#include "isrpipe/read_timeout.h"

#ifndef MODULE_AUTO_INIT_USBUS
extern usbus_t *usbus_ptr;
#else
usbus_t *usbus_ptr = NULL;
#endif

extern void uart_create(usbus_t *usbus);
extern void usb_echo_create(void);
// extern isrpipe_t *_cdc_bootload_isrpipe_ptr;
// extern void _cdc_acm_bootload_rx_pipe(usbus_cdcacm_device_t *cdcacm,
//                              uint8_t *data, size_t len);

// static uint8_t _uart_stdio_buf_mem[CONFIG_USBUS_CDC_ACM_STDIO_BUF_SIZE];
// static isrpipe_t _uart_stdio_isrpipe = ISRPIPE_INIT(_uart_stdio_buf_mem);  
// extern isrpipe_t *_boot_isrpipe_ptr;                           

#define NINA_GPIO0_PIN    GPIO_PIN(PA, 27)
#define NINA_RESETN_PIN   GPIO_PIN(PA, 8)
#define NINA_UART         UART_DEV(1)
#define NINA_UART_BAUD    115200
#define NINA_ACK_PIN      GPIO_PIN(PA, 28)

// void ser_print(void *arg, uint8_t data) {
//   (void) arg;  
//   // stdio_write(&data, 1);
//   isrpipe_write_one(&_uart_stdio_isrpipe, data);
// }

int ser_pin_init(void) {
  // initialize LED/ACK loop pins
  gpio_init(LED0_PIN, GPIO_OUT);
  gpio_init(NINA_ACK_PIN, GPIO_IN);

  // init GPIO0 (boot into bootloader pin)
  // and RESETN (active low reset pin)
  gpio_init(NINA_GPIO0_PIN, GPIO_OUT);
  gpio_init(NINA_RESETN_PIN, GPIO_OUT);

  return 0;
}

int ser_nina_restart(void) {
  // reboot
  // gpio_set(NINA_GPIO0_PIN);
  // gpio_clear(NINA_RESETN_PIN);
  // xtimer_usleep(10000);

  // reboot into bootloader
  // gpio_(NINA_GPIO0_PIN);
  // gpio_clear(NINA_RESETN_PIN);
  // xtimer_usleep(10000);
  gpio_clear(NINA_RESETN_PIN);
  gpio_clear(NINA_GPIO0_PIN);
  return 0;
}

// int ser_init(void) {
//   // initialize UART
//   return uart_init(NINA_UART, NINA_UART_BAUD, ser_print, NULL);
// }

// int ser_loop() {

// }

int ser_pass_cmd(int argc, char **argv)
{
  if (argc > 2) {
    printf("usage: %s\n", argv[0]);
    return 1;
  }
  if (argc == 2) {
    printf("usb echo\n");
    usb_echo_create();
    thread_sleep();
  } else {



    ser_pin_init();
    // ser_init();
    ser_nina_restart();

    // static const usbopt_enable_t _disable = USBOPT_DISABLE;
    // static const usbopt_enable_t _enable = USBOPT_ENABLE;
    // usbdev_set(usbus_ptr->dev, USBOPT_ATTACH, &_disable,
    //             sizeof(usbopt_enable_t));
    
    // xtimer_sleep(2);
    // // usbus_cdcacm_device_t *stdio_dev = (usbus_cdcacm_device_t *) usbus_ptr->handlers;
    // // stdio_dev->cb = _cdc_acm_bootload_rx_pipe;
    // xtimer_sleep(2);
    
    // // usbus_interface_t *stdio_if1 = usbus_ptr->iface;
    // // usbus_interface_t *stdio_if2 = usbus_ptr->iface->next;
    // // usbus_endpoint_t *one = usbus_interface_find_endpoint(stdio_if1, USB_EP_TYPE_INTERRUPT, USB_EP_DIR_IN);
    // // usbus_endpoint_t *two = usbus_interface_find_endpoint(stdio_if2, USB_EP_TYPE_BULK, USB_EP_DIR_IN);
    // // usbus_endpoint_t *three = usbus_interface_find_endpoint(stdio_if2, USB_EP_TYPE_BULK, USB_EP_DIR_OUT);

    // // usbus_disable_endpoint(one);
    // // usbus_disable_endpoint(two);
    // // usbus_disable_endpoint(three);  

    // // usbus_interface_t *boot_if1 = usbus_ptr->iface->next->next;
    // // usbus_interface_t *boot_if2 = boot_if1->next;
    // // one = usbus_interface_find_endpoint(boot_if1, USB_EP_TYPE_INTERRUPT, USB_EP_DIR_IN);
    // // two = usbus_interface_find_endpoint(boot_if2, USB_EP_TYPE_BULK, USB_EP_DIR_IN);
    // // three = usbus_interface_find_endpoint(boot_if2, USB_EP_TYPE_BULK, USB_EP_DIR_OUT);
    // // usbus_enable_endpoint(one);
    // // usbus_enable_endpoint(two);
    // // usbus_enable_endpoint(three);  

    // usbdev_set(usbus_ptr->dev, USBOPT_ATTACH, &_enable,
    //         sizeof(usbopt_enable_t));

    uart_create(usbus_ptr);
    thread_sleep();

  }
  return 0;
}