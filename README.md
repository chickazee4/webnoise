# webnoise

webnoise is a simple utility to generate arbitrary DNS/ping traffic to a user-supplied list of addresses, creating network noise and confusing packet sniffers. 

## running

webnoise requires no more than a standard C runtime to run. it should be invoked from a terminal. additional information on invocation can be found by running `webnoise -h`.

you may need to run it as superuser/administrator in order to ping correctly on some systems.

keep in mind that behavior will differ between unixoids and windows. windows operates significantly more slowly due to differences in code and therefore is less overwhelming. if you want more disruptive performance and have to be on windows, use the linux version under WSL or something.

## building

building webnoise requires a capable libc, a C compiler, cmake, and standard unix utilities. most major libc's can do the job for the actual program, as webnoise is engineered to be cross-platform, but the bundled [liboping](https://github.com/octo/liboping) might require more work. it is tested to build on both gcc and clang and should build on most other modern compilers. 

to build & install on most unixoids, including linux, macOS with xtools, openindiana/solaris, and BSDs:

    mkdir build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make && make install
    
in the event that you have to compile for some reason on early versions of solaris (after 2.5.1 this should not be necessary), you need an additional option:

    mkdir build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release -DSOLARIS=ON
    make && make install

on windows, you will need visual studio, visual c, and visual studio cmake tools. run the following from a visual studio developer command prompt in the webnoise source tree:

    mkdir build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 16 2019" -DCMAKE_C_FLAGS="-D_AMD64 -DWINDOWS"
    MSBuild.exe webnoise.sln

replace -D_AMD64 with -D_X86 for 32-bit builds and -D_IA64 for Itanium.

it may also be possible to build with MinGW, but i have not tested that. 

## licensing
webnoise and winping are licensed under the open source [Apache 2.0](http://www.apache.org/licenses/LICENSE-2.0) license, included under LICENSE. the library [liboping](https://github.com/octo/liboping) is bundled in a stripped-down and CMake-compatible form with several changes to provide pinging functionality on unixoids and is licensed under the [LGPL v2.1](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.en.html).

## disclaimer
using this tool improperly may create undue stress on small servers. please do not send traffic to small websites using webnoise unless you enable the -m flag to reduce the amount of pings generated, and use larger website lists to better distribute traffic. i am not liable for any damage caused by the use of this tool.  
