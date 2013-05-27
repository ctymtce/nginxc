/**
 * desc: nginx后台服务控制(windows)
 * call: 1,安装:
 *          nginxc --install D:\nginx-1.2.8\start.bat D:\nginx-1.2.8\stop.bat 
 *
 *       2,移除:
 *          nginxc --remove
 *
*/
#include "main.h"
#include "lib/reg.h"

SERVICE_STATUS        SSTATUS;
SERVICE_STATUS_HANDLE SHANDLE;

void WINAPI service_handler(DWORD);
void installService(char *path, char *display);
void removeService(char *display);
void start_process(char *pipath);
void pipeGetTest();

char FP_START[128] = {'\0'}; //目标可执行文件(CreateProcess)
char FP_STOP[128]  = {'\0'}; //停止服务时要启动的进程
char PIPNAME[22]   = "nginxc";

void start_process(char *pipath)
{
    STARTUPINFO sa;
    ZeroMemory(&sa, sizeof(sa));
    sa.cb = sizeof(sa);
    sa.dwFlags = STARTF_USESHOWWINDOW;
    sa.wShowWindow = SW_HIDE;
    
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));
    
    if(::CreateProcess((LPCTSTR)pipath, 
        NULL,
        NULL,
        NULL,
        false,
        CREATE_NO_WINDOW,
        NULL,
        NULL,
        &sa,
        &pi)) {
        ::CloseHandle(pi.hProcess);
        ::CloseHandle(pi.hThread);
        //MessageBox("启动进程成功!");
    }else {
        //::MessageBox(NULL, "启动进程失败!",NULL,MB_OK| MB_ICONERROR);
    }
}

void writeLog(char *logs, DWORD cnt)
{
    FILE *fp = fopen("c:/iservice__________.txt", "a");
    char buff[1024] = {'\0'};
    int i=1;
    sprintf(buff, "%s [%d]<<<\n\n", logs, cnt);
    fputs(buff, fp);
    fclose(fp);
}

void pipeGetTest()
{
    char buff[1024] = {'\0'};
    FILE *pfp;
    system("D:\\test.bat");
    pfp = _popen("D:/test.exe", "r");
    if(NULL == pfp) {
        //return "Open D:/test.exe failure!\n";
    }
    if(NULL != fgets(buff, sizeof(buff), pfp)) {
        _pclose(pfp);
    }else {
        //return "Read D:/test.exe failure!\n";
    }
}

// warning! static buffer, non-reentrable
const char *getWindowsError()
{
    static char sBuf[1024];
    DWORD uErr = ::GetLastError();
    sprintf(sBuf, "code=%d, error=", uErr );
    int iLen = strlen(sBuf);
    if(!FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, uErr, 0, sBuf+iLen, sizeof(sBuf)-iLen, NULL )) {
        // FIXME? force US-english langid?
        strcpy( sBuf+iLen, "(no message)" );
    }
    return sBuf;
}

//注册服务
void installService(char *path, char *display)
{
    SC_HANDLE hSCM = OpenSCManager (
        NULL,					// local computer
        NULL,					// ServicesActive database 
        SC_MANAGER_ALL_ACCESS );// full access rights
    
    SC_HANDLE hService = CreateService(
        hSCM,							// SCM database 
        display,					// name of service 
        display,					// service name to display 
        SERVICE_ALL_ACCESS,				// desired access 
        SERVICE_WIN32_OWN_PROCESS,		// service type 
        SERVICE_AUTO_START,				// start type 
        SERVICE_ERROR_NORMAL,			// error control type 
        path,							// path to service's binary 
        NULL,							// no load ordering group 
        NULL,							// no tag identifier 
        NULL,							// no dependencies 
        NULL,							// LocalSystem account 
        NULL );							// no password  
    
    if(!hService){
        CloseServiceHandle(hSCM);
        printf("CreateService() failed: %s", getWindowsError() );
    } else	{
        printf("Service 'nginxc' installed succesfully."); 
    }
}
//移除服务
void removeService(char *display)
{
    printf("removeing service...");
    // open manager
    SC_HANDLE hSCM = OpenSCManager (
        NULL,					// local computer
        NULL,					// ServicesActive database 
        SC_MANAGER_ALL_ACCESS );// full access rights
    
    // open service
    SC_HANDLE hService = OpenService(hSCM, (LPCTSTR)display, DELETE );
    if ( !hService )
    { 
        CloseServiceHandle ( hSCM );
        printf ( "OpenService() failed: %s\n", getWindowsError() );
    }
    
    // do delete
    bool bRes = !!DeleteService(hService);
    CloseServiceHandle ( hService );
    CloseServiceHandle ( hSCM );
    
    if ( !bRes ) 
        printf( "DeleteService() failed: %s", getWindowsError() );
    else
        printf( "Service '%s' was removed succesfully!\n", display); 
}

void WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
    SHANDLE = RegisterServiceCtrlHandler("nginxc", service_handler);
    SSTATUS.dwServiceType  = SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS;
    SSTATUS.dwCurrentState = SERVICE_START_PENDING;
    //file://如用户程序的代码比较多  （执行时间超过1秒），这儿要设成 SERVICE_START_PENDING ，待用户程序完成后再设为SERVICE_RUNNING。
    SSTATUS.dwControlsAccepted = SERVICE_ACCEPT_STOP;//表明Service目前能接受的命令是停止命令。
    SSTATUS.dwWin32ExitCode    = NO_ERROR;
    SSTATUS.dwCheckPoint       = 0;
    SSTATUS.dwWaitHint         = 0;
    SetServiceStatus(SHANDLE, &SSTATUS);
    //file://必须随时更新数据库中Service的状态。
    //Mycode();           //这儿可放入用户自己的代码
    SSTATUS.dwServiceType      = SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS;
    SSTATUS.dwCurrentState     = SERVICE_RUNNING;
    SSTATUS.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    SSTATUS.dwWin32ExitCode    = NO_ERROR;
    SSTATUS.dwCheckPoint       = 0;
    SSTATUS.dwWaitHint         = 0;
    SetServiceStatus(SHANDLE, &SSTATUS);
    // Mycode();//   这儿也可放入用户自己的代码
    // writeLog();
    //start_process("D:\\elastic\\bin\\elasticsearch.bat");
    start_process(FP_START);
    //start_process("D:\\bat.bat");
}

void WINAPI service_handler(DWORD Opcode)
{
    switch(Opcode)
    {
        case SERVICE_CONTROL_STOP:     // file://停止Service   Mycode（）；//这儿可放入用户自己的相关代码
            SSTATUS.dwWin32ExitCode   = 0;
            SSTATUS.dwCurrentState    = SERVICE_STOPPED;  //file://把Service的当前状态置为STOP
            SSTATUS.dwCheckPoint      = 0;
            SSTATUS.dwWaitHint        = 0;
            SetServiceStatus(SHANDLE, &SSTATUS);  //必须随时更新数据库中Service的状态
            //writeLog("service was stoped!", 19);
            reg_get_value("nginxc", "stop", FP_STOP, sizeof(FP_STOP));
            start_process(FP_STOP);
            break;
        case SERVICE_CONTROL_INTERROGATE:
            SetServiceStatus(SHANDLE, &SSTATUS);  //必须随时更新数据库中Service的状态
        break;
    }
}

int main(int argc, char* argv[])
{
    char buff[256] = {'\0'}, buff_temp[256] = {'\0'};
    DWORD ok = 0;
    if(argc < 2) { //分配服务程序
        printf("invaild paraters!");
        //reg_set_value(PIPNAME, "runs2", buff, 2*wcslen(buff)+1);
        reg_set_value(PIPNAME, "start", "start启动", strlen("start启动")+1);
        reg_set_value(PIPNAME, "stop",  "stop停止",  strlen("stop停止")+1);
        //Sleep(3000);
    }else {
        printf("start install service ... %s", argv[1]);
        char path_exe[128]   = {'\0'};
        char path_start[128] = {'\0'}; //服务启动时要随之启动的进程(命令)
        char path_stop[128]  = {'\0'}; //服务停止时要随之停止的进程(命令)
        ok = ::GetFullPathName("nginxc.exe", sizeof(path_exe), (LPTSTR)path_exe, (LPTSTR *)buff_temp);
        printf("--------------------------------\n");
        if(argc >= 3){
            //strcat(path, " ");
            //strcat(path, argv[2]);
            strcpy(path_start, argv[2]);
        }
        if(argc >= 4){
            //strcat(path, " ");
            //strcat(path, argv[2]);
            strcpy(path_stop, argv[3]);
        }
        printf("=============================%s\n", path_exe);
        if(0 == strcmp(argv[1], "--install")) {       //安装
            printf("安装服务\n");
            strcat(path_exe, " --start");
            installService(path_exe, PIPNAME);
            reg_set_value(PIPNAME, "start", path_start, strlen(path_start)+1);
            reg_set_value(PIPNAME, "stop",  path_stop,  strlen(path_stop)+1);
        }else if(0 == strcmp(argv[1], "--remove")) {  //移除
            printf("移除服务\n");
            removeService(PIPNAME);
        }else if(0 == strcmp(argv[1], "--start")){
            printf("启动服务\n");
            reg_get_value("nginxc", "start", FP_START, sizeof(FP_START));
            // _stprintf(FP_START, "%s", argv[1]);          //目标文件
            SERVICE_TABLE_ENTRY STABLE[2];              //一个Service进程可以有多个线程 //线程的入口表
            STABLE[0].lpServiceName= "nginxc";         //线程名字
            STABLE[0].lpServiceProc=ServiceMain;        //线程入口地址
            STABLE[1].lpServiceName=NULL;               //最后一个必须为NULL
            STABLE[1].lpServiceProc=NULL;
            StartServiceCtrlDispatcher(STABLE);
        }
    }
    return 0;
}