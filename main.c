#include <windows.h>
#include <utility.h>
#include <ansi_c.h>
#include <cvirte.h>
#include <userint.h>
#include <stdio.h>
#include <string.h>
#include "synchro.h"
#include "nai_8810a.h"
#include <rs232.h>
#include "API8810ADll.h"

static NAI8810A gApi;

static int gComPort = 4;      /* COM4 */
static int gConnected = 0;

/* API-8810A state */
static int gApiConnected = 0;
static int gApiNo = 1;        /* vendor sample uses API number 1 */
static int gUsbDeviceNo = -1;

#define DPE_BAUD 38400

/* Prevent Apply-on-change callbacks from firing during connect/UI updates */
static int gSuppressAutoApply = 1;

static int gLiveAngleOn = 0;


/* ------------------------------------------------------------- */
/* API-8810A RUNTIME LOADING                                     */
/* ------------------------------------------------------------- */

typedef int (__cdecl *PFN_API8810A_GetUSBDeviceCnt)(int *cnt);
typedef int (__cdecl *PFN_API8810A_GetDeviceIDN)(int devNo, char *idn);
typedef int (__cdecl *PFN_API8810A_ConnectViaUSB)(int apiNo, int usbDevNo);
typedef int (__cdecl *PFN_API8810A_SetRemoteUSB)(int apiNo);
typedef int (__cdecl *PFN_API8810A_GetAPIAngle)(int apiNo, int ch, double *ang);
typedef int (__cdecl *PFN_API8810A_DisconnectUSB)(int apiNo);
typedef int (__cdecl *PFN_API8810A_GetAPISignalMode)(int apiNo, int ch, int *mode);

static HMODULE gApiDll = NULL;
static PFN_API8810A_GetUSBDeviceCnt   pAPI8810A_GetUSBDeviceCnt = NULL;
static PFN_API8810A_GetDeviceIDN      pAPI8810A_GetDeviceIDN = NULL;
static PFN_API8810A_ConnectViaUSB     pAPI8810A_ConnectViaUSB = NULL;
static PFN_API8810A_SetRemoteUSB      pAPI8810A_SetRemoteUSB = NULL;
static PFN_API8810A_GetAPIAngle       pAPI8810A_GetAPIAngle = NULL;
static PFN_API8810A_DisconnectUSB     pAPI8810A_DisconnectUSB = NULL;
static PFN_API8810A_GetAPISignalMode  pAPI8810A_GetAPISignalMode = NULL;

static int LoadApi8810aDll(void)
{
    char exePath[MAX_PATH] = {0};
    char dllPath[MAX_PATH] = {0};
    char msg[1024] = {0};
    char *slash = NULL;
    DWORD gle = 0;

    if (gApiDll)
        return 0;

    GetModuleFileNameA(NULL, exePath, sizeof(exePath));
    strncpy(dllPath, exePath, sizeof(dllPath) - 1);

    slash = strrchr(dllPath, '\\');
    if (slash)
        *(slash + 1) = '\0';
    else
        dllPath[0] = '\0';

    strncat(dllPath, "API8810ADll.dll", sizeof(dllPath) - strlen(dllPath) - 1);

	MessagePopup("DLL Path", dllPath);
    gApiDll = LoadLibraryA(dllPath);
    if (!gApiDll)
    {
        gle = GetLastError();
        snprintf(msg, sizeof(msg),
                 "LoadLibrary failed.\n\nPath:\n%s\n\nGetLastError = %lu",
                 dllPath, (unsigned long)gle);
        MessagePopup("API DLL", msg);
        return -1;
    }

    pAPI8810A_GetUSBDeviceCnt =
        (PFN_API8810A_GetUSBDeviceCnt)GetProcAddress(gApiDll, "API8810A_GetAPI8810AUSBDeviceCnt");
    pAPI8810A_GetDeviceIDN =
        (PFN_API8810A_GetDeviceIDN)GetProcAddress(gApiDll, "API8810A_GetAPI8810ADeviceIDN");
    pAPI8810A_ConnectViaUSB =
        (PFN_API8810A_ConnectViaUSB)GetProcAddress(gApiDll, "API8810A_ConnectViaUSB");
    pAPI8810A_SetRemoteUSB =
        (PFN_API8810A_SetRemoteUSB)GetProcAddress(gApiDll, "API8810A_SetRemoteUSB");
    pAPI8810A_GetAPIAngle =
        (PFN_API8810A_GetAPIAngle)GetProcAddress(gApiDll, "API8810A_GetAPIAngle");
    pAPI8810A_DisconnectUSB =
        (PFN_API8810A_DisconnectUSB)GetProcAddress(gApiDll, "API8810A_DisconnectUSB");
    pAPI8810A_GetAPISignalMode =
        (PFN_API8810A_GetAPISignalMode)GetProcAddress(gApiDll, "API8810A_GetAPISignalMode");

    if (!pAPI8810A_GetUSBDeviceCnt ||
        !pAPI8810A_GetDeviceIDN ||
        !pAPI8810A_ConnectViaUSB ||
        !pAPI8810A_SetRemoteUSB ||
        !pAPI8810A_GetAPIAngle ||
        !pAPI8810A_DisconnectUSB ||
        !pAPI8810A_GetAPISignalMode)
    {
        MessagePopup("API DLL", "DLL loaded, but one or more exports were not found.");
        FreeLibrary(gApiDll);
        gApiDll = NULL;
        return -2;
    }

    return 0;
}


