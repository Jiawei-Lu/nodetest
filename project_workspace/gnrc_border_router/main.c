/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example application for demonstrating the RIOT network stack
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "msg.h"
#include "net/gnrc/rpl.h"
#include "net/gnrc/rpl/structs.h"
#include "net/gnrc/rpl/dodag.h"
#include "common.h"

#define MAIN_QUEUE_SIZE     (8U)
at86rf2xx_t devs[AT86RF2XX_NUM];
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];


int main(void)
{
    
    at86rf2xx_set_txpower(devs, 1);
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    
    
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT border router example application");

    

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
