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

#include "shell.h"
#include "msg.h"

#define ENABLE_DEBUG 1

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern int ser_pass_cmd(int argc, char **argv);

extern void auto_init_usb(void);

static const shell_command_t shell_commands[] = {
    { "ninapass", "send data over UART to NINA W10x", ser_pass_cmd },
    { NULL, NULL, NULL }
};

int main(void)
{
    auto_init_usb();

    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    // ser_pass_cmd(1, NULL);
    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}