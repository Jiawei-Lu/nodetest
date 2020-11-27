#include <stdio.h>
#include "shell.h"
#include "msg.h"
#include "periph/pm.h"
#define ENABLE_DEBUG (0)
#include "debug.h"
#include "kernel_types.h"
#include "net/gnrc/netif.h"
#include "net/ipv6/addr.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];
extern int nanotest_client_cmd(int argc, char **argv);

// static int check_mode(int argc, char **argv)
// {
//     if (argc < 2) {
//         printf("Usage: %s <power mode>\n", argv[0]);
//         return -1;
//     }

//     return 0;
// }

// static int parse_mode(char *argv1)
// {
//     uint8_t mode = atoi(argv1);

//     if (mode >= PM_NUM_MODES) {
//         printf("Error: power mode not in range 0 - %d.\n", PM_NUM_MODES - 1);
//         return -1;
//     }

//     return mode;
// }

// static int cmd_set(int argc1, char **argv1)
// {
//     if (check_mode(argc1, argv1) != 0) {
//         return 1;
//     }

//     int mode = parse_mode(argv1[1]);

//     if (mode < 0) {
//         return 1;
//     }

//     printf("CPU will enter power mode %d.\n", mode);
//     fflush(stdout);

//     pm_set(mode);

//     return 0;
// }

// static int cmd_off(int argc, char **argv)
// {
//     (void) argc;
//     (void) argv;

//     puts("CPU will turn off.");
//     fflush(stdout);

//     pm_off();

//     return 0;
// }
// static const shell_command_t shell_commands[] = {
//     { "udp", "send data over UDP and listen on UDP ports", cmd_set },
//     { "mac", "get MAC protocol's internal information", cmd_off },
//     { NULL, NULL, NULL }
// };
int main(void)
{
    /* for the thread running the shell */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("nanocoap client test app");
    // int argc1 = 2; 
    // char *argv1[] = {"set", "0"};
    // cmd_set(argc1,argv1);
    // int deep = 1;
    // sam0_cortexm_sleep(deep);
    /* start shell */
    puts("All up");
    int argc = 5; 
    char *argv[] = {"put", "fd00:dead:beef::1", "5683", "basic","tem"};
    // char *argv[] = {"client", "put", "fd00:dead:beef::1", "5683", "basic","tem"};
    nanotest_client_cmd(argc, argv);
    puts("Sleeping");
    xtimer_sleep(100);

    // char line_buf[SHELL_DEFAULT_BUFSIZE];
    // shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    /* should never be reached */
    return 0;
}
