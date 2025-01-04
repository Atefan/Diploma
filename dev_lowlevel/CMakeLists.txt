add_executable(dev_lowlevel
        dev_lowlevel.c
	usb_protocol.c
	queue.c
        )

target_link_libraries(dev_lowlevel PRIVATE pico_stdlib hardware_resets hardware_irq hardware_pwm hardware_adc pico_multicore)

pico_add_extra_outputs(dev_lowlevel)

# add url via pico_set_program_url
example_auto_set_url(dev_lowlevel)
