cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME firn)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    Core/Src/cs43l22.c
    Core/Src/synth.c
    Core/Src/envelope.c
    Core/Src/onepole.c
    Core/Src/svf.c
    Core/Src/tapedelay.c
    Core/Src/shape.c
    Core/Src/shapelfo.c
    Core/Src/midi.c
    Core/Src/softclip.c
    Core/Src/mcp23017.c
    Core/Src/waveleds.c
)

# The audio code runs in the I2S interrupt and has to be fast enough also in a
# Debug build. If it takes longer than the buffer plays, the CPU never leaves
# the interrupt and nothing else runs. So always optimize these files.
set_source_files_properties(
    Core/Src/synth.c
    Core/Src/shape.c
    Core/Src/svf.c
    Core/Src/onepole.c
    Core/Src/tapedelay.c
    Core/Src/shapelfo.c
    Core/Src/envelope.c
    Core/Src/softclip.c
    Core/Src/mcp23017.c
    PROPERTIES COMPILE_OPTIONS "-O2"
)

target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    # Add user defined libraries
    m
)