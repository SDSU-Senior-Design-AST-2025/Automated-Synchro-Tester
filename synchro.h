/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1
#define  PANEL_TEXTMSG                    2       /* control type: textMsg, callback function: (none) */
#define  PANEL_NUMERIC_4                  3       /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_3                  4       /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_2                  5       /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC                    6       /* control type: numeric, callback function: (none) */
#define  PANEL_COMMANDBUTTON_4            7       /* control type: command, callback function: (none) */
#define  PANEL_COMMANDBUTTON_3            8       /* control type: command, callback function: OnSaveCSV */
#define  PANEL_COMMANDBUTTON_2            9       /* control type: command, callback function: OnStop */
#define  PANEL_COMMANDBUTTON              10      /* control type: command, callback function: OnStartTest */
#define  PANEL_TEXTMSG_2                  11      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_3                  12      /* control type: textMsg, callback function: (none) */
#define  PANEL_LED                        13      /* control type: LED, callback function: (none) */
#define  PANEL_GRAPH                	  14      /* control type: graph, callback function: (none) */
#define  PANEL_TEXTMSG_4                  15      /* control type: textMsg, callback function: (none) */
#define  PANEL_TABLE                      16      /* control type: table, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnSaveCSV(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnStartTest(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnStop(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif