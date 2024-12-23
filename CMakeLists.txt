cmake_minimum_required(VERSION 3.28)
project(GB_EMU_2024)

set(CMAKE_CXX_STANDARD 23)

add_executable(GB_EMU_2024 main.cpp main.h
        PPU.cpp
        PPU.h)

set(CMAKE_MODULE_PATH  "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")

find_package(SFML REQUIRED system window graphics)
if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(GB_EMU_2024 ${SFML_LIBRARIES})
endif()

file(COPY libs DESTINATION ${CMAKE_BINARY_DIR})

# Copy dlls to build
if(WIN32)
    file(GLOB BINARY_DEP_DLLS "${SFML_INCLUDE_DIR}/*.dll")
    file(COPY ${BINARY_DEP_DLLS} DESTINATION ${CMAKE_BINARY_DIR})
endif()


#


#cmake_minimum_required(VERSION 3.10) # Ensure a compatible CMake version
#project(GB_EMU_2024)
#
## Set the C++ standard
#set(CMAKE_CXX_STANDARD 23)
#
## Specify the SFML directory (adjust the path to match your setup relative to your CMake project)
#set(SFML_DIR "${CMAKE_SOURCE_DIR}/libs/SFML/lib/cmake/SFML")
#
## Add SFML binaries to the runtime directory (for DLLs on Windows)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#file(COPY "${CMAKE_SOURCE_DIR}/libs/SFML/bin"
#        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
#
## Locate SFML using the configuration files
#find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
#
## Include SFML headers
#include_directories(${SFML_INCLUDE_DIRS})
#
## Add your source files
#add_executable(GB_EMU_2024 main.cpp main.h)
#
## Link SFML libraries to your project
#target_link_libraries(GB_EMU_2024 sfml-graphics sfml-window sfml-system)