
#ifndef _API8810ADLL_H
#define _API8810ADLL_H

#include "API8810ADll_BuildOption.h"

#ifdef __WIN32__
  #include <windows.h>
  #include <Winnt.h>
  #include <string.h>
#endif

#ifdef _BUILD_API8810ADLL
   #define _API8810ACLASS __declspec(dllexport )
   #define _API8810AFUNC  __declspec(dllexport )
#else
   #ifdef __WIN32__
      #define _API8810AFUNC __declspec(dllimport )
   #else
      #define _API8810AFUNC
  #endif
#endif

#ifdef __cplusplus
extern "C" {
#else
   #define bool BOOL
#endif 

enum API_STATUS
{
  API_SUCCESS,
  API_ERROR_OPEN_API_SESSION,
  API_ERROR_APINO,
  API_ERROR_ADDRS,
  API_ERROR_LANG,
  API_ERROR_DATA,
  API_ERROR_RANGE_SET,
  API_ERROR_RANGE_GET,
  API_ERROR_WRITE,
  API_ERROR_USB_CONNECTION,
  API_ERROR_ETHER_CONNECTION,
  API_ERROR_FUNC_NOT_SUPPORTED,
  API_STATUS_LAST
};

/* Version information is built into the library. */
const double API8810ADll_VERSION  = 3.1;

/* Maximum number of APIs Driver can communicate with */
#define  MAX_API 12

/* API Language Types */
#define API8810A_NATIVE          0
#define IEEE_API8810_NATIVE      1
#define IEEE_API8810_SR103       2
#define IEEE_API8810_HSR202      3
#define IEEE_API8810_HSR203      4
#define IEEE_API8810_MATECIIL    5
#define IEEE_API8810_FX2         6

/* API Communication Type */
#define NO_CONNECTION            0
#define IEEE_CONNECTION          1
#define USB_CONNECTION           2
#define ETHERNET_CONNECTION      3

/* Mode Type */
#define RESOLVER                 0
#define SYNCHRO                  1

/* Reference Source Type */
#define INTERNAL                 0
#define EXTERNAL                 1

/* Internal Reference Output State Type */
#define INT_REF_OUT_NOT_AVAILABLE   0
#define INT_REF_OUT_AVAILABLE       1

/* Internal Reference Remote Sense Type */
#define INT_REF_REMOTE_SENSE_OFF    0
#define INT_REF_REMOTE_SENSE_ON     1

/* Internal Reference Sense Direction */
#define INT_REF_SENSE_DIR_FRONT     0
#define INT_REF_SENSE_DIR_BACK      1

/* Angle Display Format Type */
#define ANGLE_FMT_360               0
#define ANGLE_FMT_180               1
#define ANGLE_FMT_MIN               2

/* Angle Limit Format Type */
#define ANGLE_LIMIT_ABS_ANG         0
#define ANGLE_LIMIT_ANG_ERR         1

/* DA Format Type */
#define DA_ANGLE_OUTPUT             0
#define DA_VELOCITY_OUTPUT          1

/* Channel 1 Input Type */
#define CH1_INPUT_FRONT_CONNECTOR   0
#define CH1_INPUT_BACK_CONNECTOR    1

/* Touchscreen Enable State Type */
#define TOUCHSCREEN_DISABLED        0
#define TOUCHSCREEN_ENABLED         1

/* Angle Difference Display State Type */
#define ANG_DIFF_DISABLED           0
#define ANG_DIFF_ENABLED            1

/* Periodic Calibration State Type */
#define PERIODIC_CAL_DISABLED           0
#define PERIODIC_CAL_ENABLED            1

/* Buffer Data Type */
#define BUF_ANGLE                   0
#define BUF_ANGERR                  1
#define BUF_VELOCITY                2

/* Channel Plot Type */
#define PLOT_BOTH_CHAN              0
#define PLOT_CHAN1                  1
#define PLOT_CHAN2                  2

/* Sample Rate Unit Type */
#define SAMPLE_RATE_MSEC            0
#define SAMPLE_RATE_SEC             1
#define SAMPLE_RATE_MIN             2

/* Buffer Recording Command Type */
#define BUFFER_REC_STOP             0
#define BUFFER_REC_START            1
#define BUFFER_REC_CLEAR            2

/* Display Command Type */
#define CHAN1_DISPLAY               0
#define CHAN2_DISPLAY               1
#define CHAN1_ANALOG_DISPLAY        2
#define CHAN2_ANALOG_DISPLAY        3
#define DUAL_DISPLAY                4
#define REFERENCE_DISPLAY           5
#define CHARTING_DISPLAY            6
#define UNKNOWN_DISPLAY            -1


/**************************************************
/* API-8810A Connect/Disconnect Routines
/*************************************************/
_API8810AFUNC int API8810A_ConnectViaIEEE(int apiNo, int nIEEEAddr, int nIEEELanguage);
_API8810AFUNC int API8810A_ConnectViaUSB(int apiNo, int nDeviceNo);
_API8810AFUNC int API8810A_ConnectViaEthernet(int apiNo, char *szIPAddr, int nPort);

_API8810AFUNC int API8810A_DisconnectIEEE(int apiNo);
_API8810AFUNC int API8810A_DisconnectUSB(int apiNo);
_API8810AFUNC int API8810A_DisconnectEthernet(int apiNo);

_API8810AFUNC int API8810A_GetAPI8810AUSBDeviceCnt(int *pnUSBDeviceCnt);
_API8810AFUNC int API8810A_GetAPI8810ADeviceIDN(int nDeviceNo, char *pszIDN);

/**************************************************
/* API-8810A Channel Routines
/*************************************************/
_API8810AFUNC int API8810A_SetAPITrackHold(int apiNo, int nChanNo, bool bHold);
_API8810AFUNC int API8810A_GetAPITrackHold(int apiNo, int nChanNo, bool *pbHold);

_API8810AFUNC int API8810A_SetAPISignalMode(int apiNo, int nChanNo, int nSigMode);
_API8810AFUNC int API8810A_GetAPISignalMode(int apiNo, int nChanNo, int *pnSigMode);

_API8810AFUNC int API8810A_SetAPIReferenceSrc(int apiNo, int nChanNo, int nRefSrc);
_API8810AFUNC int API8810A_GetAPIReferenceSrc(int apiNo, int nChanNo, int *pnRefSrc);

_API8810AFUNC int API8810A_SetAPIRatio(int apiNo, int nChanNo, int nRatio);
_API8810AFUNC int API8810A_GetAPIRatio(int apiNo, int nChanNo, int *pnRatio);

_API8810AFUNC int API8810A_SetAPIAutoBandwidth(int apiNo, int nChanNo);
_API8810AFUNC int API8810A_SetAPIBandwidth(int apiNo, int nChanNo, int nBandwidth);
_API8810AFUNC int API8810A_GetAPIBandwidth(int apiNo, int nChanNo, bool *pbAutoBW, int *pnBandwidth);

_API8810AFUNC int API8810A_GetAPIAngle(int apiNo, int nChanNo, double *pdAngle);
_API8810AFUNC int API8810A_GetAPIAvgAngle(int apiNo, int nChanNo, double *pdAvgAngle);
_API8810AFUNC int API8810A_GetAPIVelocity(int apiNo, int nChanNo, double *pdVelocity);
_API8810AFUNC int API8810A_GetAPILineLineVolt(int apiNo, int nChanNo, double *pdLineLineVolt);
_API8810AFUNC int API8810A_GetAPINullVolt(int apiNo, int nChanNo, double *pdNullVolt);
_API8810AFUNC int API8810A_GetAPIRefVolt(int apiNo, int nChanNo, double *pdRefVolt);
_API8810AFUNC int API8810A_GetAPIRefFreq(int apiNo, int nChanNo, double *pdRefFreq);

_API8810AFUNC int API8810A_SetAPIAvgState(int apiNo, int nChanNo, bool bAvgStateOn);
_API8810AFUNC int API8810A_GetAPIAvgState(int apiNo, int nChanNo, bool *pbAvgStateOn);
_API8810AFUNC int API8810A_SetAPIAvgRate(int apiNo, int nChanNo, int nAvgRate);
_API8810AFUNC int API8810A_GetAPIAvgRate(int apiNo, int nChanNo, int *pnAvgRate);

_API8810AFUNC int API8810A_SetAPIAngLimitState(int apiNo, int nChanNo, bool bAngLmtOn);
_API8810AFUNC int API8810A_GetAPIAngLimitState(int apiNo, int nChanNo, bool *pbAngLmtOn);
_API8810AFUNC int API8810A_SetAPIAngLimitCompare(int apiNo, int nChanNo, int nAngLimitCompare);
_API8810AFUNC int API8810A_GetAPIAngLimitCompare(int apiNo, int nChanNo, char *pszAngLimitCompare);
_API8810AFUNC int API8810A_SetAPIAngUpperLimit(int apiNo, int nChanNo, double dAngUpperLimit);
_API8810AFUNC int API8810A_GetAPIAngUpperLimit(int apiNo, int nChanNo, double *pdAngUpperLimit);
_API8810AFUNC int API8810A_SetAPIAngLowerLimit(int apiNo, int nChanNo, double dAngLowerLimit);
_API8810AFUNC int API8810A_GetAPIAngLowerLimit(int apiNo, int nChanNo, double *pdAngLowerLimit);
_API8810AFUNC int API8810A_SetAPIAngLimitErrorStep(int apiNo, int nChanNo, double dAngLimitErrorStep);
_API8810AFUNC int API8810A_GetAPIAngLimitErrorStep(int apiNo, int nChanNo, double *pdAngLimitErrorStep);

_API8810AFUNC int API8810A_SetAPIDAOutput(int apiNo, int nChanNo, int nDAOutput);
_API8810AFUNC int API8810A_GetAPIDAOutput(int apiNo, int nChanNo, char *pszDAOutput);
_API8810AFUNC int API8810A_SetAPIDAUpperLimit(int apiNo, int nChanNo, double dDAUpperLimit);
_API8810AFUNC int API8810A_GetAPIDAUpperLimit(int apiNo, int nChanNo, double *pdDAUpperLimit);
_API8810AFUNC int API8810A_SetAPIDALowerLimit(int apiNo, int nChanNo, double dDALowerLimit);
_API8810AFUNC int API8810A_GetAPIDALowerLimit(int apiNo, int nChanNo, double *pdDALowerLimit);
_API8810AFUNC int API8810A_SetAPIDAUpperVoltage(int apiNo, int nChanNo, double dDAUpperVoltage);
_API8810AFUNC int API8810A_GetAPIDAUpperVoltage(int apiNo, int nChanNo, double *pdDAUpperVoltage);
_API8810AFUNC int API8810A_SetAPIDALowerVoltage(int apiNo, int nChanNo, double dDALowerVoltage);
_API8810AFUNC int API8810A_GetAPIDALowerVoltage(int apiNo, int nChanNo, double *pdDALowerVoltage);

/**************************************************
/* API-8810A Angle Difference Routines
/*************************************************/
_API8810AFUNC int API8810A_SetAPIDisplayAngDiffState(int apiNo, int nAngDiffState);
_API8810AFUNC int API8810A_GetAPIDisplayAngDiffState(int apiNo, int *pnAngDiffState);
_API8810AFUNC int API8810A_GetAPIAngleDiff(int apiNo, double *pdAngleDiff);

/**************************************************
/* API-8810A Multiple Channel Query Routines
/*************************************************/
_API8810AFUNC int API8810A_GetAPIAngles(int apiNo, double *pdAngle1, double *pdAngle2);
_API8810AFUNC int API8810A_GetAPIAvgStates(int apiNo, bool *pbAvgStateOn1, int *pnAvgRate1, bool *pbAvgStateOn2, int *pnAvgRate2);
_API8810AFUNC int API8810A_GetAPIAvgAngles(int apiNo, double *pdAvgAngle1, double *pdAvgAngle2);
_API8810AFUNC int API8810A_GetAPIBandwidths(int apiNo, bool *pbAutoBW1, int *pnBandwidth1, bool *pbAutoBW2, int *pnBandwidth2);
_API8810AFUNC int API8810A_GetAPISignalModes(int apiNo, int *pnSigMode1, int *pnSigMode2);
_API8810AFUNC int API8810A_GetAPIRatios(int apiNo, int *pnRatio1, int *pnRatio2);
_API8810AFUNC int API8810A_GetAPIReferenceSrcs(int apiNo, int *pnRefSrc1, int *pnRefSrc2);
_API8810AFUNC int API8810A_GetAPITrackHolds(int apiNo, bool *pbHold1, bool *pbHold2);
_API8810AFUNC int API8810A_GetAPIAngLimitStates(int apiNo, bool *pbAngLmtOn1, int *pnAngLimitCmp1, bool *pbAngLmtOn2, int *pnAngLimitCmp2);
_API8810AFUNC int API8810A_GetAPIVelocities(int apiNo, double *pdVelocity1, double *pdVelocity2);
_API8810AFUNC int API8810A_GetAPILineLineVolts(int apiNo, double *pdLineLineVolt1, double *pdLineLineVolt2);
_API8810AFUNC int API8810A_GetAPINullVolts(int apiNo, double *pdNullVolt1, double *pdNullVolt2);
_API8810AFUNC int API8810A_GetAPIRefVolts(int apiNo, double *pdRefVolt1, double *pdRefVolt2);
_API8810AFUNC int API8810A_GetAPIRefFreqs(int apiNo, double *pdRefFreq1, double *pdRefFreq2);

/**************************************************
/* API-8810A Internal Reference Routines
/*************************************************/
_API8810AFUNC int API8810A_SetIntRefFreq(int apiNo, double dFreq);
_API8810AFUNC int API8810A_GetIntRefFreq(int apiNo, double *pdFreq);

_API8810AFUNC int API8810A_SetIntRefVolt(int apiNo, double dVolt);
_API8810AFUNC int API8810A_GetIntRefVolt(int apiNo, double *pdVolt);

_API8810AFUNC int API8810A_SetIntRefOutputState(int apiNo, int nOutputState);
_API8810AFUNC int API8810A_GetIntRefOutputState(int apiNo, int *pnOutputState);

_API8810AFUNC int API8810A_GetIntRefOverCurrentState(int apiNo, bool *pbIntRefOverCurrent);
_API8810AFUNC int API8810A_ResetIntRefOverCurrent(int apiNo);

_API8810AFUNC int API8810A_SetIntRefRemoteSense(int apiNo, int nRemoteSense);
_API8810AFUNC int API8810A_GetIntRefRemoteSense(int apiNo, int *pnRemoteSense);

_API8810AFUNC int API8810A_SetIntRefSenseDir(int apiNo, int nSenseDir);
_API8810AFUNC int API8810A_GetIntRefSenseDir(int apiNo, int *pnSenseDir);

_API8810AFUNC int API8810A_GetIntRefMeasCur(int nPAVNo, int *pnMeasCur);

/**************************************************
/* API-8810A Command Routines
/*************************************************/
_API8810AFUNC int API8810A_PerformGetID(int apiNo, char *pszID);
_API8810AFUNC int API8810A_Reset(int apiNo, char *pszResults);
_API8810AFUNC int API8810A_GetErrors(int apiNo, char *pszErrors);

/**************************************************
/* API-8810A Configuration Routines
/*************************************************/
_API8810AFUNC int API8810A_GetIEEELang(int apiNo, char *pszIEEELang);
_API8810AFUNC int API8810A_SetIEEELang(int apiNo, int nIEEELang);

_API8810AFUNC int API8810A_GetCommState(int apiNo, char *pszCommState);
_API8810AFUNC int API8810A_GoToLocal(int apiNo);
_API8810AFUNC int API8810A_SetLocalLockout(int apiNo);
_API8810AFUNC int API8810A_SetRemoteUSB(int apiNo);
_API8810AFUNC int API8810A_SetRemoteEthernet(int apiNo);
_API8810AFUNC int API8810A_SetRemoteIEEE(int apiNo);
_API8810AFUNC int API8810A_SetRemoteJ1(int apiNo);

_API8810AFUNC int API8810A_SetAngleDisplayFormat(int apiNo, int nDisplayFormat);
_API8810AFUNC int API8810A_GetAngleDisplayFormat(int apiNo, int *pnDisplayFormat);

_API8810AFUNC int API8810A_SetCh1Input(int apiNo, int nCh1InputConnector);
_API8810AFUNC int API8810A_GetCh1Input(int apiNo, int *pnCh1InputConnector);

_API8810AFUNC int API8810A_SetTouchscreenState(int apiNo, int nTouchscreenState);
_API8810AFUNC int API8810A_GetTouchscreenState(int apiNo, int *pnTouchscreenState);

_API8810AFUNC int API8810A_SetDisplayState(int apiNo, int nDisplayState);
_API8810AFUNC int API8810A_GetDisplayState(int apiNo, int *pnDisplayState);

_API8810AFUNC int API8810A_ResetDefaultValues(int apiNo);


/**************************************************
/* API-8810A Calibration Routines
/*************************************************/
_API8810AFUNC int API8810A_GetCalState(int apiNo, char *pszCalState);
_API8810AFUNC int API8810A_Calibrate(int apiNo);
_API8810AFUNC int API8810A_SetAPIPeriodicCalState(int apiNo, int nPeriodicCalState);
_API8810AFUNC int API8810A_GetAPIPeriodicCalState(int apiNo, int *pnPeriodicCalState);

/**************************************************
/* API-8810A Buffer Routines
/*************************************************/
_API8810AFUNC int API8810A_GetSampleRate(int apiNo, int *pnSampleRate, int *pnSampleRateUnit);
_API8810AFUNC int API8810A_SetSampleRate(int apiNo, int nSampleRate, int nSampleRateUnit);

_API8810AFUNC int API8810A_GetSampleType(int apiNo, char *pszSampleType);
_API8810AFUNC int API8810A_SetSampleType(int apiNo, int nSampleType);

_API8810AFUNC int API8810A_GetPlotChan(int apiNo, char *pszPlotChan);
_API8810AFUNC int API8810A_SetPlotChan(int apiNo, int nPlotChan);

_API8810AFUNC int API8810A_GetAngleErrStep(int apiNo, double *pdAngErrStep);
_API8810AFUNC int API8810A_SetAngleErrStep(int apiNo, double dAngErrStep);

_API8810AFUNC int API8810A_GetLowerRange(int apiNo, double *pdLowerRange);
_API8810AFUNC int API8810A_SetLowerRange(int apiNo, double dLowerRange);

_API8810AFUNC int API8810A_GetUpperRange(int apiNo, double *pdUpperRange);
_API8810AFUNC int API8810A_SetUpperRange(int apiNo, double dUpperRange);

_API8810AFUNC int API8810A_GetRecordingState(int apiNo, char *pszRecState);
_API8810AFUNC int API8810A_SetRecordingState(int apiNo, int nRecState);

_API8810AFUNC int API8810A_GetBufferCnt(int apiNo, int *pnBufCnt);
_API8810AFUNC int API8810A_GetBufferData(int apiNo, int nChannel, int nStartRec, int nEndRec, 
										         char *pszBufData);

/**************************************************
/* API-8810A Miscellaneous Routines
/*************************************************/
_API8810AFUNC int API8810A_MaxRetry(int nMaxRetry);
_API8810AFUNC int API8810A_LastCmdSent(int apiNo, char szLastCommand[]);
_API8810AFUNC int API8810A_WriteCommand(int apiNo, char szCommand[]);
_API8810AFUNC int API8810A_QueryCommand(int apiNo, char szCommand[], char *pszResponse);

/**************************************************
/* Local Routines (not exportable)
/*************************************************/
bool SendIEEEMessage(int apiNoIndex);
bool WaitForResponse(int apiNoIndex);
char ReadIEEESerialPoll(int apiNoIndex);
bool ParseForDataElements(int apiNoIndex); 
bool ParseForChanDataDoubleType(int apiNoIndex, double *pData1, double *pData2);
bool ParseForChanDataIntType(int apiNoIndex, int *pData1, int *pData2);
bool ParseForChanDataModeType(int apiNoIndex, int *pData1, int *pData2);
bool ParseForChanDataTrackHoldType(int apiNoIndex, bool *pData1, bool *pData2);
bool ParseForChanDataRefSrc(int apiNoIndex, int *pData1, int *pData2);
bool ParseForChanDataAvgState(int apiNoIndex,  bool *pbData1, int *pnData1, bool *pbData2, int *pnData2);
bool ParseForChanDataBandwidth(int apiNoIndex,  bool *pbData1, int *pnData1, bool *pbData2, int *pnData2);
bool ParseForChanDataLimitState(int apiNoIndex,  bool *pbData1, int *pnData1, bool *pbData2, int *pnData2);
int  ParseSampleRate(char *pszBuffer, int *pnSampleRate, int *pnSampleRateUnit);

#ifdef __cplusplus
}
#endif

#endif
