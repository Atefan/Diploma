cmake_minimum_required(VERSION 3.13)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(blink_project)

# Initialize the SDK
pico_sdk_init()

# Add the executable
add_executable(blink blink.c)

# Link the Pico standard library
target_link_libraries(blink pico_stdlib)

# Enable extra outputs (e.g. UF2)
pico_add_extra_outputs(blink)