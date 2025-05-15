cmake_minimum_required(VERSION 3.0)

# Project
project(pd_serial)

option(DOOM_USE_SINGLE_HEADER "Use single header instead of doom source" OFF)

# add_definitions(-DDOOM_FAST_TICK) 70FPS
add_definitions(
    -DDOOM_IMPLEMENT_PRINT
    -DDOOM_IMPLEMENT_MALLOC
    -DDOOM_IMPLEMENT_FILE_IO
    #-DDOOM_IMPLEMENT_SOCKETS
    -DDOOM_IMPLEMENT_GETTIME
    -DDOOM_IMPLEMENT_EXIT
    -DDOOM_IMPLEMENT_GETENV # remove later
)

# Sources
file(GLOB dpserial_src_files serial.c)
if (DOOM_USE_SINGLE_HEADER)
    add_definitions(-DDOOM_EXAMPLE_USE_SINGLE_HEADER)
else()
    file(GLOB dpserial_doom_src_files ../../src/DOOM/*.*)
    list(APPEND dpserial_includes PUBLIC ../../src/)
endif()

# Source groups
source_group("Example" FILES ${dpserial_src_files})
source_group("DOOM" FILES ${dpserial_doom_src_files})

# Executable
add_executable(${PROJECT_NAME} ${dpserial_src_files} ${dpserial_doom_src_files})

# Lib/Headers
target_include_directories(${PROJECT_NAME} PUBLIC ${dpserial_includes})
target_link_libraries(${PROJECT_NAME} PUBLIC ${dpserial_libs})
