cmake_minimum_required(VERSION 3.5.1)
project(main)  


ADD_LIBRARY(LibsModule main.cpp)

target_link_libraries(LibsModule pthread)
target_link_libraries(LibsModule X11)
target_link_libraries(LibsModule png)
target_link_libraries(LibsModule stdc++fs)
target_link_libraries(LibsModule GL)



ADD_EXECUTABLE(main main.cpp)
target_link_libraries(main LibsModule)
