cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(entropy C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


# Initialize the SDK
pico_sdk_init()

set(FAMILY rp2040)
set(BOARD pico_sdk)

add_executable(entropy
        dev_lowlevel.c
	usb_protocol.c
	queue.c
        )

target_link_libraries(entropy PRIVATE pico_stdlib hardware_resets hardware_irq hardware_pwm hardware_adc pico_multicore)

pico_add_extra_outputs(entropy)

