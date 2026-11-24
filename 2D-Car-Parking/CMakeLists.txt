cmake_minimum_required(VERSION 3.10)
project(2D_Car_Parking CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCES
    main.cpp
    Game.cpp
    PL_DataStructures.cpp
    OOPS_Vehicle.cpp
    OOPS_Car.cpp
    OOPS_Obstacle.cpp
    OOPS_ParkingLot.cpp
    COA_Processor.cpp
    CG_Renderer.cpp
)

set(HEADERS
    Game.h
    PL_DataStructures.h
    OOPS_Vehicle.h
    OOPS_Car.h
    OOPS_Obstacle.h
    OOPS_ParkingLot.h
    COA_Processor.h
    CG_Renderer.h
)

add_executable(2D_Car_Parking ${SOURCES} ${HEADERS})

if (WIN32)
    target_link_libraries(2D_Car_Parking
        ${CMAKE_CURRENT_SOURCE_DIR}/freeglut.dll
        opengl32
        glu32
        gdi32
        winmm
    )
else()
    target_link_libraries(2D_Car_Parking
        glut
        GL
        GLU
        m
    )
endif()
