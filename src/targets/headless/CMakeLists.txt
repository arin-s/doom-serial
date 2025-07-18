cmake_minimum_required(VERSION 3.0)

# Project
project(pd_serial)

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
        --rename-section .data=.data,alloc,load,readonly,data,contents
        "--add-symbol=doom_wad_data_start=.data:0,global"
        "--add-symbol=doom_wad_data_end=.data:${WAD_FILE_SIZE},global"
    ARGS
        ${WAD_FILE} ${WAD_OBJECT_FILE}
    DEPENDS ${WAD_FILE}
)
add_library(doom_wad_data OBJECT ${WAD_OBJECT_FILE})
option(DOOM_USE_SINGLE_HEADER "Use single header instead of doom source" OFF)

# add_definitions(-DDOOM_FAST_TICK) 70FPS
add_definitions(
    -D__LINUX__
)

# Sources
set(dpserial_src_files
    headless.cpp
    linux_uart.cpp
    ../../../thirdparty/JPEGENC/src/JPEGENC.cpp
)

file(GLOB dpserial_doom_src_files ../../*.*)
list(APPEND dpserial_includes PUBLIC ../../)
list(APPEND dpserial_includes PUBLIC ../../../thirdparty/JPEGENC/src)

# Source groups
source_group("Example" FILES ${dpserial_src_files})
source_group("DOOM" FILES ${dpserial_doom_src_files})

# Executable
add_executable(${PROJECT_NAME} ${dpserial_src_files} ${dpserial_doom_src_files} ${WAD_OBJECT_FILE})

# Lib/Headers
target_include_directories(${PROJECT_NAME} PUBLIC ${dpserial_includes})
target_link_options(${PROJECT_NAME} PRIVATE "-Wl,-z,noexecstack")