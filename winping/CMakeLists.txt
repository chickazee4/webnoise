add_library(winping STATIC winping.c)
target_include_directories(winping PUBLIC ../include)
target_link_libraries(winping wsock32 ws2_32)