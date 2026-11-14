#include "ft_malcolm.h"

static  int     create_raw_socket(void)
{
    // AF_PACKET gives access to the Ethernet layer.
    int fd = socket(AF_PACKET, SOCK_RAW, htons(0x0806));
    if (fd < 0) 
    {
        perror("ft_malcolm: socket");
        return -1;
    }
    // NOTE: hard coded timeout for now.
    struct timeval tv;
    tv.tv_sec = 5000 / 1000;
    tv.tv_usec = (5000 % 1000) * 1000;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
        perror("ft_malcolm: setsocketopt(SO_RCVTIMEO)");
    return fd;
}

/*
 * Ethernet header occupies exactly 14 bytes
 * DESTINATION MAC (6 Bytes)
 * SOURCE MAC (6 Bytes)
 * EtherType (2 bytes)
 * Payload
 * */
static int create_arp_message(t_data *data, packet *pkt)
{
    memset(pkt, 0, sizeof(packet));

    /* Ethernet header */
    memcpy(pkt->eth.h_source, data->src_mac, ETH_ALEN);
    memcpy(pkt->eth.h_dest, data->target_mac, ETH_ALEN);
    pkt->eth.h_proto = htons(ETH_P_ARP);

    /* ARP header */
    pkt->arp.htype = htons(ARPHRD_ETHER);
    pkt->arp.ptype = htons(ETH_P_IP);
    pkt->arp.hlen = ETH_ALEN;
    pkt->arp.plen = 4;
    pkt->arp.oper = htons(ARPOP_REQUEST);

    /* ARP addresses */
    memcpy(pkt->arp.sha, data->src_mac, ETH_ALEN);
    memcpy(pkt->arp.spa, &data->src_ip, 4);
    memcpy(pkt->arp.tha, data->target_mac, ETH_ALEN);
    memcpy(pkt->arp.tpa, &data->target_ip, 4);

    return 0;
}

int     ft_malcolm(t_data *data)
{
    // Step 1: open correct socket
    int fd = create_raw_socket(void);
    if (fd < 0) return -1;
    // Step 2: Create the Ethernet Frame Header and arp message inside it.
    if (create_arp_message(data, &pkt) < 0)
    {
        close(fd);
        return (-1);
    }
    // Step 3: Send the broadcast arp message
    // Step 4: Print log / usage in case of error
    return (0);
}
