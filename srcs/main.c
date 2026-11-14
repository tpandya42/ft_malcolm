#include "ft_malcolm.h"

int main(int argc, char **argv)
{
    printf("Hi., Welcome to our ft_malcolm!!!!\n");
    t_data  data;
    if (parse_args(argc, argv, &data) != 0)
    {
        fprintf(stderr, "parsing failed\n");
        return 1;
    }
    int ret;
    ret = ft_malcolm(data);
    return (0);
}