/* ------------------------------------------------------------- */
/* DPE / STEPPER HELPERS                                         */
/* ------------------------------------------------------------- */

static void SendDPE(const char *cmd)
{
    char buf[256];

    if (!gConnected) {
        MessagePopup("Not connected", "Connect first.");
        return;
    }

    snprintf(buf, sizeof(buf), "%s\r", cmd);
    ComWrt(gComPort, buf, (int)strlen(buf));
}

static int ReadDPEReply(char *out, int outSize, double timeoutSec)
{
    double t0 = Timer();
    int n = 0;

    if (!out || outSize <= 0)
        return 0;

    out[0] = 0;

    while ((Timer() - t0) < timeoutSec)
    {
        n = ComRd(gComPort, out, outSize - 1);
        if (n > 0)
        {
            out[n] = 0;
            return n;
        }

        Delay(0.01);
    }

    return 0;
}

static int ReadWithTimeout(char *out, int outSize, double timeoutSec)
{
    double t0 = Timer();
    int n = 0;

    if (!out || outSize <= 0)
        return 0;

    out[0] = 0;

    while ((Timer() - t0) < timeoutSec)
    {
        n = ComRd(gComPort, out, outSize - 1);
        if (n > 0)
        {
            out[n] = 0;
            return n;
        }
        Delay(0.01);
    }
    return 0;
}

int DPE_ReadError(int unit, char *outReply, int outReplySize)
{
    char rx[128] = {0};
    char cmd[16];

    if (!gConnected)
        return -1;

    snprintf(cmd, sizeof(cmd), "@%d", unit);
    SendDPE(cmd);

    FlushInQ(gComPort);
    SendDPE("!");
    Delay(0.05);

    if (ReadWithTimeout(rx, sizeof(rx), 0.5) <= 0)
    {
        if (outReply && outReplySize > 0)
        {
            strncpy(outReply, "<no reply>", outReplySize - 1);
            outReply[outReplySize - 1] = 0;
        }
        return -2;
    }

    if (outReply && outReplySize > 0)
    {
        strncpy(outReply, rx, outReplySize - 1);
        outReply[outReplySize - 1] = 0;
    }

    return atoi(rx);
}


/* ------------------------------------------------------------- */
/* API-8810A HELPERS                                             */
/* ------------------------------------------------------------- */

static void StripCRLF(char *s)
{
    int i;

    if (!s)
        return;

    for (i = 0; s[i] != '\0'; i++)
    {
        if (s[i] == '\r' || s[i] == '\n')
        {
            s[i] = '\0';
            break;
        }
    }
}

