/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "queue.h"
#include "usb_protocol.h"

#include <stdio.h>

#include "hardware/pwm.h"
#include "hardware/adc.h"

#include "pico/multicore.h"

#define RED 10
#define GREEN 11
#define BLUE 12
#define ANALOG 26
#define CHARGE_PUMP1 4
#define CHARGE_PUMP2 3


#define MAXPACKETSIZE 64



void setup_PWM(uint gpio) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 32.0f);  // Set clock divider
    pwm_config_set_wrap(&config, 49);      // Set PWM period (TOP)

    pwm_init(slice_num, &config, true);
    pwm_set_gpio_level(gpio, 39);          // Set duty cycle (e.g., 80%)
    // 78 kHz with 32 prescale Divider
}

void readValues()
{
    gpio_put(RED, 1);
    for (;;)
        enqueue(adc_read());
}

void setup() {
    stdio_init_all();

    gpio_init(RED);
    gpio_init(GREEN);
    gpio_init(BLUE);
    gpio_set_dir(RED, GPIO_OUT);
    gpio_set_dir(GREEN, GPIO_OUT);
    gpio_set_dir(BLUE, GPIO_OUT);

    gpio_put(RED, 0);
    gpio_put(GREEN, 0);
    gpio_put(BLUE, 0);

    adc_init();
    adc_gpio_init(ANALOG);
    adc_select_input(0); // Assuming ADC0 is used

    setup_PWM(CHARGE_PUMP1);
    setup_PWM(CHARGE_PUMP2);

    multicore_launch_core1(readValues);
}

int main(void) {
    setup();
    stdio_init_all();
    usb_device_init();
    
    gpio_put(BLUE, 1);
    while (!configured) {
        sleep_ms(1);
    }
    
    gpio_put(GREEN, 1);
    usb_start_transfer(usb_get_endpoint_configuration(EP1_OUT_ADDR), NULL, MAXPACKETSIZE);
    uint64_t i =0;

    while (1) 
    {
        struct message m;
        
        if(dequeue(&m.buf,sizeof m))
        {
            usb_start_transfer(usb_get_endpoint_configuration(EP2_IN_ADDR), (uint8_t *)&m, sizeof m);
            
            // if(i++ >= 10)
            //     gpio_put(GREEN, 1);

        }
        
    
        gpio_put(RED, 0);
        gpio_put(GREEN, 0);
        gpio_put(BLUE, 0);
        
    }
}