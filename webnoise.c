/* Copyright 2021 chickazee4
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#ifdef WINDOWS
#include <windows.h>
#endif

#ifndef WINDOWS
#include <libgen.h>
#include <unistd.h>
#endif
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "wping.h"
#include "config.h"

/* cross system line ending compat */
#ifdef WINDOWS
const char *newline = "\r\n";
#else
const char *newline = "\n";
#endif

char *progname;
int mflag = 0;
int nflag = 0;
int realargv = 0;

char **addrcache;
long cachenum = 0;
long cacheind = 0;

struct timespec intrt;
struct timespec *intrtp; 

/* Print the usage information */
void
usage()
{
    printf(
"Usage: %s [-hmn] [INPUT]%s\
Generate random traffic to specific addresses at a bandwidth-dependent interval.%s\
INPUT may be a file containing a list of addresses, 1 per line, or the addresses themselves delimited by spaces.%s", 
progname, newline, newline, newline
    );
}

/* Print help info */
void
help()
{
    printf("webnoise 1.0 (Apache License v2)%s", newline);
    usage();
    printf(
"%sFlags:%s\
 -h\tshow this help%s\
 -m\twait a minute between pings instead of 50 milliseconds%s\
 -n\ttreat input as strings, even if they exist as a file%s\
Note that addresses cannot exceed 255 characters in length and should not contain a leading http:// or https:// marker.%s",
newline, newline, newline, newline, newline, newline, newline /* there has to be a better way to do this LOL */
    );
}

/* sanitize string of extraneous formatting elements in place */
char *
strsan(char *in)
{
    int i,j;
    i = 0;
    while(i<strlen(in))
    {
        if (in[i] == '\t' || in[i] == '\r' || in[i] == '\n' || in[i] == ' ')
        {
            for (j = i; j < strlen(in); j++)
                in[j] = in[j + 1];
        }
        else i++;
    }
    return in;
}

/* Add a string to the cache to be processed */
void
cachestr(char *item)
{
    strsan(item);
    size_t cnum = strlen(item) + 1; 
    if (addrcache == NULL) {
        addrcache = malloc(1 * sizeof(char *));
    }
    addrcache[cacheind] = malloc(cnum * sizeof(char));
    strcpy(addrcache[cacheind], item);
    cacheind++;
    cachenum++;
}

/* Break down a file into strings to cache */
void
cachefile(FILE *file) 
{
    char buf[255];
    while(fgets(buf, 255, file)) {
        cachestr((char *)buf);
    }
}

/* Separate files and addresses and process them into the cache */
void
cache(char *in) 
{
#ifdef WINDOWS
    if (fopen(in, "r") != NULL && nflag == 0) { /* i don't love having to do it this way but i'm not familiar enough with c on w*ndows to know of a better solution */
#else
    if (access(in, F_OK) == 0 && nflag == 0) {
#endif
        FILE *fp;
        fp = fopen(in, "r");
        cachefile(fp);
    } else cachestr(in);
}

/* Start pinging after all caching and options are complete*/
void go() 
{
    int r = 0;
    int seed = 0;
    clock_t c;
    for (;;) {
        if (cachenum == 1) r = 1;
        else {
            c = clock();
            seed = (double)c / CLOCKS_PER_SEC * 1000;
            srand(seed);
            r = rand() % (cachenum - 1);
        }
        wping(addrcache[r]);
#ifdef WINDOWS
        if (mflag > 0) {
            Sleep(60000);
        } else Sleep(5);
#else
        if (mflag > 0) {
            sleep(60);
        } else nanosleep(intrtp, NULL);
#endif
    }
}

int
main(int argv,
     char **argc)
{
#ifdef WINDOWS
    progname = argc[0];
#else
    progname = basename(argc[0]);
#endif
    realargv = argv - 1;
    intrtp = &intrt;
    intrtp->tv_nsec = 50000000; 
    if (argv > 1) {
        for (int i = 1; i < argv; i++) {
            if (argc[i][0] == '-') {
                realargv--;
                for (int ib = 1; argc[i][ib] != '\0'; ib++) {
                    switch (argc[i][ib]) {
                        case 'h':
                            help();
                            exit(0);
                            break;
                        case 'm':
                            mflag++;
                            break;
                        case 'n':
                            nflag++;
                            break;
                        default:
                            usage();
                            exit(1);
                            break;
                    }
                }
            } else {
                cache(argc[i]);
            }
        }
        go();
    }
    else {
        usage();
        exit(1);
    }
}