static int ApiFindAndConnectUsb(int panel)
{
    int nStatus;
    int nUSBDeviceCnt = 0;
    int n8810ADeviceCnt = 0;
    int i;
    int aUSB_8810A_Endpoints[MAX_API];
    char sz8810AIDN[100] = {0};
    char statusMsg[256] = {0};

    nStatus = LoadApi8810aDll();
    if (nStatus != 0)
        return -100;

    for (i = 0; i < MAX_API; i++)
        aUSB_8810A_Endpoints[i] = -1;

    SetCtrlVal(panel, TABPANEL_PANEL_LED_API_2, 0);
    SetCtrlVal(panel, TABPANEL_PANEL_STR_API_2, "Scanning USB devices...");

    nStatus = pAPI8810A_GetUSBDeviceCnt(&nUSBDeviceCnt);
    if (nStatus != API_SUCCESS)
    {
        snprintf(statusMsg, sizeof(statusMsg),
                 "GetAPI8810AUSBDeviceCnt failed: %d", nStatus);
        SetCtrlVal(panel, TABPANEL_PANEL_STR_API_2, statusMsg);
        return -1;
    }

    if (nUSBDeviceCnt <= 0)
    {
        SetCtrlVal(panel, TABPANEL_PANEL_STR_API_2, "No Cypress USB devices found");
        return -2;
    }

    n8810ADeviceCnt = 0;
    gUsbDeviceNo = -1;

    for (i = 0; i < nUSBDeviceCnt; i++)
    {
        memset(sz8810AIDN, 0, sizeof(sz8810AIDN));

        nStatus = pAPI8810A_GetDeviceIDN(i, sz8810AIDN);
        if (nStatus == API_SUCCESS)
        {
            StripCRLF(sz8810AIDN);
            aUSB_8810A_Endpoints[i] = i;
            n8810ADeviceCnt++;

            if (gUsbDeviceNo < 0)
                gUsbDeviceNo = i;
        }
    }

    if (n8810ADeviceCnt <= 0 || gUsbDeviceNo < 0)
    {
        SetCtrlVal(panel, TABPANEL_PANEL_STR_API_2, "No API-8810A device found");
        return -3;
    }

    memset(sz8810AIDN, 0, sizeof(sz8810AIDN));
    pAPI8810A_GetDeviceIDN(gUsbDeviceNo, sz8810AIDN);
    StripCRLF(sz8810AIDN);

    nStatus = pAPI8810A_ConnectViaUSB(gApiNo, gUsbDeviceNo);
    if (nStatus != API_SUCCESS)
    {
        snprintf(statusMsg, sizeof(statusMsg),
                 "ConnectViaUSB failed: %d", nStatus);
        SetCtrlVal(panel, TABPANEL_PANEL_STR_API_2, statusMsg);
        return -4;
    }

    nStatus = pAPI8810A_SetRemoteUSB(gApiNo);
    if (nStatus != API_SUCCESS)
    {
        snprintf(statusMsg, sizeof(statusMsg),
                 "SetRemoteUSB failed: %d", nStatus);
        SetCtrlVal(panel, TABPANEL_PANEL_STR_API_2, statusMsg);
        pAPI8810A_DisconnectUSB(gApiNo);
        return -5;
    }

    gApiConnected = 1;
    SetCtrlVal(panel, TABPANEL_PANEL_LED_API_2, 1);

    if (sz8810AIDN[0] != '\0')
        SetCtrlVal(panel, TABPANEL_PANEL_STR_API_2, sz8810AIDN);
    else
        SetCtrlVal(panel, TABPANEL_PANEL_STR_API_2, "Connected to API-8810A via USB");

    return 0;
}


/* ------------------------------------------------------------- */
/* MAIN                                                          */
/* ------------------------------------------------------------- */

int main(int argc, char* argv[])
{
    int panel;

    if (InitCVIRTE(0, argv, 0) == 0)
        return -1;

    NAI_Init(&gApi);

    panel = LoadPanel(0, "synchro.uir", PANEL);
    if (panel < 0)
    {
        CloseCVIRTE();
        return -1;
    }
	

    DisplayPanel(panel);
    RunUserInterface();

    DiscardPanel(panel);

    if (gApiDll)
    {
        FreeLibrary(gApiDll);
        gApiDll = NULL;
    }

    CloseCVIRTE();
    return 0;
}


