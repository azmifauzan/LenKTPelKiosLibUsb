#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <direct.h>

int portnumber = 999;
char isiBuffer[102400];
char isiInfo[100];
char isiClose[25];
char isiPoll[100];
char isiPut[100];
char isiReset[25];
char isiPut[100];
char isiFinger[50];
char isiLog[102400];
char isiDemog[18096];
char isiSwitch[50];
char isiDisplay[50];
char isiSetpow[50];
char isiRespow[50];
char isiUpdateInit[50];
char isiUpdateFile[50];
char isiUpdateFinish[50];
int tid = 1;
int nunggurespon = 0, resput = 0, resinfo = 0, resclose = 0, respoll = 0, resreset = 0, resfinger = 0;
int reslog = 0, resdemog = 0, resswitch = 0, resdisplay = 0, ressetpow = 0, resrespow = 0;
int resupdateinit = 0, resupdatefile = 0, resupdatefinish = 0;
int openstatus = 0;
HANDLE hComm;
BOOL br = FALSE;
char hasil2[1024];
int kirimcmd = 0;
int lastReaderStatus = 0, lastPutKtpStatus = 0, lastVerifyFingerStatus = 0, lastDeviceStatus = 0, portInUse = 0;
char commandGlobal[100];
int resultKirim = 0;
pthread_t tid1,tid2;


char* sendCommandOP(char* port, char* command)
{
    char   ComPortName[] = "\\\\.\\";
    strcat(ComPortName,port);
    BOOL   Status;
    DWORD dwEventMask;
    char TempChar;
    DWORD NoBytesRead;
    char SerialBuffer[1024];
    int i = 0;
    char hasil3[100];
    char mylog[102400];

    sprintf(mylog,"===SendCommand===Begin process");
    tulisLog(mylog);

    sprintf(mylog,"Trying to established connection in %s",port);
    tulisLog(mylog);

    hComm = CreateFile( ComPortName,                       // Name of the Port to be Opened
                        GENERIC_READ | GENERIC_WRITE,      // Read/Write Access
                        0,                                 // No Sharing, ports cant be shared
                        NULL,                              // No Security
                        OPEN_EXISTING,                     // Open existing port only
                        0,                                 // Non Overlapped I/O
                        NULL);                             // Null for Comm Devices

    if (hComm == INVALID_HANDLE_VALUE){
        sprintf(hasil3,"opendevicefailed#");
        sprintf(mylog,"Failed to established connection in COM%d, error number:%d",portnumber,GetLastError());
        tulisLog(mylog);
    }

    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    sprintf(mylog,"Trying to GetCommState");
    tulisLog(mylog);

    Status = GetCommState(hComm, &dcbSerialParams);

    if (Status == FALSE){
        sprintf(hasil3,"opendevicefailed#");
        sprintf(mylog,"Failed to GetCommState, error number:%d",GetLastError());
        tulisLog(mylog);
    }

    dcbSerialParams.BaudRate = CBR_115200;      // Setting BaudRate = 9600
    dcbSerialParams.ByteSize = 8;             // Setting ByteSize = 8
    dcbSerialParams.StopBits = ONESTOPBIT;    // Setting StopBits = 1
    dcbSerialParams.Parity   = NOPARITY;      // Setting Parity = None

    sprintf(mylog,"Trying to SetCommState");
    tulisLog(mylog);

    Status = SetCommState(hComm, &dcbSerialParams);

    if (Status == FALSE)
    {
        sprintf(hasil3,"opendevicefailed#");
        sprintf(mylog,"Failed to SetCommState, error number:%d",GetLastError());
        tulisLog(mylog);
    }

    COMMTIMEOUTS timeouts = { 0 };

    timeouts.ReadIntervalTimeout         = 50;
    timeouts.ReadTotalTimeoutConstant    = 50;
    timeouts.ReadTotalTimeoutMultiplier  = 10;
    timeouts.WriteTotalTimeoutConstant   = 0;
    timeouts.WriteTotalTimeoutMultiplier = 0;

    sprintf(mylog,"Trying to SetCommTimeouts");
    tulisLog(mylog);

    if (SetCommTimeouts(hComm, &timeouts) == FALSE){
        sprintf(hasil3,"opendevicefailed#");
        sprintf(mylog,"Failed to SetCommTimeouts, error number:%d",GetLastError());
        tulisLog(mylog);
    }

    int pjg = strlen(command);
    char lpBuffer[pjg+1];
    strcpy(lpBuffer,command);
    DWORD  dNoOFBytestoWrite;
    DWORD  dNoOfBytesWritten = 0;

    dNoOFBytestoWrite = sizeof(lpBuffer);

    sprintf(mylog,"Trying to send command:%s",command);
    tulisLog(mylog);

    Status = WriteFile(hComm,               // Handle to the Serialport
                       lpBuffer,            // Data to be written to the port
                       dNoOFBytestoWrite,   // No of bytes to write into the port
                       &dNoOfBytesWritten,  // No of bytes written to the port
                       NULL);

    free(lpBuffer);

    if (Status == FALSE){
        sprintf(hasil3,"opendevicefailed#");
        sprintf(mylog,"Failed to send command, error number:%d",GetLastError());
        tulisLog(mylog);
    }

    sprintf(mylog,"Trying to SetCommMask");
    tulisLog(mylog);

    Status = SetCommMask(hComm, EV_RXCHAR);
    if (Status == FALSE){
        sprintf(hasil3,"opendevicefailed#");
        sprintf(mylog,"Failed to SetCommMask, error number:%d",GetLastError());
        tulisLog(mylog);
    }

    sprintf(mylog,"Trying to Set WaitCommEvent");
    tulisLog(mylog);

    Status = WaitCommEvent(hComm, &dwEventMask, NULL);
    if (Status == FALSE)
    {
        sprintf(hasil3,"opendevicefailed#");
        sprintf(mylog,"Failed to Set WaitCommEvent, error number:%d",GetLastError());
        tulisLog(mylog);
    }
    else
    {
        int err;
        do
        {
            Status = ReadFile(hComm, &TempChar, sizeof(TempChar), &NoBytesRead, NULL);
            err = GetLastError();
            SerialBuffer[i] = TempChar;
            i++;
        }
        while (NoBytesRead > 0 && err != ERROR_IO_PENDING);

        sprintf(mylog,"Receive buffer:%s",SerialBuffer);
        tulisLog(mylog);

        int j =0;
        for (j = 0; j < i-1; j++)
        {
            hasil3[j] = SerialBuffer[j];
        }
    }
    //free(SerialBuffer);
    memset(SerialBuffer,0,sizeof(SerialBuffer));

    sprintf(mylog,"===SendCommand===End process");
    tulisLog(mylog);

    return hasil3;
}

char* OpenOnly(char* port)
{
//    char mylog[1024];
//    sprintf(mylog,"send command:%s",command);
//    tulisLog(mylog);

    char mylog[1024];

    sprintf(mylog,"===Open COM===Begin process");
    tulisLog(mylog);

    char   ComPortName[] = "\\\\.\\";
    strcat(ComPortName,port);
    BOOL   Status;
    char hasil3[100];
    sprintf(hasil3,"success");

    sprintf(mylog,"Trying to established connection in COM%d",port);
    tulisLog(mylog);

    hComm = CreateFile( ComPortName,                       // Name of the Port to be Opened
                        GENERIC_READ | GENERIC_WRITE,      // Read/Write Access
                        0,                                 // No Sharing, ports cant be shared
                        NULL,                              // No Security
                        OPEN_EXISTING,                     // Open existing port only
                        0,                                 // Non Overlapped I/O
                        NULL);                             // Null for Comm Devices

    if (hComm == INVALID_HANDLE_VALUE){
        sprintf(hasil3,"opendevicefailed#");
        sprintf(mylog,"Failed to established connection in COM%d, error number:%d",portnumber,GetLastError());
        tulisLog(mylog);
    }

    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    sprintf(mylog,"Trying to GetCommState");
    tulisLog(mylog);

    Status = GetCommState(hComm, &dcbSerialParams);

    if (Status == FALSE){
        sprintf(hasil3,"opendevicefailed#");
        sprintf(mylog,"Failed to GetCommState, error number:%d",GetLastError());
        tulisLog(mylog);
    }

    dcbSerialParams.BaudRate = CBR_115200;      // Setting BaudRate = 9600
    dcbSerialParams.ByteSize = 8;             // Setting ByteSize = 8
    dcbSerialParams.StopBits = ONESTOPBIT;    // Setting StopBits = 1
    dcbSerialParams.Parity   = NOPARITY;      // Setting Parity = None
    dcbSerialParams.fAbortOnError = TRUE;

    sprintf(mylog,"Trying to SetCommState");
    tulisLog(mylog);

    Status = SetCommState(hComm, &dcbSerialParams);

    if (Status == FALSE)
    {
        sprintf(hasil3,"opendevicefailed#");
        sprintf(mylog,"Failed to SetCommState, error number:%d",GetLastError());
        tulisLog(mylog);
    }

    COMMTIMEOUTS timeouts = { 0 };

    timeouts.ReadIntervalTimeout         = 50;
    timeouts.ReadTotalTimeoutConstant    = 50;
    timeouts.ReadTotalTimeoutMultiplier  = 10;
    timeouts.WriteTotalTimeoutConstant   = 0;
    timeouts.WriteTotalTimeoutMultiplier = 0;

    sprintf(mylog,"Trying to SetCommTimeouts");
    tulisLog(mylog);

    if (SetCommTimeouts(hComm, &timeouts) == FALSE){
        sprintf(hasil3,"opendevicefailed#");
        sprintf(mylog,"Failed to SetCommTimeouts, error number:%d",GetLastError());
        tulisLog(mylog);
    }

    sprintf(mylog,"===Open COM===End process");
    tulisLog(mylog);

    return hasil3;
}

