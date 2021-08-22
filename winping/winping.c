/* oping doesn't work on windows/mingw because windows has a separate IP implementation - here's a ping implementation using winsock2
 * this is a careless implementation - these are garbage pings and we don't care if we get the wrong response, we just want traffic. as such,
 * we don't have much error handling; don't try to use this outside of this project unless you're doing something similar, or you want to
 * add error checks.
 */
#include <stdio.h>
#include <memory.h>
#include "winping.h"

unsigned short 
cksum(unsigned short *buf, 
      size_t sz) 
{
    unsigned long cs = 0;
    while (sz > 1)
    {
        cs += *buf++;
        sz -= sizeof(unsigned short); 
    }
    if (sz) cs += *(unsigned char *)buf;
    cs = (cs >> 16) + (cs & 0xffff);
    cs += (cs >> 16);
    return (unsigned short)(~cs);
}

icmphdr
generate_icmp(char *in, 
              size_t sz)
{
    icmphdr *ih;
    char *dat;
    
    ih = (icmphdr *)in;
    ih->i_type = ICMP_ECHO;
    ih->i_code = 0;
    ih->i_id = 39398; /* arbitrary id */
    ih->i_cksum = 0;
    ih->i_seq = 0;

    dat = in + sizeof(icmphdr);
    memset(dat, 0, sz);
    
    return *ih;
}

ipoptshdr * 
decode_ipopts(char *in, 
              size_t sz)
{
    ipoptshdr *io;
    IN_ADDR ia;
    HOSTENT *host;
    for (int i = 0; i < (io->ptr / 4) - 1; i++) 
    {
        ia.S_un.S_addr = io->addr[i];
        host = gethostbyaddr((char *)&ia.S_un.S_addr, sizeof(ia.S_un.S_addr), AF_INET);
    }
    return io;
}

icmphdr *
decode_icmphdr(char *buf, 
               size_t sz,
               struct sockaddr_in *in)
{
    iphdr *ih;
    icmphdr *ich;
    unsigned short ihlen;
    unsigned int tick;
    static int icmpct = 0;

    ih = (iphdr *)buf;
    ihlen = ih->h_len * 4;
    tick = GetTickCount();

    if (ihlen == MAX_IP_HDR_SIZE && (!icmpct)) {
        decode_ipopts(buf, sz);
    }
    ich = (icmphdr *)(buf + ihlen);
    icmpct++;
    return ich;
}

int 
ping(char *dest)
{
    WSADATA wd;
    SOCKET rs = INVALID_SOCKET;
    struct sockaddr_in to, from;
    int bread, fromlen = sizeof(from), timeout = 1000, ret;
    char *icmpdata, *recvbuf;
    unsigned int addr = 0;
    unsigned short seq_no = 0;
    struct hostent *hp;
    ipoptshdr io;
    if (WSAStartup(MAKEWORD(2, 2), &wd) != 0) return 1;
    if ((rs = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET) return 2;
    if ((bread = setsockopt(rs, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout))) == SOCKET_ERROR) return 3;
    memset(&to, 0, sizeof(to));
    to.sin_family = AF_INET;
    if ((to.sin_addr.s_addr = inet_addr(dest)) == INADDR_NONE) {
        if ((hp = gethostbyname(dest)) != NULL) {
            memcpy(&(to.sin_addr), hp->h_addr, hp->h_length);
            to.sin_family = hp->h_addrtype;
        } else return 4;
    }
    int datasize = sizeof(icmphdr);
    icmpdata = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, MAX_PACKET_SIZE);
    recvbuf = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, MAX_PACKET_SIZE);
    if (!icmpdata) {
        return 5;
    }
    memset(icmpdata, 0, MAX_PACKET_SIZE);
    generate_icmp(icmpdata, datasize);
    icmphdr *ih = (icmphdr *)icmpdata;
    ih->i_cksum = 0;
    int bwrote;
    for (int i = 0; i < 4; i++) {
        ih->i_seq = seq_no++;
        ih->i_cksum = cksum((unsigned short *)icmpdata, datasize);
        if ((bwrote = sendto(rs, icmpdata, datasize, 0, (struct sockaddr *)&to, sizeof(to))) == SOCKET_ERROR)
            return 6;
        bread = recvfrom(rs, recvbuf, MAX_PACKET_SIZE, 0, (struct sockaddr*)&from, &fromlen);
        decode_icmphdr(recvbuf, bread, &from);
    }
    if (rs != INVALID_SOCKET) closesocket(rs);
    HeapFree(GetProcessHeap(), 0, recvbuf);
    HeapFree(GetProcessHeap(), 0, icmpdata);
    WSACleanup();
    return 0;
}