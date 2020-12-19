#include "stdio_base.h"
#include "xtimer.h"
#include "periph/uart.h"
#include "periph/usbdev.h"
#include "isrpipe.h"
#include "usb/usbus.h"
#include "usb/usbus/cdc/acm.h"
// #include "event.h"
// #include "usb/usbus/cdc/acm.h"
#include "isrpipe/read_timeout.h"

#define NINA_GPIO0_PIN    GPIO_PIN(PA, 27)
#define NINA_RESETN_PIN   GPIO_PIN(PA, 8)
#define NINA_UART         UART_DEV(1)
#define NINA_UART_BAUD    115200
#define NINA_ACK_PIN      GPIO_PIN(PA, 28)
#define NINA_BUFF_SZ      (128 * 16)
#define RW_NUM            (128 * 16)

#define UART_THREAD_CDC_FULL_FLAG   (_U_(0x1) << 3)

extern usbus_t *usbus_ptr;
extern isrpipe_t *_boot_isrpipe_ptr;
extern isrpipe_t *_cdc_stdio_isrpipe_ptr;
extern void _cdc_acm_bootload_rx_pipe(usbus_cdcacm_device_t *cdcacm,
                             uint8_t *data, size_t len);
static char _stack[THREAD_STACKSIZE_LARGE*4];                             

static uint8_t _uart_stdio_buf_mem[NINA_BUFF_SZ];
static isrpipe_t _uart_stdio_isrpipe = ISRPIPE_INIT(_uart_stdio_buf_mem);

static void uart_print(void *arg, uint8_t data) {
  (void) arg;
  static int cnt;
  // stdio_write(&data, 1);
  cnt = isrpipe_write_one(&_uart_stdio_isrpipe, data);
  (void) cnt;
  if (cnt < 0) {
    gpio_set(LED0_PIN);
  }
}

static int chg_num(int n) {
  if (n > RW_NUM) {
    return RW_NUM;
  } else {
    return n;
  }
}