char* sendCommandSR(char* port, char* command, int timeout)
{
    char mylog[1024];
    sprintf(mylog,"send command:%s",command);
    tulisLog(mylog);

    HANDLE hComm2;
    char   ComPortName[] = "\\\\.\\";
    strcat(ComPortName,port);
    BOOL   Status;
    char TempChar;
    DWORD NoBytesRead;
    int i = 0;

    hComm2 = CreateFile( ComPortName,                       // Name of the Port to be Opened
                        GENERIC_READ | GENERIC_WRITE,      // Read/Write Access
                        0,                                 // No Sharing, ports cant be shared
                        NULL,                              // No Security
                        OPEN_EXISTING,                     // Open existing port only
                        FILE_FLAG_OVERLAPPED,              // Overlapped I/O
                        NULL);                                // Null for Comm Devices

    if (hComm2 == INVALID_HANDLE_VALUE){
        sprintf(hasil2,"opendevicefailed#");
        CloseHandle(hComm2);
        return hasil2;
    }

    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    Status = GetCommState(hComm2, &dcbSerialParams);

    if (Status == FALSE){
        sprintf(hasil2,"opendevicefailed#");
        CloseHandle(hComm2);
        return hasil2;
    }

    dcbSerialParams.BaudRate = CBR_115200;      // Setting BaudRate = 9600
    dcbSerialParams.ByteSize = 8;             // Setting ByteSize = 8
    dcbSerialParams.StopBits = ONESTOPBIT;    // Setting StopBits = 1
    dcbSerialParams.Parity   = NOPARITY;      // Setting Parity = None
    //dcbSerialParams.fAbortOnError = TRUE;

    Status = SetCommState(hComm2, &dcbSerialParams);

    if (Status == FALSE)
    {
        sprintf(hasil2,"opendevicefailed#");
        CloseHandle(hComm2);
        return hasil2;
    }

    COMMTIMEOUTS timeouts = { 0 };

    timeouts.ReadIntervalTimeout         = 50;
    timeouts.ReadTotalTimeoutConstant    = 50;
    timeouts.ReadTotalTimeoutMultiplier  = 10;
    timeouts.WriteTotalTimeoutConstant   = 0;
    timeouts.WriteTotalTimeoutMultiplier = 0;

    if (SetCommTimeouts(hComm2, &timeouts) == FALSE){
        sprintf(hasil2,"opendevicefailed#");
        CloseHandle(hComm2);
        return hasil2;
    }

    int pjg = strlen(command);
    char lpBuffer[pjg+1];
    strcpy(lpBuffer,command);
    DWORD  dNoOFBytestoWrite,dwOvRes;
    DWORD  dNoOfBytesWritten = 0;

    OVERLAPPED osWrite = { 0 };
    osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osWrite.hEvent == NULL){
		sprintf(hasil2,"opendevicefailed#");
		CloseHandle(hComm2);
		return hasil2;
	}

    dNoOFBytestoWrite = sizeof(lpBuffer);

    BOOL okw = WriteFile(hComm2, lpBuffer, dNoOFBytestoWrite, &dNoOfBytesWritten, &osWrite);
    if(!okw){
        DWORD err = GetLastError();
        if(err != ERROR_IO_PENDING)
        {
            sprintf(hasil2,"timeoutwrite#");
            CloseHandle(hComm2);
            return hasil2;
        }

        DWORD reason = WaitForSingleObject(osWrite.hEvent,INFINITE);
        switch(reason){
            case WAIT_OBJECT_0:
                break;
            case WAIT_OBJECT_0+1:
                break;
        }
    }

    CloseHandle(osWrite.hEvent);

    Status = SetCommMask(hComm2, EV_RXCHAR);
    if (Status == FALSE){
        sprintf(hasil2,"opendevicefailed#");
        CloseHandle(hComm2);
        return hasil2;
    }

    OVERLAPPED osRead = { 0 };
    osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osRead.hEvent == NULL){
		sprintf(hasil2,"error#open device failed");
		CloseHandle(hComm2);
		return hasil2;
	}

    BOOL readstat = TRUE;
    BOOL adaisi = FALSE;

    int counter = 0;
    while(readstat){
        BOOL okr = ReadFile(hComm2,&TempChar, sizeof(TempChar), &NoBytesRead, &osRead);
        if(!okr)
        {
            DWORD err = GetLastError();
            if(err != ERROR_IO_PENDING)
            {
                readstat = FALSE;
                continue;
            }

            DWORD result = WaitForSingleObject(osRead.hEvent,100);
            switch(result){
            case WAIT_OBJECT_0:
                if(!GetOverlappedResult(hComm2,&osRead,&dwOvRes,FALSE)){
                    readstat = FALSE;
                    continue;
                }
                if(dwOvRes == 1){
                    hasil2[i] = TempChar;
                    i++;
                    adaisi = TRUE;
                    if(TempChar == ';'){
                        readstat = FALSE;
                        continue;
                    }
                }
                break;
            case WAIT_OBJECT_0 + 1:
                break;
            case WAIT_TIMEOUT:{
                    readstat = FALSE;
                }
                break;
            }
        }
        counter++;

        if(counter > timeout*15 && adaisi == FALSE){
            sprintf(hasil2,"timeoutread#");
            CloseHandle(osRead.hEvent);
            CloseHandle(hComm2);
            return hasil2;
        }
    }
    CloseHandle(osRead.hEvent);
    CloseHandle(hComm2);
    return hasil2;
}

void parsingIsi()
{
    //printf("\nisi buffer:%s\n",isiBuffer);
    char mylog[10240];
//    sprintf(mylog,"receive buffer:%s",isiBuffer);
//    tulisLog(mylog);
    //PurgeComm(hComm,PURGE_RXCLEAR|PURGE_TXCLEAR);
    //portInUse = 0;

    if(isiBuffer[0] == '$'){
        char *hdr = strtok(isiBuffer,"#");
        char *isi = strtok(NULL,"#");

        if(strcmp(hdr,"$demog") == 0){
            sprintf(mylog,"receive buffer: data demografi");
        }
        else if(strcmp(hdr,"$machinelg") == 0){
            sprintf(mylog,"receive buffer: data log");
        }
        else{
            sprintf(mylog,"receive buffer:%s",isi);
        }

        if(strcmp(hdr,"$poll") == 0){
            strcpy(isiPoll,isi);
            respoll = 1;
        }
        else if(strcmp(hdr,"$info") == 0){
            strcpy(isiInfo,isi);
            resinfo = 1;
        }
        else if(strcmp(hdr,"$close") == 0){
            strcpy(isiClose,isi);
            resclose = 1;
        }
        else if(strcmp(hdr,"$putktp") == 0){
            strcpy(isiPut,isi);
            resput = 1;
        }
        else if(strcmp(hdr,"$reset") == 0){
            strcpy(isiReset,isi);
            resreset = 1;
        }
        else if(strcmp(hdr,"$finger") == 0){
            strcpy(isiFinger,isi);
            resfinger = 1;
        }
        else if(strcmp(hdr,"$demog") == 0){
            strcpy(isiDemog,isi);
            resdemog = 1;
        }
        else if(strcmp(hdr,"$machinelg") == 0){
            strcpy(isiLog,isi);
            reslog = 1;
        }
        else if(strcmp(hdr,"$switch") == 0){
            strcpy(isiSwitch,isi);
            resswitch = 1;
        }
        else if(strcmp(hdr,"$display") == 0){
            strcpy(isiDisplay,isi);
            resdisplay = 1;
        }
        else if(strcmp(hdr,"$setpow") == 0){
            strcpy(isiSetpow,isi);
            ressetpow = 1;
        }
        else if(strcmp(hdr,"$respow") == 0){
            strcpy(isiRespow,isi);
            resrespow = 1;
        }
        else if(strcmp(hdr,"$updateinit") == 0){
            strcpy(isiUpdateInit,isi);
            resupdateinit = 1;
        }
        else if(strcmp(hdr,"$updatefile") == 0){
            strcpy(isiUpdateFile,isi);
            resupdatefile = 1;
        }
        else if(strcmp(hdr,"$updatefinish") == 0){
            strcpy(isiUpdateFinish,isi);
            resupdatefinish = 1;
        }

        free(hdr);
        free(isi);
        //free(isiBuffer);
        memset(isiBuffer,0,sizeof(isiBuffer));

        tulisLog(mylog);
    }
}

