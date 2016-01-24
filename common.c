#include "common.h"

#include <stdio.h>



// // fake Duff's Device
// void
// duff_memclr(char* p, unsigned int size)
// {
//     int n = (size + 7) / 8 ;

//     switch (size % 8) {
//     case 0 : do{    *p++    = 0;
//     case 7 :        *p++    = 0;
//     case 6 :        *p++    = 0;
//     case 5 :        *p++    = 0;
//     case 4 :        *p++    = 0;
//     case 3 :        *p++    = 0;
//     case 2 :        *p++    = 0;
//     case 1 :        *p++    = 0;
//                 } while ( -- n > 0 );
//     }
// }

// // Duff's Device
// void
// duff_memcpy(char* to, char* from, unsigned int size)
// {
//     int n = (size + 7) / 8 ;

//     switch (size % 8) {
//     case 0 :  do{   *to++   = *from++;
//     case 7 :        *to++   = *from++;
//     case 6 :        *to++   = *from++;
//     case 5 :        *to++   = *from++;
//     case 4 :        *to++   = *from++;
//     case 3 :        *to++   = *from++;
//     case 2 :        *to++   = *from++;
//     case 1 :        *to++   = *from++;
//                 } while ( -- n > 0 );
//     }
// }

#define BigLittleSwap16(A)  ( (((unsigned short)(A) & 0xff00) >> 8)     |      \
                              (((unsigned short)(A) & 0x00ff) << 8) )
 
#define BigLittleSwap32(A)  ( (((unsigned int)(A) & 0xff000000) >> 24)  |      \
                              (((unsigned int)(A) & 0x00ff0000) >> 8)   |      \
                              (((unsigned int)(A) & 0x0000ff00) << 8)   |      \
                              (((unsigned int)(A) & 0x000000ff) << 24) )

unsigned int
_ntoh32(unsigned int ui32)
{
#ifdef __LITTLE_ENDIAN_BITFIELD
    return BigLittleSwap32(ui32);
#else
    return ui32;
#endif
}


unsigned short
_ntoh16(unsigned short ui16)
{
#ifdef __LITTLE_ENDIAN_BITFIELD
    return BigLittleSwap16(ui16);
#else
    return ui32;
#endif
}


unsigned int
_iptonetint32(char* ip)
{
    union
    {
        unsigned int    ui32;
        unsigned char   uc8[4];
    }r;

    sscanf(ip, "%u.%u.%u.%u", r.uc8, r.uc8 + 1, r.uc8 + 2, r.uc8 + 3);
    // _DEBUG_LOG("===%s, %08x\n", ip, r.ui32);

    return r.ui32;
}


unsigned char*
_netint32toip(unsigned int ui32)
{
    union
    {
        unsigned int    ui32;
        unsigned char   uc8[4];
    }r;

    static unsigned char    ip[16]  = {0};
    r.ui32  = ui32;
    sprintf(ip, "%u.%u.%u.%u", r.uc8[0], r.uc8[1], r.uc8[2], r.uc8[3]);
    // _DEBUG_LOG("=========%s, %08x\n", ip, r.ui32);

    return ip;
}


int
__log_out_null(char* t, ...)
{
    return 0;
}


int
__send_package_null(const unsigned char* pkt, const unsigned int pkt_size)
{
    ___MESSAGE_LOG__("[!] __send_package_null !!!");
    return FALSE;
}


int
net_state_init( const unsigned char *interface,
                const unsigned char *mac,
                const unsigned int  ip_netint32,
                const unsigned int  ip_mask,
                const unsigned int  ip_route_netint32)
{
    int len     = 0;

    if(!interface || !mac || !ip_netint32 || !ip_mask || !ip_route_netint32)
    {
        _MESSAGE_OUT("[!] net_state_init failed, check network!\n");
        return false;
    }

    net_info    = malloc( sizeof(struct _net_state) );
    memset(net_info, 0, sizeof(struct _net_state));

    len                     = strlen(interface);
    net_info->net_interface = malloc(len + 1);
    memset(net_info->net_interface, 0, len + 1);
    memcpy(net_info->net_interface, interface, len);

    memcpy(net_info->mac, mac, 6);

    net_info->ip_netint32       = ip_netint32;
    net_info->ip_mask           = ip_mask;
    net_info->ip_route_netint32 = ip_route_netint32;

    net_info->d_arr_max         = ~(_ntoh32(ip_mask)) + 1;

    net_info->d_arr = malloc(sizeof(struct _device_info) * net_info->d_arr_max);
    memset(net_info->d_arr, 0, sizeof(struct _device_info)*net_info->d_arr_max);

    return true;
}


LOG_OUT_FUN         _log_out        = __log_out_null;
SEND_PACKAGE_FUN    _send_package   = __send_package_null;
struct _net_state   *net_info       = 0;