cmake_minimum_required(VERSION 3.0)

# Project
project(pd_sdl_example)

# embed the wad file
set(WAD_FILE ${CMAKE_SOURCE_DIR}/doom1.wad)
set(WAD_OBJECT_FILE ${CMAKE_CURRENT_BINARY_DIR}/shareware.o)
execute_process(
    COMMAND stat -c%s ${WAD_FILE}
    OUTPUT_VARIABLE WAD_FILE_SIZE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
add_custom_command(
    OUTPUT ${WAD_OBJECT_FILE}
	COMMAND objcopy
        --input-target binary
        --output-target elf64-x86-64
        --binary-architecture i386
        --rename-section .data=.rodata,alloc,load,readonly,data,contents
        "--add-symbol=doom_wad_data_start=.rodata:0,global"
        "--add-symbol=doom_wad_data_end=.rodata:${WAD_FILE_SIZE},global"
    ARGS
        ${WAD_FILE} ${WAD_OBJECT_FILE}
    DEPENDS ${WAD_FILE}
)
add_library(doom_wad_data OBJECT ${WAD_OBJECT_FILE})
option(DOOM_USE_SINGLE_HEADER "Use single header instead of doom source" OFF)

# add_definitions(-DDOOM_FAST_TICK) 70FPS
add_definitions(
    -DDOOM_IMPLEMENT_PRINT
    -DDOOM_IMPLEMENT_MALLOC
    #-DDOOM_IMPLEMENT_FILE_IO
    #-DDOOM_IMPLEMENT_SOCKETS
    -DDOOM_IMPLEMENT_GETTIME
    -DDOOM_IMPLEMENT_EXIT
    -DDOOM_IMPLEMENT_GETENV # remove later
    -D__LINUX__
)

# Sources
set(dpsdl_src_files
    sdl_example.c
)
file(GLOB dpsdl_doom_src_files ../../src/DOOM/*.*)
list(APPEND dpsdl_includes PUBLIC ../../src/DOOM)
list(APPEND dpsdl_includes PUBLIC ../../src/common)

# SDL
add_definitions(-DSDL_MAIN_HANDLED)
set(HAVE_LIBC ON)
add_subdirectory(../../thirdparty/SDL/ ./thirdparty/SDL/)
list(APPEND dpsdl_libs SDL2-static)
list(APPEND dpsdl_includes PUBLIC ../../thirdparty/SDL/include/)

# Source groups
source_group("Example" FILES ${dpsdl_src_files})
source_group("DOOM" FILES ${dpsdl_doom_src_files})

# Executable
add_executable(${PROJECT_NAME} ${dpsdl_src_files} ${dpsdl_doom_src_files} ${WAD_OBJECT_FILE})

# Work dir
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../")

# For midi output on mac
if (APPLE)
    list(APPEND dpsdl_libs "-framework CoreFoundation")
    list(APPEND dpsdl_libs "-framework AudioToolbox")
    list(APPEND dpsdl_libs "-framework CoreMIDI")
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-F/Library/Frameworks")
endif()

# Lib/Headers
target_include_directories(${PROJECT_NAME} PUBLIC ${dpsdl_includes})
target_link_libraries(${PROJECT_NAME} PUBLIC ${dpsdl_libs})
