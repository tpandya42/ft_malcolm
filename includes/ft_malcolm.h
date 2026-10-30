#ifndef FT_MALCOLM_H
# define FT_MALCOLM_H

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <net/ethernet.h>

#define AFINET_LEN 4
#define BUFFER_SIZE 42

typedef struct ethCfg {
    uint8_t dest_mac[ETH_ALEN];
    uint8_t src_mac[ETH_ALEN];
    uint16_t ethertype; // Protocol type (e.g. 0x0806 for ARP)
} t_ethCfg;

typedef struct s_conf {
    uint16_t htype;
    uint16_t ptype;
    uint8_t hlen;
    uint8_t plen;
    uint16_t operation;
    uint8_t sender_mac[ETH_ALEN];
    uint8_t sender_ip[AFINET_LEN];
    uint8_t target_mac[ETH_ALEN];
    uint8_t target_ip[AFINET_LEN];
} t_arp_conf;

typedef struct s_data {
    uint8_t source_ip[AFINET_LEN];
    uint8_t source_mac[ETH_ALEN];
    uint8_t target_ip[AFINET_ALEN];
    uint8_t target_mac[ETH_ALEN];
    int sockfd;
    char iface[IF_NAMESIZE];
    bool    verbose;
} t_data;


// parse.c
t_data  parse_args(int argc, char **argv);

#endif
