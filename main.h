struct apiresult
{
    int status;
    char* error;
    char* result;
};

__declspec(dllexport) struct apiresult ektp_getDLL();
__declspec(dllexport) struct apiresult ektp_getAPI();
__declspec(dllexport) struct apiresult ektp_open(char* port);
__declspec(dllexport) struct apiresult ektp_close(char* port);
__declspec(dllexport) struct apiresult ektp_info(char* port);
__declspec(dllexport) struct apiresult ektp_poll(char* port);
__declspec(dllexport) struct apiresult ektp_reset(char* port);
__declspec(dllexport) struct apiresult ektp_getMachineLog(char* port, char* tanggal);
__declspec(dllexport) struct apiresult ektp_dispMessage(char* port);
__declspec(dllexport) struct apiresult ektp_putKTP(char* port, char* text, int timeout);
__declspec(dllexport) struct apiresult ektp_verifyFinger(char* port, char* text, char* jari, char* idop, char* nikop, int timeout);
__declspec(dllexport) struct apiresult ektp_getDataDemography(char* port, char* idop, char* nikop, int timeout);
__declspec(dllexport) struct apiresult ektp_switchMode(char* port, int mode);
__declspec(dllexport) struct apiresult ektp_setPowText(char* port, char* gambar);
__declspec(dllexport) struct apiresult ektp_resPowText(char* port);
