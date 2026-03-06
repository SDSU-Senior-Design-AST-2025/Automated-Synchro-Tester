#include <ansi_c.h>
#include <cvirte.h>
#include <userint.h>
#include "synchro.h"
#include "nai_8810a.h"
#include <rs232.h>
#include "synchro.h"

static NAI8810A gApi;
static int gComPort = 4;      // com port 4
static int gConnected = 0;


#define DPE_BAUD 38400

int main (int argc, char* argv[])
{
    int panel;

    if (InitCVIRTE(0, argv, 0) == 0)
        return -1;

    NAI_Init(&gApi);

    panel = LoadPanel(0, "synchro.uir", PANEL);


    DisplayPanel(panel);
    RunUserInterface();

    DiscardPanel(panel);
    CloseCVIRTE();
    return 0;
}

int CVICALLBACK ConnectCB (int panel, int control, int event,
                           void *callbackData, int eventData1, int eventData2)
{
    int err;
    char status[128];

    if (event != EVENT_COMMIT)
        return 0;

    // Open COM port at 38400, 8 data bits, no parity, 1 stop bit
    err = OpenComConfig(gComPort, "", DPE_BAUD, 0, 8, 1, 512, 512);
    if (err < 0)
    {
        MessagePopup("Connect Failed", "Could not open COM port. Wrong COM# or port already in use.");
        return err;
    }

    FlushInQ(gComPort);
    FlushOutQ(gComPort);

    gConnected = 1;

    // ---- Update UI to Connected ----
    SetCtrlVal(panel, TABPANEL_PANEL_LED_STEPPER, 1);

    sprintf(status, "Connected (COM%d, %d)", gComPort, DPE_BAUD);
    SetCtrlVal(panel, TABPANEL_PANEL_STR_STEPPER, status);

    // Grey out Connect, enable Disconnect
    SetCtrlAttribute(panel, TABPANEL_Conn_COMMANDBUTTON,    ATTR_DIMMED, 1);
    SetCtrlAttribute(panel, TABPANEL_Conn_DISCONNECTBUTTON, ATTR_DIMMED, 0);
    // --------------------------------

    MessagePopup("Connect", "COM port opened successfully!");
    return 0;
}