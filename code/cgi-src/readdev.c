#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ehClient.h"
#include "cgi_common.h"

#define DEFAULT_IP  "192.168.1.130"
#define DEFAULT_PORT  2222

#define ADDR "addr"

/*
  Function: Get address from environment
  return: -1 -- not found.
           0 -- find and save the result in addr.
*/
int get_addr(char* addr, int buflen)
{
    char buf[BUFFER_SIZE] = {0};

    if (get_inputs(buf, BUFFER_SIZE) == 0)
        return -1;

    return get_value_from_query(buf, ADDR, addr, buflen);
}

/*
  Function: read device value
  return: -1 -- failed.
           0 -- success.
*/
int cgi_readDev(int* p_data)
{
    char addr[KEYBUFFER_SIZE] = {0};
    int devno = 0;
    int value = 0;

    //get addr
    if (get_addr(addr, KEYBUFFER_SIZE))
        return -1;

    devno = atoi(addr);

    //read device value
    if (readDev(DEFAULT_IP, DEFAULT_PORT, devno, p_data))
        return -1;

    return 0;
}

int main(int argc, char** argv)
{    
    int value = -1;

    cgi_readDev(&value);

    printf("content-type:text/html\n\n");
    printf("%d\n", value);

    return 0;
}
