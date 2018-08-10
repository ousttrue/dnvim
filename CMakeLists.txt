CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)
PROJECT(dnvim VERSION 0.1.0)

FILE(GLOB SRC
    tiny-process-library/process.hpp
    tiny-process-library/process.cpp
    tiny-process-library/process_win.cpp
    msgpackpp/include/msgpackpp.h
    *.cpp
    *.h
    resource.rc
    ) 

ADD_EXECUTABLE(${PROJECT_NAME} WIN32 ${SRC})

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC
	tiny-process-library
    msgpackpp/include
    plog/include
    )

TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PUBLIC
    UNICODE
    _UNICODE
    NOMINMAX
    )

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    D3D11
    d3dcompiler
    dxguid
    )

