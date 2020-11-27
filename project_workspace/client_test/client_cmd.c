// #include <stdint.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #define DS18_PARAM_PIN  GPIO_PIN(PA,18)
// #include "ds18.h"
// #include "ds18_params.h"
// #include "fmt.h"
// #include "net/coap.h"
// #include "net/nanocoap.h"
// #include "net/nanocoap_sock.h"
// #include "net/sock/udp.h"
// #include "od.h"

// extern ds18_t dev;
// static ssize_t _send(coap_pkt_t *pkt, size_t len, char *addr_str, char *port_str)
// {
//     ipv6_addr_t addr;
//     sock_udp_ep_t remote;

//     remote.family = AF_INET6;
    
//     /* parse for interface */
//     char *iface = ipv6_addr_split_iface(addr_str);
//     if (!iface) {
//         if (gnrc_netif_numof() == 1) {
//             /* assign the single interface found in gnrc_netif_numof() */
//             remote.netif = (uint16_t)gnrc_netif_iter(NULL)->pid;
//         }
//         else {
//             remote.netif = SOCK_ADDR_ANY_NETIF;
//         }
//     }
//     else {
//         int pid = atoi(iface);
//         if (gnrc_netif_get_by_pid(pid) == NULL) {
//             puts("nanocli: interface not valid");
//             return 0;
//         }
//         remote.netif = pid;
//     }

//     /* parse destination address */
//     if (ipv6_addr_from_str(&addr, addr_str) == NULL) {
//         puts("nanocli: unable to parse destination address");
//         return 0;
//     }
//     if ((remote.netif == SOCK_ADDR_ANY_NETIF) && ipv6_addr_is_link_local(&addr)) {
//         puts("nanocli: must specify interface for link local target");
//         return 0;
//     }
//     memcpy(&remote.addr.ipv6[0], &addr.u8[0], sizeof(addr.u8));

//     /* parse port */
//     remote.port = atoi(port_str);
//     if (remote.port == 0) {
//         puts("nanocli: unable to parse destination port");
//         return 0;
//     }

//     return nanocoap_request(pkt, NULL, &remote, len);
// }

// int nanotest_client_cmd(int argc, char **argv)      /* agrc 是输入参数个数 argv是输入参数/字符串的集合，空格区分每个元素*/
// {
//     /* Ordered like the RFC method code numbers, but off by 1. GET is code 0. */
//     // char *method_codes[] = {"get", "post", "put"};
//     unsigned buflen = 128;
//     uint8_t buf[buflen];
//     coap_pkt_t pkt;
//     size_t len;
//     uint8_t token[2] = {0xDA, 0xEC};
//     ssize_t p = 0;
//      char rsp[16];
//      int16_t temperature;
//      ds18_t dev;
//      /* get temp in celsius */
//      ds18_init(&dev, &ds18_params[0]);
//      gpio_set(DS18_PARAM_PIN);
//      if (ds18_get_temperature(&dev, &temperature) == DS18_OK) 
//      {
//          bool negative = (temperature < 0);
//          if (negative) {
//             temperature = -temperature;
//          }
//          float a = (float)temperature / 100;
//          p += fmt_float(rsp,a,2);
//      }
//     if (argc == 1) {
//         /* show help for commands */
//         goto end;
//     }

//     int code_pos = 2;
//     // for (size_t i = 0; i < ARRAY_SIZE(method_codes); i++) {
//     //     if (strcmp(argv[1], method_codes[i]) == 0) {           /*遍历寻找对应的操作*/
//     //         code_pos = i;
//     //     }
//     // }
//     // if (code_pos == -1) {
//     //     goto end;
//     // }

//     pkt.hdr = (coap_hdr_t *)buf;

//     /* parse options */
//     if (argc == 5 || argc == 6) {
//         ssize_t hdrlen = coap_build_hdr(pkt.hdr, COAP_TYPE_CON, &token[0], 2,
//                                         code_pos+1, 1);
//         coap_pkt_init(&pkt, &buf[0], buflen, hdrlen);
//         coap_opt_add_string(&pkt, COAP_OPT_URI_PATH, argv[4], '/');
//         if (argc == 6) {
//             coap_opt_add_uint(&pkt, COAP_OPT_CONTENT_FORMAT, COAP_FORMAT_TEXT);
//             len = coap_opt_finish(&pkt, COAP_OPT_FINISH_PAYLOAD);

//             pkt.payload_len = strlen(rsp);
//             memcpy(pkt.payload,(uint8_t*)rsp , p);
//             len += pkt.payload_len;
//         }
//         else {
//             len = coap_opt_finish(&pkt, COAP_OPT_FINISH_NONE);
//         }

