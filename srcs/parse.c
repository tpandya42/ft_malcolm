#include "ft_malcolm.h"

static int      is_valid_ip(const char *ip, uint8_t *ip_bytes)
{
    if (ip == NULL || *ip == '\0')
        return 1;
    if (inet_pton(AF_INET, ip, ip_bytes) != 1)
    {
        fprintf(stderr, "ip: invalid address format\n");
        return 1;
    }
    return 0;
}

static int      is_valid_mac(const char *mac, uint8_t mac_bytes[6])
{
    if (mac == NULL || *mac == '\0')
        return 1;
    if (strlen(mac) != 17)
        return 1;

    int     i = 0;
    while (i < 6)
    {
        int value = 0;
        int j = 0;
        while (j < 2)
        {
            char c = mac[i * 3 + j];
            int digit;
            if (c >= '0' && c <= '9')
                digit = c - '0';
            else if (c >= 'a' && c <= 'f')
                digit = c - 'a' + 10;
            else if (c >= 'A' && c <= 'F')
                digit = c - 'A' + 10;
            else
                return (1);
            value = value * 16 + digit;
            j++;
        }
        mac_bytes[i] = (uint8_t)value;
        if ( i < 5 && mac[i *3 +2] != ':')
            return (1);
        i++;
    }

    return (0);  
}

static  int     validate_args(const char **args, t_data *data)
{
    const char  *src_ip = args[0];
    const char  *src_mac = args[1];
    const char  *target_ip = args[2];
    const char  *target_mac = args[3];
    // step 1: check if valid ip
    if (!js_valid_ip(src_ip, data->source_ip)){
        return 1;
    }
    if (!is_valid_ip(target_ip, data->target_ip)){
        return 1;
    }
    // step 2: check if valid mac address
    if (!is_valid_mac(src_mac, data->source_mac)){
        return 1;
    }
    if (!is_valid_mac(target_mac, data->target_mac)){
        return 1;
    }
    return 0;
}

int  parse_args(int argc, char **argv, t_data *data){

    memset(data, 0, sizeof(*data));
    int opt;
    while ((opt = getopt(argc, argv, "vh")) != -1)
    {
        switch(opt)
        {
        case 'v':
            data->verbose = true;
            break;
        case 'h':
        default:
//            print_usage();
            return 1;
        }
    }

    if (argc - optind != 4)
    {
//        print_usage();
        return 1;
    }

    const char **args = (const char **)&argv[optind];
    if (validate_args(args, data) != 0) {
        return 1;
    }
/*    data.source_ip = args[0];
    data.source_mac = args[1];
    data.target_ip = args[2];
    data.target_mac= args[3]; */
    return 0;
}
