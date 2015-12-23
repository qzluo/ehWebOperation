#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ehClient.h"
#include "cgi_common.h"

#define DEFAULT_IP  "192.168.1.118"
#define DEFAULT_PORT  2222

#define ADDR "addr"
#define VALUE "value"

/*
  Function: Get address from environment
  return: -1 -- not found.
           0 -- find and save the result in addr.
*/
int get_addr_and_value(char* addr, char* value, int buflen)
{
    char buf[BUFFER_SIZE] = {0};

    if (get_inputs(buf, BUFFER_SIZE) == 0)
        return -1;

    if (get_value_from_query(buf, ADDR, addr, buflen))
        return -1;

    return get_value_from_query(buf, VALUE, value, buflen);
}

/*
  Function: read device value
  return: -1 -- failed.
           0 -- success.
*/
int cgi_writeDev(void)
{
    char addr[KEYBUFFER_SIZE] = {0};
    char strValue[KEYBUFFER_SIZE] = {0};
    int devno = 0;
    int value = 0;

    //get addr
    if (get_addr_and_value(addr, strValue, KEYBUFFER_SIZE))
        return -1;

    devno = atoi(addr);
    value = atoi(strValue);

    //read device value
    if (writeDev(NULL, DEFAULT_PORT, devno, value))
        return -1;

    return 0;
}

int main(int argc, char** argv)
{    
    int ret = 0;

    ret = cgi_writeDev();

    printf("content-type:text/html\n\n");
    printf("%s\n", ret ? "failed" : "success");

    return 0;
}
