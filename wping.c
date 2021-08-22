#include <errno.h>
#include <stdio.h>
#include "wping.h"
#include "config.h"
#ifdef WINDOWS
#include "winping.h"
void
wping(char *addr)
{
    int i;
    if ((i = ping(addr)) != 0) {
        switch (i) { /* these are not unix exit codes they're just chronological by where they show up in program flow */
            case 1:
                fprintf(stderr, "webnoise: error: WSA startup failed (%d). continuing anyway.\r\n", errno);
                break;
            case 2:
                fprintf(stderr, "webnoise: error: could not create socket (%d). continuing anyway.\r\n", errno);
                break;
            case 3:
                fprintf(stderr, "webnoise: error: could not format socket (%d). continuing anyway.\r\n", errno);
                break;
            case 4:
                fprintf(stderr, "webnoise: error: could not get host (%d). continuing anyway.\r\n", errno);
                break;
            case 5:
                fprintf(stderr, "webnoise: error: could not allocate memory (%d). continuing anyway.\r\n", errno);
                break;
            case 6:
                fprintf(stderr, "webnoise: error: could not ping (%d). continuing anyway.\r\n", errno);
                break;
            default:
                fprintf(stderr, "webnoise: unknown error (%d). this is very bad news and indicates likely memory issues. now terminating.", errno);
                break;
        }
    }
}
#else
#include "oping.h"
void
wping(char *addr)
{
    pingobj_t *pp = ping_construct();
    ping_host_add(pp, addr);
    ping_send(pp);
    ping_destroy(pp);
}
#endif