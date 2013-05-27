/**
 * desc: nginx��̨�������(windows)
 * call: 1,��װ:
 *          nginxc --install D:\nginx-1.2.8\start.bat D:\nginx-1.2.8\stop.bat 
 *
 *       2,�Ƴ�:
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

char FP_START[128] = {'\0'}; //Ŀ���ִ���ļ�(CreateProcess)
char FP_STOP[128]  = {'\0'}; //ֹͣ����ʱҪ�����Ľ���
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
        //MessageBox("�������̳ɹ�!");
    }else {
        //::MessageBox(NULL, "��������ʧ��!",NULL,MB_OK| MB_ICONERROR);
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

//ע�����
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
//�Ƴ�����
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
    //file://���û�����Ĵ���Ƚ϶�  ��ִ��ʱ�䳬��1�룩�����Ҫ��� SERVICE_START_PENDING �����û�������ɺ�����ΪSERVICE_RUNNING��
    SSTATUS.dwControlsAccepted = SERVICE_ACCEPT_STOP;//����ServiceĿǰ�ܽ��ܵ�������ֹͣ���
    SSTATUS.dwWin32ExitCode    = NO_ERROR;
    SSTATUS.dwCheckPoint       = 0;
    SSTATUS.dwWaitHint         = 0;
    SetServiceStatus(SHANDLE, &SSTATUS);
    //file://������ʱ�������ݿ���Service��״̬��
    //Mycode();           //����ɷ����û��Լ��Ĵ���
    SSTATUS.dwServiceType      = SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS;
    SSTATUS.dwCurrentState     = SERVICE_RUNNING;
    SSTATUS.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    SSTATUS.dwWin32ExitCode    = NO_ERROR;
    SSTATUS.dwCheckPoint       = 0;
    SSTATUS.dwWaitHint         = 0;
    SetServiceStatus(SHANDLE, &SSTATUS);
    // Mycode();//   ���Ҳ�ɷ����û��Լ��Ĵ���
    // writeLog();
    //start_process("D:\\elastic\\bin\\elasticsearch.bat");
    start_process(FP_START);
    //start_process("D:\\bat.bat");
}

void WINAPI service_handler(DWORD Opcode)
{
    switch(Opcode)
    {
        case SERVICE_CONTROL_STOP:     // file://ֹͣService   Mycode������//����ɷ����û��Լ�����ش���
            SSTATUS.dwWin32ExitCode   = 0;
            SSTATUS.dwCurrentState    = SERVICE_STOPPED;  //file://��Service�ĵ�ǰ״̬��ΪSTOP
            SSTATUS.dwCheckPoint      = 0;
            SSTATUS.dwWaitHint        = 0;
            SetServiceStatus(SHANDLE, &SSTATUS);  //������ʱ�������ݿ���Service��״̬
            //writeLog("service was stoped!", 19);
            reg_get_value("nginxc", "stop", FP_STOP, sizeof(FP_STOP));
            start_process(FP_STOP);
            break;
        case SERVICE_CONTROL_INTERROGATE:
            SetServiceStatus(SHANDLE, &SSTATUS);  //������ʱ�������ݿ���Service��״̬
        break;
    }
}

int main(int argc, char* argv[])
{
    char buff[256] = {'\0'}, buff_temp[256] = {'\0'};
    DWORD ok = 0;
    if(argc < 2) { //����������
        printf("invaild paraters!");
        //reg_set_value(PIPNAME, "runs2", buff, 2*wcslen(buff)+1);
        reg_set_value(PIPNAME, "start", "start����", strlen("start����")+1);
        reg_set_value(PIPNAME, "stop",  "stopֹͣ",  strlen("stopֹͣ")+1);
        //Sleep(3000);
    }else {
        printf("start install service ... %s", argv[1]);
        char path_exe[128]   = {'\0'};
        char path_start[128] = {'\0'}; //��������ʱҪ��֮�����Ľ���(����)
        char path_stop[128]  = {'\0'}; //����ֹͣʱҪ��ֹ֮ͣ�Ľ���(����)
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
        if(0 == strcmp(argv[1], "--install")) {       //��װ
            printf("��װ����\n");
            strcat(path_exe, " --start");
            installService(path_exe, PIPNAME);
            reg_set_value(PIPNAME, "start", path_start, strlen(path_start)+1);
            reg_set_value(PIPNAME, "stop",  path_stop,  strlen(path_stop)+1);
        }else if(0 == strcmp(argv[1], "--remove")) {  //�Ƴ�
            printf("�Ƴ�����\n");
            removeService(PIPNAME);
        }else if(0 == strcmp(argv[1], "--start")){
            printf("��������\n");
            reg_get_value("nginxc", "start", FP_START, sizeof(FP_START));
            // _stprintf(FP_START, "%s", argv[1]);          //Ŀ���ļ�
            SERVICE_TABLE_ENTRY STABLE[2];              //һ��Service���̿����ж���߳� //�̵߳���ڱ�
            STABLE[0].lpServiceName= "nginxc";         //�߳�����
            STABLE[0].lpServiceProc=ServiceMain;        //�߳���ڵ�ַ
            STABLE[1].lpServiceName=NULL;               //���һ������ΪNULL
            STABLE[1].lpServiceProc=NULL;
            StartServiceCtrlDispatcher(STABLE);
        }
    }
    return 0;
}