#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>

typedef struct _iphdr
{
    unsigned int    h_len : 4;
    unsigned int    version : 4;
    unsigned char   tos;
    unsigned short  total_len;
    unsigned short  ident;
    unsigned short  flags;
    unsigned char   ttl;
    unsigned char   protocol;
    unsigned short  checksum;
    
    unsigned int    srcip;
    unsigned int    destip;
} iphdr;

typedef struct _ipoptionhdr
{
    unsigned char   code;
    unsigned char   len;
    unsigned char   ptr;
    unsigned long   addr[9];
} ipoptshdr;

typedef struct _icmphdr
{
    unsigned char   i_type;
    unsigned char   i_code;
    unsigned short  i_cksum;
    unsigned short  i_id;
    unsigned short  i_seq;
} icmphdr;

#define ICMP_ECHO       8
#define ICMP_ECHOREPLY  0
#define ICMP_MIN        8
#define DEF_PACKET_SIZE 32
#define MAX_PACKET_SIZE 1024
#define MAX_IP_HDR_SIZE 60

unsigned short cksum(unsigned short *, size_t);
icmphdr generate_icmp(char *, size_t);
ipoptshdr *decode_ipopts(char *, size_t);
icmphdr *decode_icmphdr(char *, size_t, struct sockaddr_in *);
int ping(char *);