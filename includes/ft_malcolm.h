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
#include <netinet/ether.h>
#include <net/if.h>
#include <net/if_arp.h>


#define AFINET_LEN 4
#define BUFFER_SIZE 42

typedef struct s_data {
    uint8_t source_ip[AFINET_LEN];
    uint8_t source_mac[ETH_ALEN];
    uint8_t target_ip[AFINET_LEN];
    uint8_t target_mac[ETH_ALEN];
    // Might need variables to store ip and mac address in char * form.
    int sockfd;
    char iface[IF_NAMESIZE];
    bool    verbose;
} t_data;

typedef struct  arp_packet
{
    struct  ethhdr eth;
    struct  arp_hdr arp;
}   packet;

// utils.c
int     ft_strlen(const char *str);

// parse.c
int  parse_args(int argc, char **argv, t_data *data);

// ft_malcolm.h
int ft_malcolm(t_data *data);

#endif