void *threadRespon(void *x)
{
    char TempChar;
    DWORD NoBytesRead;
    int i = 0;
    BOOL simpanData = FALSE;
    char dataSebelum = '\0';
    do
    {
        ReadFile(hComm, &TempChar, sizeof(TempChar), &NoBytesRead, NULL);
        //printf("c:%c ,",TempChar);

        while(br == TRUE){Sleep(10);}

        //printf("temp:%c\n,",TempChar);
        if(TempChar == '$'){
            simpanData = TRUE;
        }

        if(simpanData){
            isiBuffer[i] = TempChar;
            //isiBuffer[i] = '0';
            i++;
        }

        if(TempChar == ';'){
            if(dataSebelum != ';'){
                simpanData = FALSE;
                parsingIsi();
            }
            i = 0;
        }

        dataSebelum = TempChar;

    }
    while (nunggurespon == 0);
    //printf("\n thread selesai.\n");
}

int searchreader()
{
    if(portnumber == 999)
    {
        char folde[FILENAME_MAX];
        _getcwd(folde,FILENAME_MAX);
        char pathfile[5000];
        sprintf(pathfile,"%s\\LenKTPelKiosLibUsb.l",folde);
        FILE *fp;
        char buff[255];
        int porttemp = 1;
        fp = fopen(pathfile,"r");
        if (fp != NULL){
            fscanf(fp, "%s", buff);
            porttemp = atoi(buff);
        }
        fclose(fp);

        if(porttemp != 0){
            char scport2[5];
            sprintf(scport2,"COM%d",porttemp);
            char* hasil2 = sendCommandSR(scport2,"ektpack#;",10);
            char *split12 = strtok(hasil2,"#");
            char *split22 = strtok(NULL,"#");
            if(strcmp(split12,"$ack") == 0 && strcmp(split22,"len442ektpreader") == 0){
                portnumber = porttemp;
            }

            free(hasil2);
            free(split12);
            free(split22);
        }

        if(portnumber == 999)
        {
            int i=0;
            for(i=1; i<=50; i++){
                char scport[5];
                sprintf(scport,"COM%d",i);
                char* hasil = sendCommandSR(scport,"ektpack#;",10);
                //int c = 0;
                //char **arr = NULL;
                //c = split(hasil, '#', &arr);
                char *split1 = strtok(hasil,"#");
                char *split2 = strtok(NULL,"#");
                if(strcmp(split1,"$ack") == 0 && strcmp(split2,"len442ektpreader") == 0){
                    portnumber = i;
                    break;
                }
                free(hasil);
                free(split1);
                free(split2);
                //free(arr);
            }
            FILE *fp;
            fp = fopen(pathfile, "w");
            if (fp != NULL){
                char myport[5];
                sprintf(myport,"%d",portnumber);
                fputs(myport, fp);
            }

            fclose(fp);
        }
    }
    //free(hasil2);
    memset(hasil2,0,sizeof(hasil2));
    return portnumber;
}

void *sendCommandReceiveTr()
{
    char mylog[1024];
    sprintf(mylog,"$$$send command receive$$$Begin Process");
    tulisLog(mylog);

    portInUse = 1;
    PurgeComm(hComm,PURGE_TXCLEAR);
    BOOL   Status;
    int pjg = strlen(commandGlobal);
    char lpBuffer[pjg+1];
    strcpy(lpBuffer,commandGlobal);
    DWORD  dNoOFBytestoWrite;
    DWORD  dNoOfBytesWritten = 0;

    dNoOFBytestoWrite = sizeof(lpBuffer);

    sprintf(mylog,"$$$send command receive$$$send command:%s",commandGlobal);
    tulisLog(mylog);

    Status = WriteFile(hComm,               // Handle to the Serialport
                       lpBuffer,            // Data to be written to the port
                       dNoOFBytestoWrite,   // No of bytes to write into the port
                       &dNoOfBytesWritten,  // No of bytes written to the port
                       NULL);

    if (Status == FALSE){
        resultKirim = 99;
    }
    else{
        resultKirim = 0;
    }

    free(lpBuffer);

    Sleep(500);
    //PurgeComm(hComm,PURGE_RXCLEAR);

    char TempChar;
    DWORD NoBytesRead;
    int i = 0;
    BOOL simpanData = FALSE;
    char dataSebelum = '\0';
    sprintf(mylog,"$$$send command receive$$$begin receive");
    tulisLog(mylog);
    do
    {
        ReadFile(hComm, &TempChar, sizeof(TempChar), &NoBytesRead, NULL);

        if(TempChar == '$'){
            simpanData = TRUE;
        }

        if(simpanData){
            isiBuffer[i] = TempChar;
            i++;
        }

        if(TempChar == ';'){
            if(dataSebelum != ';'){
                simpanData = FALSE;
                parsingIsi();
                //nunggurespon = 1;
            }
            i = 0;
        }

        dataSebelum = TempChar;

    }
    while (nunggurespon == 0);
    //portInUse = 0;
    sprintf(mylog,"$$$send command receive$$$End Process");
    tulisLog(mylog);
}

void sendCommandReceive(char* command)
{
    pthread_cancel(tid1);
    pthread_cancel(tid2);
    strcpy(commandGlobal,command);
    pthread_create(&tid1,NULL,sendCommandReceiveTr,NULL);
}

int sendCommandOnly(char* command)
{
//    if(kirimcmd >= 50){
//        cAOReaderAgain();
//        kirimcmd = 0;
//    }

    char mylog[1024];
    sprintf(mylog,"$$$send command$$$Begin Process");
    tulisLog(mylog);

    portInUse = 1;
    PurgeComm(hComm,PURGE_RXCLEAR|PURGE_TXCLEAR);
    BOOL   Status;
    int pjg = strlen(command);
    char lpBuffer[pjg+1];
    strcpy(lpBuffer,command);
    DWORD  dNoOFBytestoWrite;
    DWORD  dNoOfBytesWritten = 0;

    dNoOFBytestoWrite = sizeof(lpBuffer);

    sprintf(mylog,"send command:%s",command);
    tulisLog(mylog);

    Status = WriteFile(hComm,               // Handle to the Serialport
                       lpBuffer,            // Data to be written to the port
                       dNoOFBytestoWrite,   // No of bytes to write into the port
                       &dNoOfBytesWritten,  // No of bytes written to the port
                       NULL);

    free(lpBuffer);
    kirimcmd++;

    if (Status == FALSE){
        //cAOReaderAgain();
        sprintf(mylog,"send command failed, error number:%d",GetLastError());
        tulisLog(mylog);
        return 99;
    }

    sprintf(mylog,"$$$send command$$$End Process");
    tulisLog(mylog);

    return 0;
}