static void *_uart_thread(void *args) {
  // init  
  (void) _boot_isrpipe_ptr;
  usbus_t *usbus = (usbus_t *) args;
  usbus_cdcacm_device_t *cdcacm = (usbus_cdcacm_device_t *) usbus->handlers->next;
  static uint8_t usbr_uartw[NINA_BUFF_SZ];
  static uint8_t uartr_usbw[NINA_BUFF_SZ];
  // char *help_msg = "help\r\n";
  size_t num, len;
  num = tsrb_avail(&_cdc_stdio_isrpipe_ptr->tsrb);
  len = 0;
  if (num > 0) {
    len = stdio_read(usbr_uartw, num);      
  }

//   usbdev_ep_t *ep_bulk_out = cdcacm->iface_data.ep->next->ep;
//   int j = 0;
//   for (usbus_endpoint_t *ep = cdcacm->iface_data.ep; ep; ep = ep->next) {
//     printf("Endpoint %d dir %d type %d\n", j, ep->ep->dir, ep->ep->type);
//     j++;
//   }
// printf("out: %d and bulk: %d", USB_EP_DIR_OUT, USB_EP_TYPE_BULK);
//   printf("Actual dir: %d and type: %d\n", ep_bulk_out->dir, ep_bulk_out->type);
  

//   if((ep_bulk_out->dir == USB_EP_DIR_OUT) && (ep_bulk_out->type == USB_EP_TYPE_BULK))
//     printf("Endpoint is good");


  // read and clear uart
  // num = tsrb_avail(&_uart_stdio_isrpipe.tsrb);
  // if (num > 0) {
  //   isrpipe_read(&_uart_stdio_isrpipe, uartr_usbw, num);
  // }

  uart_init(NINA_UART, NINA_UART_BAUD, uart_print, NULL);
    // printf("UART OK\n");
  // int hd_ct = 1;
  // for (usbus_handler_t *hd = usbus->handlers; hd; hd = hd->next) {
    // printf("Handler addr: %d",(int) cdcacm);
  //   hd_ct++;
  // }
  // printf("CDC ACM: DTE UART enabled on handler %u\n", (int) cdcacm);
  printf("CDC max pkt size: %d\n", cdcacm->iface_data.ep->maxpacketsize);
  printf("EP is out %d and bulk %d\n", cdcacm->iface_data.ep->ep->dir == USB_EP_DIR_OUT,
          cdcacm->iface_data.ep->ep->type == USB_EP_TYPE_BULK);

  int i = 1;
  bool c0_came = false;
  (void) c0_came;
  bool pause_usb = false;
  bool pause_uart = false;
  bool esp32_term_cncted = false;
  bool esp32_done = false;
  (void) esp32_term_cncted;
  (void) esp32_done;
  gpio_clear(LED0_PIN);

  printf("Size of int %d int8_t %d and size_t %d\n", sizeof(int), sizeof(int8_t), sizeof(size_t));

  while(1) {
    if(!(i % 100000)) {
      // gpio_toggle(LED0_PIN);
    }
    if (esp32_term_cncted && !esp32_done) {
      tsrb_drop(&_uart_stdio_isrpipe.tsrb, NINA_BUFF_SZ);
      gpio_clear(LED0_PIN);
    }

    if(cdcacm->state == USBUS_CDC_ACM_LINE_STATE_CARRIER ||
       cdcacm->state == USBUS_CDC_ACM_LINE_STATE_CARRIER_DTE){
      gpio_clear(NINA_RESETN_PIN);
    } else {
      gpio_set(NINA_RESETN_PIN);
    }

    if(cdcacm->state == USBUS_CDC_ACM_LINE_STATE_CARRIER_DTE ||
       cdcacm->state == USBUS_CDC_ACM_LINE_STATE_DTE){
      gpio_clear(NINA_GPIO0_PIN);
      if (esp32_term_cncted) esp32_done = true;     
    } else {
      gpio_set(NINA_GPIO0_PIN);
      esp32_term_cncted = true;
    }

    // if (!(i % 1000)) {
    //   // printf("cdacm_state: %d\n", cdcacm->state);
    //   // gpio_set(LED0_PIN);
    // }

    // read usb
    num = tsrb_avail(&_cdc_stdio_isrpipe_ptr->tsrb);
    len = 0;
    if (num > 0) {
      num = chg_num(num);
      len = stdio_read(usbr_uartw, num);      
      if (num != len) {
        gpio_set(LED0_PIN);
      }   
    }

    // write uart
    if (len > 0) {
      uart_write(NINA_UART, usbr_uartw, len);
    }

    // read uart
    num = tsrb_avail(&_uart_stdio_isrpipe.tsrb);
    len = 0;
    if (num > 0 && !pause_usb) {
      num = chg_num(num);
      len = isrpipe_read(&_uart_stdio_isrpipe, uartr_usbw, num);
      if (len != num) {
        gpio_set(LED0_PIN);
      }     
      // if (c0_came && uartr_usbw[0] == 0xc0) gpio_set(LED0_PIN);
    }

    // if (num <= 0)
    //   uart_write(NINA_UART, (uint8_t *) help_msg, sizeof(help_msg));

    // write usb
    if (len > 0) {
      num = stdio_write(uartr_usbw, len);
      if (len != num) {
        gpio_set(LED0_PIN);
      }
    } 
    (void) pause_uart;
    (void) pause_usb;
    // if (len > 0) {      
    //   uint8_t *buffer = uartr_usbw;
    //   do {
    //       size_t n = usbus_cdc_acm_submit(cdcacm, buffer, len);
    //       // usbus_cdc_acm_flush(cdcacm);
    //       usbus_event_post(usbus, &cdcacm->flush);
    //       /* Use tsrb and flush */
    //       buffer = buffer + n;
    //       len -= n;
    //   } while (len);
    // }          

    // xtimer_usleep(5);
    i++;
  }
  return NULL;
}

void uart_create(usbus_t *usbus) {
  int res = thread_create(_stack, THREAD_STACKSIZE_LARGE*4, USBUS_PRIO+1, THREAD_CREATE_STACKTEST,
                          _uart_thread, (void *)usbus, "usb_to_uart");

  (void)res;
  assert(res > 0);
}