/* ------------------------------------------------------------- */
/* CVI CALLBACKS                                                 */
/* ------------------------------------------------------------- */

int CVICALLBACK ConnectCB(int panel, int control, int event,
                          void *callbackData, int eventData1, int eventData2)
{
    int err;
    char status[128];

    if (event != EVENT_COMMIT)
        return 0;

    gSuppressAutoApply = 1;

    err = OpenComConfig(gComPort, "", DPE_BAUD, 0, 8, 1, 512, 512);
    if (err < 0)
    {
        MessagePopup("Connect Failed", "Could not open COM port. Wrong COM# or port already in use.");
        return err;
    }

    SetXMode(gComPort, 1);
    FlushInQ(gComPort);

    gConnected = 1;

    SetCtrlVal(panel, TABPANEL_PANEL_LED_STEPPER, 1);

    sprintf(status, "Connected (COM%d, %d)", gComPort, DPE_BAUD);
    SetCtrlVal(panel, TABPANEL_PANEL_STR_STEPPER, status);

    SetCtrlAttribute(panel, TABPANEL_Conn_COMMANDBUTTON,    ATTR_DIMMED, 1);
    SetCtrlAttribute(panel, TABPANEL_Conn_DISCONNECTBUTTON, ATTR_DIMMED, 0);

    Delay(0.10);
    FlushInQ(gComPort);

    gSuppressAutoApply = 0;

    MessagePopup("Connect", "COM port opened successfully!");
    return 0;
}

int CVICALLBACK SendOneCommandCB(int panel, int control, int event,
                                 void *callbackData, int eventData1, int eventData2)
{
    char cmd[128] = {0};
    char rx[256]  = {0};
    int n = 0;

    if (event != EVENT_COMMIT)
        return 0;

    if (!gConnected)
    {
        MessagePopup("Not connected", "Connect first.");
        return 0;
    }

    GetCtrlVal(panel, TABPANEL_2_TABPANEL_2_TX_CMD, cmd);

    if (cmd[0] == 0)
    {
        MessagePopup("Send", "Type a command first (e.g., @0$).");
        return 0;
    }

    FlushInQ(gComPort);
    SendDPE(cmd);

    n = ReadDPEReply(rx, sizeof(rx), 0.5);

    if (n > 0)
        SetCtrlVal(panel, TABPANEL_2_TABPANEL_2_RX_REPLY, rx);
    else
        SetCtrlVal(panel, TABPANEL_2_TABPANEL_2_RX_REPLY, "<no reply>");

    return 0;
}

int CVICALLBACK MoveToggleCB(int panel, int control, int event,
                             void *callbackData, int eventData1, int eventData2)
{
    int moveOn = 0;

    if (event != EVENT_COMMIT)
        return 0;

    if (!gConnected)
    {
        MessagePopup("Not connected", "Connect first.");
        return 0;
    }

    GetCtrlVal(panel, TABPANEL_2_TEST_MOTION_BUTT, &moveOn);

    if (moveOn)
    {
        SendDPE("@0T1");
        Delay(0.10);

        SendDPE("@0-");
        Delay(0.10);

        SendDPE("@0S");
        Delay(5.0);

        SendDPE("@0,");
        Delay(0.10);

        SendDPE("@0T0");
    }
    else
    {
        SendDPE(",");
        SendDPE("T0");
    }

    return 0;
}

int CVICALLBACK ReadErrorButtonCB(int panel, int control, int event,
                                  void *callbackData, int eventData1, int eventData2)
{
    char reply[64];
    int code;

    if (event != EVENT_COMMIT)
        return 0;

    code = DPE_ReadError(0, reply, sizeof(reply));
    if (code >= 0)
        MessagePopup("DPE Error", reply);
    else
        MessagePopup("DPE Error", "Failed to read error.");

    return 0;
}