void *cAOReaderAgainTr(){
    char mylog[1024];
    sprintf(mylog,"+++Refresh Serial+++Begin process");
    tulisLog(mylog);

    br = FALSE;
    nunggurespon = 1;
    FlushFileBuffers(hComm);
    Sleep(200);

    sprintf(mylog,"+++Refresh Serial+++Close serial");
    tulisLog(mylog);

    PurgeComm(hComm,PURGE_RXCLEAR|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_TXABORT);
    CloseHandle(hComm);
    Sleep(200);

    sprintf(mylog,"+++Refresh Serial+++Open serial");
    tulisLog(mylog);

    int scport = searchreader();
    if(scport > 50){
        //hasil = -1001;
        //strcpy(error,"No Device Found");
        sprintf(mylog,"+++Refresh Serial+++No Device Found");
        tulisLog(mylog);
    }
    else{
        char port[6];
        sprintf(port,"COM%d",scport);
        char* result = sendCommandOP(port,"ektpopen#;");

        char *split1 = strtok(result,"#");

        if(strcmp(split1,"timeoutwrite") == 0)
        {
            //hasil = -1005;
            //strcpy(error,"Send Data Timeout");
            sprintf(mylog,"+++Refresh Serial+++Send Data Timeout");
            tulisLog(mylog);
        }
        else if(strcmp(split1,"timeoutread") == 0)
        {
            //hasil = -1007;
            //strcpy(error,"Receive Data Timeout");
            sprintf(mylog,"+++Refresh Serial+++Receive Data Timeout");
            tulisLog(mylog);
        }
        else if(strcmp(split1,"opendevicefailed") == 0)
        {
            //hasil = -1004;
            //strcpy(error,"Setup Device Param Failed");
            sprintf(mylog,"+++Refresh Serial+++Setup Device Param Failed");
            tulisLog(mylog);
        }
        else if(strcmp(split1,"$open") == 0)
        {
            char *split2 = strtok(NULL,"#");
            if(strcmp(split2,"0") == 0)
            {
                //hasil = 0;
                //strcpy(error,"ERR_OK");
                sprintf(mylog,"+++Refresh Serial+++Refress success");
                tulisLog(mylog);

                openstatus = 1;
                nunggurespon = 0;
                br = TRUE;
                pthread_t tid;
                pthread_create(&tid,NULL,threadRespon,NULL);
            }
            else {
                //hasil = -1003;
                //strcpy(error,"Open Device Failed");
                sprintf(mylog,"+++Refresh Serial+++Open Device Failed");
                tulisLog(mylog);
            }
            free(split2);
        }
        free(split1);
        free(result);
    }

//    char port[6];
//    int scport = searchreader();
//    sprintf(port,"COM%d",scport);
//    char* result = OpenOnly(port);
//    if(strcmp(result,"success") == 0){
//        nunggurespon = 0;
//        pthread_t tid;
//        pthread_create(&tid,NULL,threadRespon,NULL);
//        Sleep(200);
//        char command[100];
//        openstatus = 1;
//        sprintf(mylog,"+++Refresh Serial+++Open Success");
//        tulisLog(mylog);
//    }
//    else{
//        DWORD dwerr;
//        COMSTAT commstat;
//        ClearCommError(hComm,&dwerr,&commstat);
//        sprintf(mylog,"+++Refresh Serial+++Open serial failed, error number:%d",GetLastError());
//        tulisLog(mylog);
//    }

    sprintf(mylog,"+++Refresh Serial+++End process");
    tulisLog(mylog);
    Sleep(500);
}

void cAOReaderAgain(){
    pthread_cancel(tid1);
    pthread_cancel(tid2);
    pthread_create(&tid2,NULL,cAOReaderAgainTr,NULL);
}

char* sendAndRec(char* command)
{
//    char mylog[1024];
//    sprintf(mylog,"send command:%s",command);
//    tulisLog(mylog);

    BOOL   Status;
    int pjg = strlen(command);
    char lpBuffer[pjg+1];
    strcpy(lpBuffer,command);
    DWORD  dNoOFBytestoWrite;
    DWORD  dNoOfBytesWritten = 0;

    dNoOFBytestoWrite = sizeof(lpBuffer);

    Status = WriteFile(hComm,               // Handle to the Serialport
                       lpBuffer,            // Data to be written to the port
                       dNoOFBytestoWrite,   // No of bytes to write into the port
                       &dNoOfBytesWritten,  // No of bytes written to the port
                       NULL);

    free(lpBuffer);

    if (Status == FALSE)
        return "error";

    DWORD dwEventMask, NoBytesRead;
    char TempChar;
    char SerialBuffer[1024], hasil3[1024];
    int i = 0;
    Status = WaitCommEvent(hComm, &dwEventMask, NULL);
    if (Status == FALSE)
    {
        sprintf(hasil3,"opendevicefailed#");
    }
    else
    {
        int err;
        do
        {
            Status = ReadFile(hComm, &TempChar, sizeof(TempChar), &NoBytesRead, NULL);
            err = GetLastError();
            SerialBuffer[i] = TempChar;
            i++;
        }
        while (NoBytesRead > 0 && err != ERROR_IO_PENDING);

        int j =0;
        for (j = 0; j < i-1; j++)
        {
            hasil3[j] = SerialBuffer[j];
        }
    }
    //free(SerialBuffer);
    return hasil3;
}

void tulisLog(char tulis[10240])
{
    char folde[FILENAME_MAX];
    _getcwd(folde,FILENAME_MAX);
    char pathfile[5000];
    sprintf(pathfile,"%s\\LenKTPelKiosLibUsb.log",folde);

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    FILE *fp;
    fp = fopen(pathfile, "a+");
    if (fp != NULL){
        char mylog[10240];
        sprintf(mylog,"%d/%d/%d %d:%d:%d - %s\n",tm.tm_mday,tm.tm_mon+1,tm.tm_year+1900,tm.tm_hour,tm.tm_min,tm.tm_sec,tulis);
        fputs(mylog, fp);
    }
    fclose(fp);
}

void bacafile()
{
    FILE *fp;
    char buff[255];
    fp = fopen("D:\\test.txt","r");
    if (fp == NULL){
        printf("Could not open file");
    }
    else{
        printf("Hasil Read file: ");
        fscanf(fp, "%s", buff);
        printf("%s",buff);
    }
    fclose(fp);
}

//int split (const char *str, char c, char ***arr)
//{
//    int count = 1;
//    int token_len = 1;
//    int i = 0;
//    char *p;
//    char *t;
//
//    p = str;
//    while (*p != '\0')
//    {
//        if (*p == c)
//            count++;
//        p++;
//    }
//
//    *arr = (char**) malloc(sizeof(char*) * count);
//    if (*arr == NULL)
//        exit(1);
//
//    p = str;
//    while (*p != '\0')
//    {
//        if (*p == c)
//        {
//            (*arr)[i] = (char*) malloc( sizeof(char) * token_len );
//            if ((*arr)[i] == NULL)
//                exit(1);
//
//            token_len = 0;
//            i++;
//        }
//        p++;
//        token_len++;
//    }
//    (*arr)[i] = (char*) malloc( sizeof(char) * token_len );
//    if ((*arr)[i] == NULL)
//        exit(1);
//
//    i = 0;
//    p = str;
//    t = ((*arr)[i]);
//    while (*p != '\0')
//    {
//        if (*p != c && *p != '\0')
//        {
//            *t = *p;
//            t++;
//        }
//        else
//        {
//            *t = '\0';
//            i++;
//            t = ((*arr)[i]);
//        }
//        p++;
//    }
//    //free(arr);
//    return count;
//}

__declspec(dllexport) int ektp_getDLL(char error[100], char dllVersion[100])
{
    strcpy(error,"ERR_OK");
    strcpy(dllVersion,"4.2.190.216");
    return 0;
}

__declspec(dllexport) int ektp_getAPI(char error[100], char dllVersion[100])
{
    strcpy(error,"ERR_OK");
    strcpy(dllVersion,"01.02.00");
    return 0;
}

__declspec(dllexport) int ektp_open(char error[100])
{
    char mylog[1024];
    sprintf(mylog,"###ektp_open###Receive request call");
    tulisLog(mylog);

    int hasil = 0;
    nunggurespon = 1;
    CloseHandle(hComm);
    openstatus = 0;
    Sleep(1000);

        int scport = searchreader();
        if(scport > 50){
            hasil = -1001;
            strcpy(error,"No Device Found");
        }
        else{
            char port[6];
            sprintf(port,"COM%d",scport);
            char* result = sendCommandOP(port,"ektpopen#;");

            char* split1 = strtok(result,"#");

            if(strcmp(split1,"timeoutwrite") == 0)
            {
                hasil = -1005;
                strcpy(error,"Send Data Timeout");
            }
            else if(strcmp(split1,"timeoutread") == 0)
            {
                hasil = -1007;
                strcpy(error,"Receive Data Timeout");
            }
            else if(strcmp(split1,"opendevicefailed") == 0)
            {
                hasil = -1004;
                strcpy(error,"Setup Device Param Failed");
            }
            else if(strcmp(split1,"$open") == 0)
            {
                char *split2 = strtok(NULL,"#");
                if(strcmp(split2,"0") == 0)
                {
                    hasil = 0;
                    strcpy(error,"ERR_OK");
                    openstatus = 1;
                    //nunggurespon = 0;
                    //br = TRUE;
                    //pthread_t tid;
                    //pthread_create(&tid,NULL,threadRespon,NULL);
                }
                else {
                    hasil = -1003;
                    strcpy(error,"Open Device Failed");
                }
                free(split2);
                printf("4");
            }

            //free(split1);
            //free(result);
        }

    sprintf(mylog,"###ektp_open###Return request call:%d,%s",hasil,error);
    tulisLog(mylog);

    return hasil;
}

