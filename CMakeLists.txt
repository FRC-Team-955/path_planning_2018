# Export all of the compile commands so I can use them in vim
SET( CMAKE_EXPORT_COMPILE_COMMANDS 1 )

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0 FATAL_ERROR)

# Project
PROJECT(MENU)

FIND_PACKAGE(Curses REQUIRED)
INCLUDE_DIRECTORIES(${CURSES_INCLUDE_DIR})

FIND_PACKAGE(OpenCV)

FIND_PACKAGE(GLUT)
include_directories(${GLUT_INCLUDE_DIR})
link_directories(${GLUT_LIBRARY_DIRS})
add_definitions(${GLUT_DEFINITIONS})

FIND_PACKAGE(OpenGL)
include_directories(${OPENGL_INCLUDE_DIRS})
link_directories(${OPENGL_LIBRARY_DIRS})
add_definitions(${OPENGL_DEFINITIONS})

# Compile source files
FILE(GLOB SOURCES "src/*.cpp")
INCLUDE_DIRECTORIES("./include")

# Use c++11 and clang
SET (CMAKE_CXX_COMPILER "/usr/bin/clang++")
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ")

# Make all of the sources executable
ADD_EXECUTABLE(MENU ${SOURCES})

# Link libraries into the executable
TARGET_LINK_LIBRARIES(MENU ${CURSES_LIBRARIES} ${OpenCV_LIBS} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