//         printf("nanocli: sending msg ID %u, %u bytes\n", coap_get_id(&pkt),
//                (unsigned) len);

//         ssize_t res = _send(&pkt, buflen, argv[2], argv[3]);
//         if (res < 0) {
//             printf("nanocli: msg send failed: %d\n", (int)res);
//         }
//         else {
//             char *class_str = (coap_get_code_class(&pkt) == COAP_CLASS_SUCCESS)
//                                     ? "Success" : "Error";
//             printf("nanocli: response %s, code %1u.%02u", class_str,
//                    coap_get_code_class(&pkt), coap_get_code_detail(&pkt));
//             if (pkt.payload_len) {
//                 unsigned format = coap_get_content_type(&pkt);
//                 if (format == COAP_FORMAT_TEXT
//                         || format == COAP_FORMAT_LINK
//                         || coap_get_code_class(&pkt) == COAP_CLASS_CLIENT_FAILURE
//                         || coap_get_code_class(&pkt) == COAP_CLASS_SERVER_FAILURE) {
//                     /* Expecting diagnostic payload in failure cases */
//                     printf(", %u bytes\n%.*s\n", pkt.payload_len, pkt.payload_len,
//                                                                   (char *)pkt.payload);
//                 }
//                 else {
//                     printf(", %u bytes\n", pkt.payload_len);
//                     od_hex_dump(pkt.payload, pkt.payload_len, OD_WIDTH_DEFAULT);
//                 }
//             }
//             else {
//                 printf(", empty payload\n");
//             }
//         }
//         return 0;
//     }

//     end:
//     printf("usage: %s <get|post|put> <addr>[%%iface] <port> <path> [data]\n",
//            argv[0]);
//     return 1;
// }
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#define SENSOR_POWER_PIN GPIO_PIN(PA, 28)
// #include "pm_layered.h"
#include "periph/pm.h"
#include "ds18.h"
#include "ds18_params.h"
#include "fmt.h"
#include "net/coap.h"
#include "net/nanocoap.h"
#include "net/nanocoap_sock.h"
#include "net/sock/udp.h"
#include "od.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/udp.h"
#include "net/gnrc/pktdump.h"
#include "timex.h"
#include "utlist.h"
#include "xtimer.h"


extern ds18_t dev;

// static gnrc_netreg_entry_t coapclient = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL,
//                                                                KERNEL_PID_UNDEF);

static ssize_t _send(coap_pkt_t *pkt, size_t len, char *addr_str, char *port_str)
{
    ipv6_addr_t addr;
    sock_udp_ep_t remote;

    remote.family = AF_INET6;
    
    /* parse for interface */
    char *iface = ipv6_addr_split_iface(addr_str);
    if (!iface) {
        if (gnrc_netif_numof() == 1) {
            /* assign the single interface found in gnrc_netif_numof() */
            remote.netif = (uint16_t)gnrc_netif_iter(NULL)->pid;
        }
        else {
            remote.netif = SOCK_ADDR_ANY_NETIF;
        }
    }
    else {
        int pid = atoi(iface);
        if (gnrc_netif_get_by_pid(pid) == NULL) {
            puts("nanocli: interface not valid");
            return 0;
        }
        remote.netif = pid;
    }

    /* parse destination address */
    if (ipv6_addr_from_str(&addr, addr_str) == NULL) {
        puts("nanocli: unable to parse destination address");
        return 0;
    }
    if ((remote.netif == SOCK_ADDR_ANY_NETIF) && ipv6_addr_is_link_local(&addr)) {
        puts("nanocli: must specify interface for link local target");
        return 0;
    }
    memcpy(&remote.addr.ipv6[0], &addr.u8[0], sizeof(addr.u8));

    /* parse port */
    remote.port = atoi(port_str);
    if (remote.port == 0) {
        puts("nanocli: unable to parse destination port");
        return 0;
    }

    return nanocoap_request(pkt, NULL, &remote, len);
}

// static void start_server(char *port_str)
// {
//      sock_udp_ep_t remote;

//     /* check if server is already running */
//     if (coapclient.target.pid != KERNEL_PID_UNDEF) {
//         printf("Error: server already running on port %" PRIu32 "\n",
//                coapclient.demux_ctx);
//         return;
//     }
//     /* parse port */
//     remote.port = atoi(port_str);
//     if (remote.port == 0) {
//         puts("Error: invalid port specified");
//         return;
//     }
//     /* start server (which means registering pktdump for the chosen port) */
//     coapclient.target.pid = gnrc_pktdump_pid;
//     coapclient.demux_ctx = (uint32_t)port;
//     gnrc_netreg_register(GNRC_NETTYPE_UDP, &coapclient);
//     printf("Success: started UDP server on port %" PRIu16 "\n", port);
// }

