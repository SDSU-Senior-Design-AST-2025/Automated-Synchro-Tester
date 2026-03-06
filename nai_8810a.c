#include <ansi_c.h>
#include <tcpsupp.h>
#include "nai_8810a.h"

static char g_lastRx[512]  = "";
static char g_lastErr[256] = "";

static void set_err(const char* msg)
{
    strncpy(g_lastErr, msg ? msg : "", sizeof(g_lastErr)-1);
    g_lastErr[sizeof(g_lastErr)-1] = '\0';
}

static void trim(char* s)
{
    int n;
    if (!s) return;
    n = (int)strlen(s);
    while (n > 0 && (s[n-1]=='\r' || s[n-1]=='\n' || s[n-1]==' ' || s[n-1]=='\t'))
        s[--n] = '\0';
}

static int read_line(unsigned int h, char* buf, int n, int timeoutMs)
{
    int total = 0, r;
    char c;

    if (!buf || n < 2) return -1;
    buf[0] = 0;

    while (total < n-1)
    {
        r = ClientTCPRead(h, &c, 1, timeoutMs);
        if (r < 0) return r;
        if (r == 0) break;

        buf[total++] = c;
        buf[total] = 0;

        if (c=='\n' || c=='\r')
            break;
    }
    trim(buf);
    return total;
}

static int send_cmd(unsigned int h, const char* cmdNoCRLF, int timeoutMs)
{
    char out[512];
    int w;
    snprintf(out, sizeof(out), "%s\r\n", cmdNoCRLF);
    w = ClientTCPWrite(h, out, (int)strlen(out), timeoutMs);
    return (w < 0) ? w : 0;
}

void NAI_Init(NAI8810A* d)
{
    memset(d, 0, sizeof(*d));
    d->ioTimeoutMs = 2000;
    g_lastRx[0] = 0;
    g_lastErr[0] = 0;
}

int NAI_Connect(NAI8810A* d, const char* ip, int port)
{
    int err;
    char tmp[256];

    if (!d || !ip) return -1;
    if (d->connected) return 0;

    // NOTE: 6 args in CVI
    err = ConnectToTCPServer(&d->h, port, (char*)ip, 3000, NULL, NULL);
    if (err < 0)
    {
        d->h = 0;
        d->connected = 0;
        set_err("ConnectToTCPServer failed");
        return err;
    }

    d->connected = 1;

    // Login required
    err = NAI_Login(d);
    if (err < 0)
    {
        set_err("NAII login failed");
        NAI_Disconnect(d);
        return err;
    }

    // Optional immediate response
    if (read_line(d->h, tmp, sizeof(tmp), 250) > 0)
        strncpy(g_lastRx, tmp, sizeof(g_lastRx)-1);

    set_err("");
    return 0;
}

void NAI_Disconnect(NAI8810A* d)
{
    if (!d) return;

    if (d->connected && d->h)
        DisconnectFromTCPServer(d->h);

    d->h = 0;
    d->connected = 0;
}

int NAI_Login(NAI8810A* d)
{
    if (!d || !d->connected) return -1;
    return send_cmd(d->h, "NAII", d->ioTimeoutMs);
}

int NAI_Query(NAI8810A* d, const char* cmdNoCRLF, char* reply, int replyMax)
{
    int err;

    if (!d || !d->connected) { set_err("Not connected"); return -1; }
    if (!reply || replyMax < 2) return -2;

    err = send_cmd(d->h, cmdNoCRLF, d->ioTimeoutMs);
    if (err < 0) { set_err("ClientTCPWrite failed"); return err; }

    err = read_line(d->h, reply, replyMax, d->ioTimeoutMs);
    if (err < 0) { set_err("ClientTCPRead failed"); return err; }

    strncpy(g_lastRx, reply, sizeof(g_lastRx)-1);
    g_lastRx[sizeof(g_lastRx)-1] = 0;
    trim(reply);

    set_err("");
    return 0;
}

int NAI_ReadAngleCh1Deg(NAI8810A* d, double* deg)
{
    char reply[256];
    char* p;

    if (!d || !deg) return -1;

    // Try both command formats (space/no space)
    if (NAI_Query(d, "API1 ANGLE?", reply, sizeof(reply)) < 0)
        if (NAI_Query(d, "API1ANGLE?", reply, sizeof(reply)) < 0)
            return -2;

    p = strpbrk(reply, "0123456789-+");
    if (!p) return -3;

    *deg = atof(p);
    while (*deg < 0) *deg += 360.0;
    while (*deg >= 360.0) *deg -= 360.0;

    return 0;
}

const char* NAI_LastRx(void)  { return g_lastRx; }
const char* NAI_LastErr(void) { return g_lastErr; }