__declspec(dllexport) int ektp_close(char error[100])
{
    char mylog[1024];
    sprintf(mylog,"###ektp_close###Receive request call");
    tulisLog(mylog);

    int hasil = 99;
    if(openstatus == 1)
    {
        br = TRUE;
        //int result = sendCommandOnly("ektpclose#;");
        resultKirim = 88;
        nunggurespon = 0;
        sendCommandReceive("ektpclose#;");
        int cnt2 = 0;
        int tt2 = 20;
        while(resultKirim == 88 && cnt2 < tt2){
            Sleep(100);
            cnt2++;
        }

        int timeout = 2;
        if(resultKirim == 0){
            br = FALSE;
            Sleep(500);
            int cnt = 0;
            int tt = timeout * 10;

            while(resclose == 0 && cnt < tt){
                Sleep(100);
                cnt++;
            }
            nunggurespon = 1;

            if(resclose == 0){
                hasil = -1007;
                strcpy(error,"Receive Data Timeout");
            }
            else{
                if(strcmp(isiClose,"0") == 0){
                    hasil = 0;
                    strcpy(error,"ERR_OK");
                    nunggurespon = 1;
                    CloseHandle(hComm);
                    openstatus = 0;
                }
                else{
                    hasil = -1;
                    strcpy(error,"ERR_ERROR");
                }
                resclose = 0;
            }
        }
        else if(resultKirim == 88){
            hasil = -1005;
            strcpy(error,"Send Data Timeout");
            cAOReaderAgain();
        }
        else{
            hasil = -1006;
            strcpy(error,"Send Data Failed");
            cAOReaderAgain();
        }
    }
    else{
        hasil = -1012;
        strcpy(error,"Device Not Open");
    }

    sprintf(mylog,"###ektp_close###Return request call:%d,%s",hasil,error);
    tulisLog(mylog);

    return hasil;
}

__declspec(dllexport) int ektp_info(char error[100], char hid[50], char sn[50], char fw[50], char status[30])
{
    char mylog[1024];
    sprintf(mylog,"###ektp_info###Receive request call");
    tulisLog(mylog);

    int hasil = 0;

    if(openstatus == 1)
    {
        while(portInUse == 1){
            Sleep(500);
        }

        resultKirim = 88;
        nunggurespon = 0;
        sendCommandReceive("ektpinfo#;");
        int cnt2 = 0;
        int tt2 = 20;
        while(resultKirim == 88 && cnt2 < tt2){
            Sleep(100);
            cnt2++;
        }

        int timeout = 5;
        if(resultKirim == 0){
            br = FALSE;
            Sleep(500);
            int cnt = 0;
            int tt = timeout * 10;
            while(resinfo == 0 && cnt < tt){
                Sleep(100);
                cnt++;
            }
            nunggurespon = 1;

            if(resinfo == 0){
                hasil = -1007;
                strcpy(error,"Receive Data Timeout");
                cAOReaderAgain();
            }
            else{
                char *split1 = strtok(isiInfo,",");
                if(strcmp(split1,"error") == 0){
                    hasil = -1006;
                    strcpy(error,"Send Data Failed");
                }
                else{
                    strcpy(error,"ERR_OK");
                    strcpy(hid,split1);
                    strcpy(sn,strtok(NULL,","));
                    strcpy(fw,strtok(NULL,","));
                    strcpy(status,"01");
                }
                free(split1);
                resinfo = 0;
                //free(isiInfo);
                memset(isiInfo,0,sizeof(isiInfo));
            }
        }
        else if(resultKirim == 88){
            hasil = -1005;
            strcpy(error,"Send Data Timeout");
            cAOReaderAgain();
        }
        else{
            hasil = -1006;
            strcpy(error,"Send Data Failed");
            cAOReaderAgain();
        }

        portInUse = 0;
    }
    else{
        hasil = -1012;
        strcpy(error,"Device Not Open");
    }

    sprintf(mylog,"###ektp_info###Return request call:%d,%s",hasil,error);
    tulisLog(mylog);

    return hasil;
}

__declspec(dllexport) int ektp_putKTP(char error[100], char dm[80], int timeout, char ft[6])
{
    char mylog[1024];
    sprintf(mylog,"###ektp_putKTP###Receive request call");
    tulisLog(mylog);

    int hasil = 0;

    if(openstatus == 1)
    {
        while(portInUse == 1){
            Sleep(500);
        }

        lastPutKtpStatus = 18;
        br = TRUE;
        char command[100];
        //fflush(stdin);
        sprintf(command,"ektpput#%d#%s#;",timeout,dm);
        //int result = sendCommandOnly(command);
        resultKirim = 88;
        nunggurespon = 0;
        sendCommandReceive(command);
        int cnt2 = 0;
        int tt2 = 20;
        while(resultKirim == 88 && cnt2 < tt2){
            Sleep(100);
            cnt2++;
        }

        if(resultKirim == 0){
            br = FALSE;
            Sleep(500);
            int cnt = 0;
            int tt = (timeout+1) * 10;

            while(resput == 0 && cnt < tt){
                Sleep(100);
                cnt++;
            }

            nunggurespon = 1;

            if(resput == 0){
                hasil = -1007;
                strcpy(error,"Receive Data Timeout");
                lastPutKtpStatus = 3;
                lastReaderStatus = 0;
                cAOReaderAgain();
            }
            else{
                char *split1 = strtok(isiPut,",");
                char *split2 = strtok(NULL,",");
                char *split3 = strtok(NULL,",");
                if(strcmp(split1,"error") == 0){
                    hasil = -1;
                    strcpy(error,split2);
                    lastPutKtpStatus = 27;
                    lastReaderStatus = 24;
                }
                else{
                    strcpy(error,"ERR_OK");
                    char jari[10];
                    sprintf(jari,"%s,%s",split2,split3);
                    strcpy(ft,jari);
                    lastPutKtpStatus = 34;
                    lastReaderStatus = 0;
                }
                free(split1);
                free(split2);
                free(split3);
                memset(isiPut,0,sizeof(isiPut));
                resput = 0;
                //cAOReaderAgain();
            }
        }
        else if(resultKirim == 88){
            hasil = -1005;
            strcpy(error,"Send Data Timeout");
            cAOReaderAgain();
            lastPutKtpStatus = 24;
            lastReaderStatus = 0;
        }
        else{
            hasil = -1006;
            strcpy(error,"Send Data Failed");
            cAOReaderAgain();
            lastPutKtpStatus = 24;
            lastReaderStatus = 0;
        }
        portInUse = 0;
    }
    else{
        hasil = -1012;
        strcpy(error,"Device Not Open");
    }

    sprintf(mylog,"###ektp_putKTP###Return request call:%d,%s",hasil,error);
    tulisLog(mylog);

    return hasil;
}

