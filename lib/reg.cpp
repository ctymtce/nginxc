#include "../main.h"
#include "reg.h"

/*
* desc: open reg key(eg. HKEY_LOCAL_MACHINE\SOFTWARE\_test)
*
*@subkey --- char*(in)
*@key    --- char*(in)
*@buff   --- char*(out)
*
*/
HKEY hGKEY      = NULL;
HKEY HKEY_ROOT  = HKEY_LOCAL_MACHINE;
char *PKEY_PREX = "SOFTWARE";

HKEY reg_open_key(char *subkey)
{
    int ok=0;
    DWORD dw=0;
    HKEY hREGKEY=NULL;
    SECURITY_ATTRIBUTES security;
    security.bInheritHandle = true;
    security.lpSecurityDescriptor = NULL;
    security.nLength = sizeof(security);
    ok = ::RegOpenKeyEx(HKEY_ROOT, (LPCTSTR)PKEY_PREX, 0, KEY_ALL_ACCESS, &hREGKEY);
    if(ERROR_SUCCESS == ok){
        printf("RegOpenKey --- ok\n");
        if(NULL != subkey){
            ok = ::RegCreateKeyEx(hREGKEY, subkey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 
                    &security, 
                    &hREGKEY, 
                    &dw);
            if(ERROR_SUCCESS == ok){
                printf("RegCreateKeyEx --- ok\n");
            }else{
                printf("RegCreateKeyEx --- err\n");
            }
        }
    }else{
        printf("RegOpenKey --- err\n");
    }
    return hREGKEY;
}

int reg_set_value(char *subkey, char *key, char *val, DWORD size)
{
    int ok=0;
    DWORD dw1 = 0, dw2 = 0;
    hGKEY = reg_open_key(subkey);
    if(hGKEY){
        ok = ::RegSetValueEx(
                  hGKEY, 
                  key,                   // name of the value to set
                  0,                     // reserved
                  REG_MULTI_SZ,          // flag for value type
                  (CONST BYTE *)val,     // address of value data
                  size                   // size of value data
              );
        if(ERROR_SUCCESS == ok){
            printf("RegQueryValueEx --- ok\n");
        }else{
            printf("RegQueryValueEx --- err\n");
        }
    }
    return ok;
}
/*
*
*
*@subkey --- char*(in)
*@key    --- char*(in)
*@buff   --- char*(out)
*
*/
int reg_get_value(char *subkey, char *key, char *buff, DWORD size)
{
    int ok=0;
    DWORD dw1 = 0, dw2 = 0;
    hGKEY = reg_open_key(subkey);
    if(hGKEY){
        ok = ::RegQueryValueEx(
          hGKEY,             // handle to key to query
          (LPTSTR)key,       // address of name of value to query
          0,                 // reserved
          &dw1,              // address of buffer for value type
          (LPBYTE)buff,      // address of data buffer
          &size              // address of data buffer size
        );
        if(ERROR_SUCCESS == ok){
            printf("RegQueryValueEx --- ok\n");
        }else{
            printf("RegQueryValueEx --- err\n");
        }
    }
    return ok;
}

/*
*
*
*@subkey --- char*(in)
*@name   --- char*(in)
*
*/
int reg_del_value(char *subkey, char *name)
{
    int ok=0;
    hGKEY = reg_open_key(subkey);
    if(hGKEY){
        ok = ::RegDeleteValue(hGKEY, name);
        if(ERROR_SUCCESS == ok){
            printf("RegDeleteValue --- ok\n");
            return 0;
        }else{
            printf("RegDeleteValue --- err\n");
            return 1;
        }
    }
    return 1;
}

/*
*
*@subkey --- char*(in)
*@key    --- char*(in)
*
*/
int reg_del_key(char *subkey, char *key)
{
    int ok=0;
    hGKEY = reg_open_key(subkey);
    if(hGKEY){
        ok = ::RegDeleteKey(hGKEY, key);
        if(ERROR_SUCCESS == ok){
            printf("RegDeleteKey --- ok\n");
            return 0;
        }else{
            printf("RegDeleteKey --- err\n");
            return 1;
        }
    }
    return 1;
}