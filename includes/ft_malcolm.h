#ifndef FT_MALCOLM_H
# define FT_MALCOLM_H

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdint.h>
#include <stdbool.h>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <net/if_arp.h>
#include <linux/if_packet.h>

#define INET4_LEN 4
#define BUFFER_SIZE 42

typedef struct s_data {
    uint8_t source_ip[INET4_LEN];
    uint8_t source_mac[ETH_ALEN];
    uint8_t target_ip[INET4_LEN];
    uint8_t target_mac[ETH_ALEN];
    // Might need variables to store ip and mac address in char * form.
    int sockfd;
    char iface[IF_NAMESIZE];
    bool    verbose;
} t_data;

typedef struct  arp_packet
{
    struct  ethhdr eth;
    struct  ether_arp arp;
}   packet;

// utils.c
int     ft_strlen(const char *str);

// parse.c
int  parse_args(int argc, char **argv, t_data *data);

// ft_malcolm.h
void    handle_sigint(int sig);
int ft_malcolm(t_data *data);

#endif