__declspec(dllexport) int ektp_poll(char error[100], unsigned char *readerStatus, unsigned char *putKTPStatus, unsigned char *verifyFingerStatus, unsigned char *deviceStatus)
{
    char mylog[1024];
    sprintf(mylog,"###ektp_poll###Receive request call");
    tulisLog(mylog);

    int hasil = 0;

    if(openstatus == 1)
    {
        if(portInUse == 1)
        {
            *readerStatus = lastReaderStatus;
            *putKTPStatus = lastPutKtpStatus;
            *verifyFingerStatus = lastVerifyFingerStatus;
            *deviceStatus = lastDeviceStatus;
            Sleep(1000);
        }
        else
        {
            br = TRUE;
            //fflush(stdin);
            //free(isiBuffer);
            //memset(isiBuffer,0,sizeof(isiBuffer));
            //free(isiPoll);
            //memset(isiPoll,0,sizeof(isiPoll));
            //FlushFileBuffers(hComm);
            //int result = sendCommandOnly("ektppoll#;");
            resultKirim = 88;
            nunggurespon = 0;
            sendCommandReceive("ektppoll#;");
            int cnt2 = 0;
            int tt2 = 20;
            while(resultKirim == 88 && cnt2 < tt2){
                Sleep(100);
                cnt2++;
            }

            int timeout = 2;
            *readerStatus = '\0';
            *putKTPStatus = '\0';
            *verifyFingerStatus = '\0';
            *deviceStatus = '\0';
            if(resultKirim == 0){
                br = FALSE;
                Sleep(500);
                int cnt = 0;
                int tt = timeout * 10;

                while(respoll == 0 && cnt < tt){
                    Sleep(100);
                    cnt++;
                }
                nunggurespon = 1;

                //br = TRUE;
                if(respoll == 0){
                    hasil = -1007;
                    strcpy(error,"Receive Data Timeout");
                    cAOReaderAgain();
                }
                else{
                    strcpy(error,"ERR_OK");
                    char *split1 = strtok(isiPoll,",");
                    if(strcmp(split1,"error") == 0){
                        hasil = -1012;
                        strcpy(error,"Device Not Open");
                    }
                    else{
                        int rdst = atoi(split1);
                        int ptst = atoi(strtok(NULL,","));
                        int vrst = atoi(strtok(NULL,","));
                        int dvst = atoi(strtok(NULL,","));;

                        *readerStatus = rdst;
                        *putKTPStatus = ptst;
                        *verifyFingerStatus = vrst;
                        *deviceStatus = dvst;

                        lastReaderStatus = rdst;
                        lastPutKtpStatus = ptst;
                        lastVerifyFingerStatus = vrst;
                        lastDeviceStatus = dvst;
                    }

                    free(split1);
                    memset(isiPoll,0,sizeof(isiPoll));
                    respoll = 0;
                    //free(isiBuffer);
                    memset(isiBuffer,0,sizeof(isiBuffer));
                }
            }
            else if(resultKirim == 88){
                hasil = -1005;
                strcpy(error,"Send Data Timeout");
                cAOReaderAgain();
            }
            else{
                hasil = -1006;
                strcpy(error,"Send Data Failed");
                cAOReaderAgain();
            }
            portInUse = 0;
        }
    }
    else{
        hasil = -1012;
        strcpy(error,"Device Not Open");
    }

    sprintf(mylog,"###ektp_poll###Return request call:%d,%s",hasil,error);
    tulisLog(mylog);

    return hasil;
}

__declspec(dllexport) int ektp_reset(char error[100], char type[2])
{
    char mylog[1024];
    sprintf(mylog,"###ektp_reset###Receive request call");
    tulisLog(mylog);

    int hasil = 0;

    if(openstatus == 1)
    {
        while(portInUse == 1){
            Sleep(500);
        }

        br = TRUE;
        char command[100];
        //fflush(stdin);
        sprintf(command,"ektpreset#%s#;",type);
        //int result = sendCommandOnly(command);
        resultKirim = 88;
        nunggurespon = 0;
        sendCommandReceive(command);
        int cnt2 = 0;
        int tt2 = 20;
        while(resultKirim == 88 && cnt2 < tt2){
            Sleep(100);
            cnt2++;
        }

        int timeout = 2;
        if(resultKirim == 0){
            br = FALSE;
            Sleep(500);
            int cnt = 0;
            int tt = timeout * 10;

            while(resreset == 0 && cnt < tt){
                Sleep(100);
                cnt++;
            }
            nunggurespon = 1;

            if(resreset == 0){
                hasil = -1007;
                strcpy(error,"Receive Data Timeout");
                cAOReaderAgain();
            }
            else{
                if(strcmp(isiReset,"0") == 0){
                    hasil = 0;
                    strcpy(error,"ERR_OK");
                    Sleep(1000);
//                    nunggurespon = 1;
//                    CloseHandle(hComm);
//                    openstatus = 0;
                }
                else{
                    hasil = -1;
                    strcpy(error,"ERR_ERROR");
                }
                resreset = 0;
                //free(isiReset);
            }
        }
        else if(resultKirim == 88){
            hasil = -1005;
            strcpy(error,"Send Data Timeout");
            cAOReaderAgain();
        }
        else{
            hasil = -1006;
            strcpy(error,"Send Data Failed");
            cAOReaderAgain();
        }
        portInUse = 0;
    }
    else{
        hasil = -1012;
        strcpy(error,"Device Not Open");
    }

    sprintf(mylog,"###ektp_reset###Return request call:%d,%s",hasil,error);
    tulisLog(mylog);

    return hasil;
}

__declspec(dllexport) int ektp_verifyFinger(char error[100], char dm[80], int timeout, char ft[6], char opid[20], char opnik[20])
{
    char mylog[1024];
    sprintf(mylog,"###ektp_verifyFinger###Receive request call");
    tulisLog(mylog);

    int hasil = 0;
    //cAOReaderAgain();

    if(openstatus == 1)
    {
        while(portInUse == 1){
            Sleep(500);
        }

        lastVerifyFingerStatus = 19;
        br = TRUE;
        char command[100];
        //fflush(stdin);
        sprintf(command,"ektpfinger#%d#%s#%s#%s#%s#;",timeout,ft,opid,opnik,dm);
        //int result = sendCommandOnly(command);
        resultKirim = 88;
        nunggurespon = 0;
        sendCommandReceive(command);
        int cnt2 = 0;
        int tt2 = 20;
        while(resultKirim == 88 && cnt2 < tt2){
            Sleep(100);
            cnt2++;
        }

        if(resultKirim == 0){
            br = FALSE;
            Sleep(500);
            int cnt = 0;
            int tt = (timeout+1) * 10;

            while(resfinger == 0 && cnt < tt){
                Sleep(100);
                cnt++;
            }
            nunggurespon = 1;

            if(resfinger == 0){
                hasil = -1007;
                strcpy(error,"Receive Data Timeout");
                lastVerifyFingerStatus = 3;
                lastReaderStatus = 0;
                cAOReaderAgain();
            }
            else{
                //int cc = 0;
                //char **arrr = NULL;
                //cc = split(isiFinger, ',', &arrr);
                char *split1 = strtok(isiFinger,",");
                if(strcmp(split1,"error") == 0){
                    hasil = -1;
                    strcpy(error,strtok(NULL,","));
                    lastVerifyFingerStatus = 0;
                    lastReaderStatus = 0;
                }
                else{
                    if(strcmp(isiFinger,"Y")==0){
                        strcpy(error,"Fingerprint match");
                        lastVerifyFingerStatus = 35;
                        lastReaderStatus = 0;
                    }
                    else{
                        hasil = -1;
                        strcpy(error,"Fingerprint not match");
                        lastVerifyFingerStatus = 30;
                    }
                }

                resfinger = 0;
                free(split1);
                //free(isiFinger);
                //cAOReaderAgain();
            }
        }
        else if(resultKirim == 88){
            hasil = -1005;
            strcpy(error,"Send Data Timeout");
            cAOReaderAgain();
            lastVerifyFingerStatus = 24;
            lastReaderStatus = 0;
        }
        else{
            hasil = -1006;
            strcpy(error,"Send Data Failed");
            cAOReaderAgain();
            lastVerifyFingerStatus = 24;
            lastReaderStatus = 0;
        }
        portInUse = 0;
    }
    else{
        hasil = -1012;
        strcpy(error,"Device Not Open");
    }

    sprintf(mylog,"###ektp_verifyFinger###Return request call:%d,%s",hasil,error);
    tulisLog(mylog);

    return hasil;
}