int CVICALLBACK ApiConnectCB(int panel, int control, int event,
                             void *callbackData, int eventData1, int eventData2)
{
    int st;
    int nSigMode = 0;

    if (event != EVENT_COMMIT)
        return 0;

    SetCtrlAttribute(panel, TABPANEL_CTRL_API_IP,   ATTR_DIMMED, 1);
    SetCtrlAttribute(panel, TABPANEL_CTRL_API_PORT, ATTR_DIMMED, 1);

    st = ApiFindAndConnectUsb(panel);
    if (st != 0)
    {
        MessagePopup("API Connect", "Failed to find/connect API-8810A over USB.");
        return 0;
    }

    st = pAPI8810A_GetAPISignalMode(gApiNo, 1, &nSigMode);
    if (st == API_SUCCESS)
    {
        if (nSigMode == RESOLVER)
            MessagePopup("API Mode", "Channel 1 Signal Mode = RESOLVER");
        else if (nSigMode == SYNCHRO)
            MessagePopup("API Mode", "Channel 1 Signal Mode = SYNCHRO");
        else
            MessagePopup("API Mode", "Channel 1 Signal Mode = UNKNOWN");
    }

    MessagePopup("API Connect", "API-8810A connected successfully.");
    return 0;
}



int CVICALLBACK ApiDisconnectCB(int panel, int control, int event,
                                void *callbackData, int eventData1, int eventData2)
{
    int st;

    if (event != EVENT_COMMIT)
        return 0;

    if (!gApiConnected)
        return 0;

    st = pAPI8810A_DisconnectUSB(gApiNo);

    gApiConnected = 0;
    gUsbDeviceNo = -1;

    SetCtrlVal(panel, TABPANEL_PANEL_LED_API_2, 0);

    if (st == API_SUCCESS)
        SetCtrlVal(panel, TABPANEL_PANEL_STR_API_2, "API disconnected");
    else
        SetCtrlVal(panel, TABPANEL_PANEL_STR_API_2, "DisconnectUSB returned error");

    return 0;
}

int CVICALLBACK LiveAngleToggleCB(int panel, int control, int event,
                                  void *callbackData, int eventData1, int eventData2)
{
    int on = 0;

    if (event != EVENT_COMMIT)
        return 0;

    GetCtrlVal(panel, control, &on);

    if (!gApiConnected && on)
    {
        MessagePopup("Live Angle", "API not connected.");
        SetCtrlVal(panel, control, 0);
        return 0;
    }

    gLiveAngleOn = on;
    SetCtrlAttribute(panel,TABPANEL_CTRL_ANGLE_NUM, ATTR_ENABLED, on);

    return 0;
}

int CVICALLBACK AngleTimerCB(int panel, int control, int event,
                             void *callbackData, int eventData1, int eventData2)
{
    double ang = 0.0;
    int st;

    if (event != EVENT_TIMER_TICK)
        return 0;

    if (!gApiConnected || !gLiveAngleOn)
        return 0;

    st = pAPI8810A_GetAPIAngle(gApiNo, 1, &ang);
    if (st == API_SUCCESS)
        SetCtrlVal(panel, TABPANEL_CTRL_ANGLE_NUM, ang);

    return 0;
}

int CVICALLBACK ReadAngleOnceCB (int panel, int control, int event,
                                 void *callbackData, int eventData1, int eventData2)
{
    double ang = 0.0;
    int st;
    char msg[128];

    if (event != EVENT_COMMIT)
        return 0;

    if (!gApiConnected)
    {
        MessagePopup("Angle", "API not connected.");
        return 0;
    }

    if (!pAPI8810A_GetAPIAngle)
    {
        MessagePopup("Angle", "Angle function not loaded.");
        return 0;
    }

    st = pAPI8810A_GetAPIAngle(gApiNo, 1, &ang);
    if (st != API_SUCCESS)
    {
        snprintf(msg, sizeof(msg), "GetAPIAngle failed: %d", st);
        MessagePopup("Angle", msg);
        return 0;
    }

    SetCtrlVal(panel, TABPANEL_CTRL_ANGLE_NUM, ang);
    return 0;
}