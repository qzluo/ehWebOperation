#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgi_common.h"

/*
  Get input string from environment.
  return: string length
*/
int get_inputs(char* inputstring, int buflen)
{
    int length = 0;
    char* method = NULL;

    method = getenv("REQUEST_METHOD");
    if(method == NULL)
        return 0;    

    if (!strcmp(method, "POST")) {
        length = MIN(atoi(getenv("CONTENT_LENGTH")), buflen); 
        if(length != 0) {
            fread(inputstring, sizeof(char), 
                length, stdin);
            inputstring[MIN(length, buflen)] = 0;
        }
    }
    else if(!strcmp(method, "GET")) {
        length = MIN(strlen(getenv("QUERY_STRING")), buflen);
        strncpy(inputstring, getenv("QUERY_STRING"), length);
    }

    return length;
}

/*
  Function: Get value from inputstring used in html.
            the inputstring is in the format£º
            key1=value1&key2=value2&...
  return: -1 -- value with the key is not found.
           0 -- find and save the result in value.
*/
int get_value_from_query(const char* inputstring, const char* key, char* value, int buflen)
{
    char keybuf_temp[KEYBUFFER_SIZE] = {0};
    char valuebuf_temp[KEYBUFFER_SIZE] = {0};
    char* keyStart = NULL;
    char* valueStart = NULL;

    keyStart = (char*)inputstring;
    while (keyStart) {
        valueStart = strstr(keyStart, "=");
        if (!valueStart)    //not find '='
            return -1;

        if (keyStart == valueStart)
            return -1;

        //copy key
        strncpy(keybuf_temp, keyStart, (valueStart - keyStart));
        keybuf_temp[valueStart - keyStart] = 0;
        
        //find next key
        valueStart++;
        keyStart = strstr(valueStart, "&");

        //copy value
        if (keyStart) {
            strncpy(valuebuf_temp, valueStart, (keyStart - valueStart));
            valuebuf_temp[keyStart - valueStart] = 0;
            keyStart++;
        }
        else
            strcpy(valuebuf_temp, valueStart);

        //compare key
        if (strcmp(key, keybuf_temp) == 0) {
            //copy value
            strncpy(value, valuebuf_temp, buflen);
            return 0;
        }
    }

    return -1;
}