__declspec(dllexport) int ektp_getDataDemography(char error[100], int timeout, char opid[20], char opnik[20], char auid[20], char aunik[20], char** ektpdata)
{
    char mylog[102400];
    sprintf(mylog,"###ektp_getDataDemography###Receive request call");
    tulisLog(mylog);

    int hasil = 0;

    if(openstatus == 1)
    {
        while(portInUse == 1){
            Sleep(500);
        }

        br = TRUE;
        char command[100];
        //fflush(stdin);
        sprintf(command,"ektpdemog#%d#%s#%s#;",timeout,opid,opnik);
        //int result = sendCommandOnly(command);
        resultKirim = 88;
        nunggurespon = 0;
        sendCommandReceive(command);
        int cnt2 = 0;
        int tt2 = 20;
        while(resultKirim == 88 && cnt2 < tt2){
            Sleep(100);
            cnt2++;
        }

        *ektpdata = '\0';
        if(resultKirim == 0){
            br = FALSE;
            Sleep(500);
            int cnt = 0;
            int tt = (timeout+2) * 10;

            while(resdemog == 0 && cnt < tt){
                //printf("nunggu respon cnt=%d, tt=%d\n",cnt,tt);
                Sleep(100);
                cnt++;
            }
            nunggurespon = 1;

            if(resdemog == 0){
                hasil = -1007;
                strcpy(error,"Receive Data Timeout");
                cAOReaderAgain();
            }
            else{
                //int c = 0;
                //char **arr = NULL;
                //c = split(isiDemog, ',', &arr);
                char *p = isiDemog;
                int i=0;
                char arr[3][1024];
                int x=0;
                char temp[1024];
                char c = ',';
                //char *t;
                while (*p != '\0')
                {
                    if (*p != c && *p != '\0')
                    {
                        temp[x] = *p;
                        x++;
                    }
                    else
                    {
                        temp[x] = '\0';
                        strcpy(arr[i],temp);
                        x=0;
                        i++;
                        if(i==2){
                            break;
                        }
                    }
                    p++;
                }
//                char isiDemog2[18096];
//                printf("1");
//                strcpy(isiDemog2,isiDemog);
//                printf("2");
//                char *split1 = strtok(isiDemog2,",");
//                printf("3");
//                if(strcmp(split1,"error") == 0){
//                    hasil = -1;
//                    //strcpy(error,arr[1]);
//                    strcpy(error,strtok(NULL,","));
//                }
//                else{
//                    printf("4");
//                    strcpy(error,"ERR_OK");
//                    printf("5");
//                    //*ektpdata = isiDemog;
//                    int i = 0;
//                    while(isiDemog[i] != '\0'){
//                        ektpdata[i] = isiDemog[i];
//                        i++;
//                    }
//                    ektpdata[i] = '\0';
//                    //ektpdata = isiDemog;
//                    //printf("isi Demog: %s",isiDemog);
//                    printf("6");
//                }
//                if(strcmp(arr[0],"error") == 0){
//                    hasil = -1;
//                    strcpy(error,arr[1]);
//                }
//                else{
//                    strcpy(error,"ERR_OK");
//                    //*ektpdata = isiDemog;
//                    strcpy(ektpdata,isiDemog);
//                }
                if(strcmp(arr[0],"error")== 0){
                    hasil = -1;
                    strcpy(error,arr[1]);
                }
                else{
                    strcpy(error,"ERR_OK");
                    //strcpy(*ektpdata,isiDemog);
                    *ektpdata = isiDemog;
                }
                resdemog = 0;
                //free(isiDemog);
                //free(isiDemog2);
                //free(arr);
                free(p);
                //free(t);
                //free(temp);
                //cAOReaderAgain();
            }
        }
        else if(resultKirim == 88){
            hasil = -1005;
            strcpy(error,"Send Data Timeout");
            cAOReaderAgain();
            lastVerifyFingerStatus = 24;
            lastReaderStatus = 0;
        }
        else{
            hasil = -1006;
            strcpy(error,"Send Data Failed");
            cAOReaderAgain();
        }
        portInUse = 0;
    }
    else{
        hasil = -1012;
        strcpy(error,"Device Not Open");
    }

    sprintf(mylog,"###ektp_getDataDemography###Return request call:%d,%s",hasil,error);
    tulisLog(mylog);

    return hasil;
}

__declspec(dllexport) int ektp_getMachineLog(char error[100], char date[10], char** map)
{
    char mylog[102400];
    sprintf(mylog,"###ektp_getMachineLog###Receive request call");
    tulisLog(mylog);

    int hasil = 0;

    if(openstatus == 1)
    {
        while(portInUse == 1){
            Sleep(500);
        }

        br = TRUE;
        char command[100];
        //fflush(stdin);
        sprintf(command,"ektplog#%s#;",date);
        //int result = sendCommandOnly(command);
        resultKirim = 88;
        nunggurespon = 0;
        sendCommandReceive(command);
        int cnt2 = 0;
        int tt2 = 20;
        while(resultKirim == 88 && cnt2 < tt2){
            Sleep(100);
            cnt2++;
        }

        int timeout = 30;
        *map = '\0';
        if(resultKirim == 0){
            br = FALSE;
            Sleep(500);
            int cnt = 0;
            int tt = timeout * 10;
            while(reslog == 0 && cnt < tt){
                Sleep(100);
                cnt++;
            }
            nunggurespon = 1;

            if(reslog == 0){
                hasil = -1007;
                strcpy(error,"Receive Data Timeout");
                cAOReaderAgain();
            }
            else{
                //int cc = 0;
                //char **arrr = NULL;
                //cc = split(isiLog, ',', &arrr);
                char isiLog2[102400];
                strcpy(isiLog2,isiLog);
                //printf("isi log:%s",isiLog);
                //printf("isi log2:%s",isiLog2);
                char *split1 = strtok(isiLog,",");
                char *split2 = strtok(NULL,",");
                if(strcmp(split1,"error") == 0){
                    hasil = -1;
                    strcpy(error,split2);
                }
                else if(strcmp(split1,"kosong") == 0){
                    strcpy(error,"ERR_OK");
                }
                else{
                    strcpy(error,"ERR_OK");
                    *map = isiLog2;
                    //strcpy(map,isiLog2);
                }

                reslog = 0;
                //free(isiLog2);
                free(split1);
                free(split2);
            }
        }
        else if(resultKirim == 88){
            hasil = -1005;
            strcpy(error,"Send Data Timeout");
            cAOReaderAgain();
            lastVerifyFingerStatus = 24;
            lastReaderStatus = 0;
        }
        else{
            hasil = -1006;
            strcpy(error,"Send Data Failed");
            cAOReaderAgain();
        }

        portInUse = 0;
    }
    else{
        hasil = -1012;
        strcpy(error,"Device Not Open");
    }

    sprintf(mylog,"###ektp_getMachineLog###Return request call:%d,%s",hasil,error);
    tulisLog(mylog);

    return hasil;
}

__declspec(dllexport) int ektp_dispMessage(char error[100], char dspMessage[50])
{
    char mylog[1024];
    sprintf(mylog,"==============================\nReceive request call ektp_dispMessage");
    tulisLog(mylog);

    int hasil = 0;

    if(openstatus == 1)
    {
        hasil = 1;
        strcpy(error,"Device not supported");
        Sleep(1000);
    }
    else{
        hasil = -1012;
        strcpy(error,"Device Not Open");
    }

    return hasil;
}

__declspec(dllexport) int ektp_switchMode(char error[100], int mode)
{
    char mylog[1024];
    sprintf(mylog,"==============================\nReceive request call ektp_switchMode");
    tulisLog(mylog);

    int hasil = 0;

    if(openstatus == 1)
    {
        hasil = 1;
        strcpy(error,"Device not supported");
        Sleep(1000);
    }
    else{
        hasil = -1012;
        strcpy(error,"Device Not Open");
    }

    sprintf(mylog,"Return request call ektp_switchMode\n=============================");
    tulisLog(mylog);

    return hasil;
}

__declspec(dllexport) int ektp_setPowText(char error[100], char powText[256])
{
    char mylog[1024];
    sprintf(mylog,"==============================\nReceive request call ektp_setPowText");
    tulisLog(mylog);

    int hasil = 0;

    if(openstatus == 1)
    {
        hasil = 1;
        strcpy(error,"Device not supported");
        Sleep(1000);
    }
    else{
        hasil = -1012;
        strcpy(error,"Device Not Open");
    }

    sprintf(mylog,"Return request call ektp_setPowText\n=============================");
    tulisLog(mylog);

    return hasil;
}

__declspec(dllexport) int ektp_resPowText(char error[100])
{
    char mylog[1024];
    sprintf(mylog,"==============================\nReceive request call ektp_resPowText");
    tulisLog(mylog);

    int hasil = 0;

    if(openstatus == 1)
    {
        hasil = 1;
        strcpy(error,"Device not supported");
        Sleep(1000);
    }
    else{
        hasil = -1012;
        strcpy(error,"Device Not Open");
    }

    sprintf(mylog,"Return request call ektp_resPowText\n=============================");
    tulisLog(mylog);

    return hasil;
}

char* readFileBytes(const char *name)
{
    FILE *fl = fopen(name, "r");
    fseek(fl, 0, SEEK_END);
    long len = ftell(fl);
    char *ret = malloc(len);
    fseek(fl, 0, SEEK_SET);
    fread(ret, 1, len, fl);
    fclose(fl);
    return ret;
}

int updateFinish()
{
    br = TRUE;
    char command[100];
    sprintf(command,"ektpupdatefinish#qwerty#;");
    //int result = sendCommandOnly(command);
    resultKirim = 88;
    nunggurespon = 0;
    sendCommandReceive(command);
    int cnt2 = 0;
    int tt2 = 20;
    while(resultKirim == 88 && cnt2 < tt2){
        Sleep(100);
        cnt2++;
    }

    int timeout = 2;
    int x = -1;
    if(resultKirim == 0){
        br = FALSE;
        Sleep(500);
        int cnt = 0;
        int tt = timeout * 10;

        while(resupdatefinish == 0 && cnt < tt){
            Sleep(100);
            cnt++;
        }
        nunggurespon = 1;

        if(resupdatefinish == 0){
            x = -1;
        }
        else{

            if(strcmp(isiUpdateFinish,"error") == 0){
                x = -1;
            }
            else if(strcmp(isiUpdateFinish,"0") == 0){
                x = 0;
            }
            else{
                x = atoi(isiUpdateFinish);
            }

            resupdatefinish = 0;
        }
    }
    return x;
}

