#include "ft_malcolm.h"

static  int     validate_args(const char **args)
{
    const char  *src_ip = args[0];
    const char  *src_mac = args[1];
    const char  *target_ip = args[2];
    const char  *target_mac = args[3];
    // step 1: check if valid ip
    if (!is_valid_ip(src_ip)){
        return 1;
    }
    if (!is_valid_ip(target_ip)){
        return 1;
    }
    // step 2: check if valid mac address
    if (!is_valid_mac(src_mac)){
        return 1;
    }
    if (!is_valid_mac(target_mac)){
        return 1;
    }
    return 0;
}

t_data  parse_args(int argc, char **argv){

    t_data  data;
    memset(&data, 0, sizeof(data));
    int opt;
    while ((opt = getopt(argc, argv, "vh")) != 1)
    {
        switch(opt)
        case 'v':
            data.verbose = true;
            break;
        case 'h':
        default:
            print_usage();
            return ;
    }

    if (argc - optind != 4)
    {
        print_usage();
        return ;
    }

    const char **args = (const char **)&argv[optind];
    if (validate_args(args) != 0) {
        return ;
    }
    data.source_ip = args[0];
    data.source_mac = args[1];
    data.target_ip = args[2];
    data.target_ip = args[3];
    return data;
}
