# webnoise

webnoise is a simple utility to generate arbitrary DNS/ping traffic to a user-supplied list of addresses, creating network noise and confusing packet sniffers. 

# running

webnoise requires the [liboping](https://github.com/octo/liboping) library to run. it's available in most linux distribution package managers and can be compiled from source. it should be invoked from a terminal. additional information on invocation can be found by running `webnoise -h`.

# building

building webnoise requires [liboping](https://github.com/octo/liboping) and a capable libc. most major libc's can do the job, as webnoise is specifically engineered to be cross-platform. it is tested to build on both gcc and clang and should build on most other modern compilers.

    mkdir build && cd build
    cmake ..
    make && make install

commands may differ by OS. 

# disclaimer

using this tool improperly may create undue stress on small servers. please do not send traffic to small websites using webnoise unless you enable the -m flag to reduce the amount of pings generated, and use larger website lists to better distribute traffic. i am not liable for any damage caused by the use of this tool.  