#ifndef NAI_8810A_H
#define NAI_8810A_H

typedef struct {
    unsigned int h;     // TCP handle (CVI)
    int connected;
    int ioTimeoutMs;
} NAI8810A;

void NAI_Init(NAI8810A* d);
int  NAI_Connect(NAI8810A* d, const char* ip, int port);
void NAI_Disconnect(NAI8810A* d);

int  NAI_Login(NAI8810A* d);
int  NAI_Query(NAI8810A* d, const char* cmdNoCRLF, char* reply, int replyMax);
int NAI_ReadAngleCh1Deg(NAI8810A* d, double* deg);

const char* NAI_LastRx(void);
const char* NAI_LastErr(void);

#endif