#include <stdio.h>
#include "isrpipe.h"
#include "usb/usbus.h"
#include "stdio_base.h"
#include "usb/usbus/cdc/acm.h"
#include "periph/gpio.h"
#include "periph/spi.h"

static uint8_t stdio_rw_buff[128];
extern isrpipe_t *_cdc_stdio_isrpipe_ptr;
static char _stack[THREAD_STACKSIZE_LARGE*4];
extern usbus_t *usbus_ptr;

int numchar = 0;
int len = 0;

#define WIFI_NINA SPI_DEV(0)
#define WIFI_NINA_CS GPIO_PIN(PA, 14)

spi_conf_t *nina = &spi_config[0];

static void* usb_echo(void *args) {
    (void) args;
    usbus_t *usbus = usbus_ptr;
    usbus_cdcacm_device_t *cdcacm = (usbus_cdcacm_device_t *) usbus->handlers->next;
    usbdev_ep_t *ep_bulk_out = cdcacm->iface_data.ep->next->ep;
    usbdev_ep_t *ep_bulk_in = cdcacm->iface_data.ep->ep;
    (void) cdcacm;
    (void) ep_bulk_out;
    (void) ep_bulk_in;
    int i = 1;

    spi_init(WIFI_NINA);


    gpio_init(LED0_PIN, GPIO_OUT);
    while(true) {
        // if (!(i % 10000)) {gpio_toggle(LED0_PIN);}
        (void) i;
        numchar = tsrb_avail(&_cdc_stdio_isrpipe_ptr->tsrb);
        len = 0;
        if (numchar > 0) {
            // printf("\n\nread %d characters\n\n", numchar);
            len = stdio_read(stdio_rw_buff, numchar);
        }
        if (len > 0) {
            if (stdio_rw_buff[len - 1] == '\n') {}
            printf("\n\nwrite %d characters\n\n", len);
            spi_acquire(WIFI_NINA, WIFI_NINA_CS, SPI_MODE_0, SPI_CLK_100KHZ);
            spi_transfer_bytes(WIFI_NINA, WIFI_NINA_CS, true, stdio_rw_buff, NULL, len);
            spi_release(WIFI_NINA);
            // stdio_write(stdio_rw_buff, len);
        }
        i++;
    }
    return NULL;
}

void usb_echo_create(void) {
  int res = thread_create(_stack, THREAD_STACKSIZE_LARGE*4, USBUS_PRIO+1, THREAD_CREATE_STACKTEST,
                          usb_echo, NULL, "usb_echo");

  (void)res;
  assert(res > 0);
}