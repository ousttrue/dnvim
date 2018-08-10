CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)
PROJECT(dnvim VERSION 0.1.0)

FILE(GLOB SRC
    tiny-process-library/process.hpp
    tiny-process-library/process.cpp
    tiny-process-library/process_win.cpp
    msgpackpp/include/msgpackpp.h
    *.cpp
    *.h
    ) 

ADD_EXECUTABLE(${PROJECT_NAME} ${SRC})

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC
	tiny-process-library
    msgpackpp/include
    plog/include
    )

