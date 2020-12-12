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
 * @brief       Application controller program
 *
 * @author      Eric Bassett <eric.bassett@outlook.com>
 *
 * @}
 */

#include <stdio.h>
#include "led.h"
#include "xtimer.h"
#include "shell.h"
#include "thread.h"
#include "periph/spi.h"
#include "periph_cpu.h"
#include "nina_w102.h"
#include "nina_w102_params.h"
#include <string.h>

extern nina_w102_t nina_w102_dev;

char blink_thread_stack[THREAD_STACKSIZE_TINY];
void *blink_thread(void *arg)
{
  if (arg) {};
  while(true) {
    LED_TOGGLE(0);
    xtimer_sleep(1);
  }
  return NULL;
}

char line_buf[SHELL_DEFAULT_BUFSIZE];
char shell_thread_stack[THREAD_STACKSIZE_MAIN];

void *shell_thread(void *arg)
{
  if (arg) {};
  shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);
  return NULL;
}


int main(void)
{
  int len = 10;
  char firmware[50];
  char networks[32*20];
  const char *ssid = "4210_2.4GHz";
  const char *pass = "kayaeggokaroabe";

  // int num_networks;
  // int rows = 5, cols = 10;
  xtimer_sleep(3);
  nina_w102_t *wifi = &nina_w102_dev;
  nina_w102_init(wifi, nina_w102_params);
  xtimer_sleep(2);
  len = nina_w102_get_mac_id(wifi, firmware);
  printf("MAC address (maybe): ");
  for(int i = 0; i < len; i++) {
    printf("%X", firmware[i]);
  }
  printf("\n");
  printf("MAC address length (maybe): %d\n", len);  
  xtimer_sleep(2);
  // uint8_t r = nina_w102_get_conn_stat(wifi);
  // printf("Connection status: %#x\n", r);
  // xtimer_sleep(2);
  uint8_t r;

  len = nina_w102_get_fw_ver(wifi, firmware);  
  printf("Firmware version (maybe): %s\n", firmware);
  printf("Firmware version length (maybe): %d\n", len);
  xtimer_sleep(2);

  r = nina_w102_get_conn_stat(wifi);
  printf("Connection status: %#x\n", r);
  xtimer_sleep(2);

  len = nina_w102_get_fw_ver(wifi, firmware);  
  printf("Firmware version (maybe): %s\n", firmware);
  printf("Firmware version length (maybe): %d\n", len);

  if(firmware[1] == '\0') {
    printf("Firmware version error: %#x\n", (int) firmware[0]);
  }
  // printf("Command sequence:\n");
  // for(int x = 0; x < rows; x++) {
  //   for(int y = 0; y < cols; y++) {
  //     printf("%#x\t", firmware[x*cols + y]);
  //   }
  //   printf("\n");
  // }
  xtimer_sleep(2);
  nina_w102_start_scan_networks(wifi);
  printf("Start scanning networks\n");
  xtimer_sleep(10);
  uint8_t n_nets = nina_w102_get_scan_networks(wifi, (char **) networks, 20);

  for (int i = 0; i < n_nets; i++) {
    printf("Network[%d]: %s\n", i, networks + i*32);
  }

  // connect
  nina_w102_connect(wifi, ssid, strlen(ssid), pass, strlen(pass));
  wifi->connected = false;
  while (!wifi->connected) {
    printf("Attempting to connect to network: %s\n", ssid);
    xtimer_sleep(2);
    r = nina_w102_get_conn_stat(wifi);
    printf("response for connection: %#X\n", r);
    if (r == 3) wifi->connected = true;
  }

  printf("Connected to network with stat: %#X\n", r);



  thread_create(blink_thread_stack, sizeof(blink_thread_stack),
                THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
                blink_thread, NULL, "blink_thread");
  
  thread_create(shell_thread_stack, sizeof(shell_thread_stack),
                THREAD_PRIORITY_MAIN - 2, THREAD_CREATE_STACKTEST,
                shell_thread, NULL, "shell_thread");
              
  return 0;
}