#ifndef __REG_LIB__h
#define __REG_LIB__h

int reg_set_value(char *subkey, char *key, char *val,  DWORD size);
int reg_get_value(char *subkey, char *key, char *buff, DWORD size);

#endif