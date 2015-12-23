#ifndef __CGI_COMMONT_H__
#define __CGI_COMMONT_H__

#define MIN(a,b) ((a) > (b) ? (b) : (a))
#define KEYBUFFER_SIZE 256

int get_inputs(char* inputstring, int buflen);
int get_value_from_query(const char* inputstring, const char* key, char* value, int buflen);

#endif  //__CGI_COMMONT_H__