// static void stop_server(void)
// {
//     // /* check if server is running at all */
//     // if (coapclient.target.pid == KERNEL_PID_UNDEF) {
//     //     printf("Error: server was not running\n");
//     //     return;
//     // }
//     /* stop server */
//     gnrc_netreg_unregister(GNRC_NETTYPE_UDP, &coapclient);
//     coapclient.target.pid = KERNEL_PID_UNDEF;
//     puts("Success: stopped UDP server");
// }int deep = 1;int deep = 1;

int nanotest_client_cmd(int argc, char **argv)      /* agrc 是输入参数个数 argv是输入参数/字符串的集合，空格区分每个元素*/
{
    /* Ordered like the RFC method code numbers, but off by 1. GET is code 0. */
    // char *method_codes[] = {"get", "post", "put"};
    unsigned buflen = 128;
    uint8_t buf[buflen];
    coap_pkt_t pkt;
    size_t len;
    uint8_t token[2] = {0xDA, 0xEC};
    ssize_t p = 0;
     char rsp[16];
     int16_t temperature;
     ds18_t dev;
     if (strcmp(argv[4], "tem") == 0){
        /* get temp in celsius */
        ds18_init(&dev, &ds18_params[0]);
        gpio_set(SENSOR_POWER_PIN);
        if (ds18_get_temperature(&dev, &temperature) == DS18_OK) 
        {
            bool negative = (temperature < 0);
            if (negative) {
                temperature = -temperature;
            }
            float a = (float)temperature / 100;
             p += fmt_float(rsp,a,2);
        }
     }
    if (argc == 1) {
        /* show help for commands */
        goto end;
    }

    int code_pos = 2;
    // for (size_t i = 0; i < ARRAY_SIZE(method_codes); i++) {
    //     if (strcmp(argv[0], method_codes[i]) == 0) {           /*遍历寻找对应的操作*/
    //         code_pos = i;
    //     }
    // }
    // if (code_pos == -1) {
    //     goto end;
    // }

    pkt.hdr = (coap_hdr_t *)buf;

    /* parse options */
    if (argc == 4 || argc == 5) {
        ssize_t hdrlen = coap_build_hdr(pkt.hdr, COAP_TYPE_CON, &token[0], 2,
                                        code_pos+1, 1);
        coap_pkt_init(&pkt, &buf[0], buflen, hdrlen);
        coap_opt_add_string(&pkt, COAP_OPT_URI_PATH, argv[3], '/');
        if (argc == 5) {
            coap_opt_add_uint(&pkt, COAP_OPT_CONTENT_FORMAT, COAP_FORMAT_TEXT);
            len = coap_opt_finish(&pkt, COAP_OPT_FINISH_PAYLOAD);

            pkt.payload_len = strlen(rsp);
            memcpy(pkt.payload,(uint8_t*)rsp , p);
            len += pkt.payload_len;
        }
        else {
            len = coap_opt_finish(&pkt, COAP_OPT_FINISH_NONE);
        }

        printf("nanocli: sending msg ID %u, %u bytes\n", coap_get_id(&pkt),
               (unsigned) len);

        ssize_t res = _send(&pkt, buflen, argv[1], argv[2]);
        if (res < 0) {
            printf("nanocli: msg send failed: %d\n", (int)res);
        }
        else {
            char *class_str = (coap_get_code_class(&pkt) == COAP_CLASS_SUCCESS)
                                    ? "Success" : "Error";
            printf("nanocli: response %s, code %1u.%02u", class_str,
                   coap_get_code_class(&pkt), coap_get_code_detail(&pkt));
            if (pkt.payload_len) {
                unsigned format = coap_get_content_type(&pkt);
                if (format == COAP_FORMAT_TEXT
                        || format == COAP_FORMAT_LINK
                        || coap_get_code_class(&pkt) == COAP_CLASS_CLIENT_FAILURE
                        || coap_get_code_class(&pkt) == COAP_CLASS_SERVER_FAILURE) {
                    /* Expecting diagnostic payload in failure cases */
                    printf(", %u bytes\n%.*s\n", pkt.payload_len, pkt.payload_len,
                                                                  (char *)pkt.payload);
                }
                else {
                    printf(", %u bytes\n", pkt.payload_len);
                    od_hex_dump(pkt.payload, pkt.payload_len, OD_WIDTH_DEFAULT);
                }
            }
            else {
                printf(", empty payload\n");
            }
        }
        int deep =1 ;
        sam0_cortexm_sleep(deep);  
        if (cpu_woke_from_backup() != 0) {
            printf("true\n");}
        return 0;
    }



    end:
    printf("usage: CoAPclient <get|post|put> <addr>[%%iface] <port> <path> [data]\n");
    return 1;
}