void kirimFile(int fileLen, char* buffer)
{
    int x = -1;
    br = TRUE;
    //int arr = (fileLen*2)+100;
    char command[20000];
    sprintf(command,"ektpupdatefile#%d#%d#%d#%s#;",fileLen,0,fileLen,buffer);
    //int result = sendCommandOnly(command);
    resultKirim = 88;
    nunggurespon = 0;
    sendCommandReceive(command);
    int cnt2 = 0;
    int tt2 = 20;
    while(resultKirim == 88 && cnt2 < tt2){
        Sleep(100);
        cnt2++;
    }

    free(buffer);
    int timeout = 2;
    if(resultKirim == 0){
        br = FALSE;
        Sleep(500);
        int cnt = 0;
        int tt = timeout * 10;

        while(resupdatefile == 0 && cnt < tt){
            Sleep(100);
            cnt++;
        }
        nunggurespon = 1;

        if(resupdatefile == 0){
            x = -1;
        }
        else{
            if(strcmp(isiUpdateFile,"error") == 0){
                x = -1;
            }
            else if(strcmp(isiUpdateFile,"0") == 0){
                x = updateFinish();
                //printf("\nyyy:%d",x);
            }
            else{
                x = -1;
            }

            resupdatefile = 0;
        }
    }
    //nunggurespon = 1;
    //Sleep(1000);
    //printf("sebelum retrurn:%d",x);
    //return x;
}

int kirimFile2(int fileLen, char* buffer)
{
    //int x = -1;
    br = TRUE;
    char command[4100];
    int jumdata = 2000;
    int jum = jumdata;
    int divlen = (fileLen*2) / jumdata;
    int modlen = (fileLen*2) % jumdata;
    char dt1[4100];
    int lenbaru = jum/2;
    int i,awal=0,z;
    for(i=0; i<divlen; i++){
        awal=i*jumdata;
        //free(dt1);
        for(z=0; z<jumdata; z++){
            dt1[z] = buffer[awal+z];
        }
        dt1[jum] = '\0';

        sprintf(command,"ektpupdatefile#%d#%d#%d#%s#;",lenbaru,awal/2,((awal/2)+lenbaru-1),dt1);

        sendCommandOnly(command);
        fflush(stdin);
        fflush(stdout);
        Sleep(15);
    }

    if(modlen > 0){
        char dt2[modlen];
        awal = awal + jumdata;
        for(z=0; z<modlen; z++){
            dt2[z] = buffer[awal+z];
        }
        dt2[modlen] = '\0';
        sprintf(command,"ektpupdatefile#%d#%d#%d#%s#;",modlen/2,(awal/2),fileLen,dt2);
        sendCommandOnly(command);
    }
    Sleep(100);

    return updateFinish();
}


__declspec(dllexport) int ektp_update(char error[100], char *updateApp[256])
{
    int hasil = -1;
    FILE *file;
	char *buffer;
	unsigned long fileLen;

	//Open file
	file = fopen(updateApp, "rb");
	if (!file)
	{
		hasil = -1;
        strcpy(error,"Unable to open file");
		return hasil;
	}

	//Get file length
	fseek(file, 0, SEEK_END);
	fileLen=ftell(file);
	fseek(file, 0, SEEK_SET);
	//printf("Panjang file:%d\n",fileLen);

	//Allocate memory
	buffer=(char *)malloc(fileLen+1);
	if (!buffer)
	{
		//fprintf(stderr, "Memory error!");
        fclose(file);
        hasil = -1;
        strcpy(error,"Cannot alocate memory to update");
		return hasil;
	}

	//Read file contents into buffer
	fread(buffer, fileLen, 1, file);
	fclose(file);

	//Rubah byte ke hexa string
	int i;
    char hasils[((fileLen-1) * 2)-1];
    char hex_char[16] = "0123456789abcdef";

    for (i = 0; i < fileLen; ++i)
    {
        // High nybble
        hasils[i<<1] = hex_char[(buffer[i] >> 4) & 0x0f];
        // Low nybble
        hasils[(i<<1) + 1] = hex_char[buffer[i] & 0x0f];
    }
    //printf("setelah convert\nHasil convert:%s",hasils);
    free(buffer);

    int hasilkirim;

    char dt[100];
    int z;
    for(z=0; z<100; z++){
            //printf("%c,",buffer[awal+z]);
            dt[z] = hasils[z];
        }
        dt[100]='\0';

    if(openstatus == 1)
    {
        br = TRUE;
        char command[100];
        fflush(stdin);
        sprintf(command,"ektpupdateinit#%d#%s#;",fileLen,dt);
        //int result = sendCommandOnly(command);
        resultKirim = 88;
        nunggurespon = 0;
        sendCommandReceive(command);
        int cnt2 = 0;
        int tt2 = 20;
        while(resultKirim == 88 && cnt2 < tt2){
            Sleep(100);
            cnt2++;
        }

        int timeout = 2;
        if(resultKirim == 0){
            br = FALSE;
            Sleep(500);
            int cnt = 0;
            int tt = timeout * 10;

            while(resupdateinit == 0 && cnt < tt){
                Sleep(100);
                cnt++;
            }
            nunggurespon = 1;

            if(resupdateinit == 0){
                hasil = -1007;
                strcpy(error,"Receive Data Timeout");
            }
            else{

                if(strcmp(isiUpdateInit,"error") == 0){
                    hasil = -1;
                    strcpy(error,"ERR_ERROR");
                }
                else if(strcmp(isiUpdateInit,"0") == 0){
                    hasilkirim = kirimFile2(fileLen,hasils);
                    if(hasilkirim == 0){
                        hasil = hasilkirim;
                        strcpy(error,"ERR_OK");
                        nunggurespon = 1;
                        CloseHandle(hComm);
                        openstatus = 0;
                    }
                    else{
                        hasil = hasilkirim;
                        strcpy(error,"Cannot send update file");
                    }
                }
                else if(strcmp(isiUpdateInit,"2") == 0){
                    hasil = -1018;
                    strcpy(error,"Invalid file update version");
                }
                else if(strcmp(isiUpdateInit,"3") == 0){
                    hasil = -1017;
                    strcpy(error,"Invalid file update type");
                }
                else{
                    hasil = -1;
                    strcpy(error,"Cannot update reader");
                    //printf("isi:%s",isiUpdateInit);
                }

                resupdateinit = 0;
            }
        }
        else if(resultKirim == 88){
            hasil = -1005;
            strcpy(error,"Send Data Timeout");
            cAOReaderAgain();
        }
        else{
            hasil = -1006;
            strcpy(error,"Send Data Failed");
            cAOReaderAgain();
        }
    }
    else{
        hasil = -1012;
        strcpy(error,"Device Not Open");
    }

    return hasil;
}

//__declspec(dllexport) int ektp_resetVar(char error[100])
//{
//    free(isiBuffer);
//    free(isiInfo);
//    free(isiClose);
//    free(isiPoll);
//    free(isiPut);
//    free(isiReset);
//    free(isiPut);
//    free(isiFinger);
//    free(isiLog);
//    free(isiDemog);
//    free(isiSwitch);
//    free(isiDisplay);
//    free(isiSetpow);
//    free(isiRespow);
//    free(isiUpdateInit);
//    free(isiUpdateFile);
//    free(isiUpdateFinish);
////    int tid = 1;
////    int nunggurespon = 0, resput = 0, resinfo = 0, resclose = 0, respoll = 0, resreset = 0, resfinger = 0;
////    int reslog = 0, resdemog = 0, resswitch = 0, resdisplay = 0, ressetpow = 0, resrespow = 0;
////    int resupdateinit = 0, resupdatefile = 0, resupdatefinish = 0;
////    int openstatus = 0;
//    CloseHandle(hComm);
//    //BOOL br = FALSE;
//    return 0;
//}

//__declspec(dllexport) int ektp_pollTanpaRespon()
//{
//    if(openstatus == 1)
//    {
//        br = TRUE;
//        int result = sendCommandOnly("ektppoll#;");
//        //return 0;
//    }
//}


