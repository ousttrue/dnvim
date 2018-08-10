SET(EXE_NAME dnvim)
FILE(GLOB SRC
    ../tiny-process-library/process.hpp
    ../tiny-process-library/process.cpp
    ../tiny-process-library/process_win.cpp
    ../msgpackpp/include/msgpackpp.h
    *.cpp
    *.h
    resource.rc
    ) 

ADD_EXECUTABLE(${EXE_NAME} WIN32 ${SRC})

TARGET_INCLUDE_DIRECTORIES(${EXE_NAME} PUBLIC
	../tiny-process-library
    ../msgpackpp/include
    ../plog/include
    )

TARGET_COMPILE_DEFINITIONS(${EXE_NAME} PUBLIC
    UNICODE
    _UNICODE
    NOMINMAX
    )

TARGET_LINK_LIBRARIES(${EXE_NAME}
    D3D11
    d3dcompiler
    dxguid
    )

