#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "io_ports.h"
#include "uart.h"
#include "state_machine.h"
#include "relais.h"
#include "raspberry.h"

static bool m_shutdown_after_boot = false;

int main(void) {
    //eUartCommand cmd;
    io_ports_init();
    uart_init();
    state_machine_init();
    led_set_state(LED_STATE_POWERED_OFF);

    // Work around a Caterina bug (bootloader bug), that does
    // not disable the USB interrupt
    USBCON=0;

    sei();

    for(;;) {
        // Handle button press
        if(button_pressed()) {
            eState raspiState = state_get_raspberry();
            switch(raspiState) {
            case STATE_POWERED_OFF:
                raspberry_power_on();
                camera_power_on();
                light_power_on();
                printer_power_on();
                led_set_state(LED_STATE_BOOTING);
                break;
            case STATE_BOOTING:

                break;
            case STATE_RUNNING:
                raspberry_trigger();
                break;
            case STATE_SHUTTING_DOWN:
                break;
            }

            button_handled();
        }

        if(button_middle_pressed()) {
            eState raspiState = state_get_raspberry();
            switch(raspiState) {
            case STATE_POWERED_OFF:
                break;
            case STATE_BOOTING:
                m_shutdown_after_boot = true;
                led_set_state(LED_STATE_SHUTTING_DOWN);
                break;
            case STATE_RUNNING:
                raspberry_power_off();
                //camera_power_off();
                light_power_off();
                //printer_power_off();
                led_set_state(LED_STATE_SHUTTING_DOWN);
                break;
            case STATE_SHUTTING_DOWN:
                break;
            }
            button_middle_handled();
        }

        if(button_long_pressed()) {
            camera_power_off();
            printer_power_off();
            light_power_off();
            system_power_off();
            state_set_raspberry(STATE_POWERED_OFF);
            led_set_state(LED_STATE_POWERED_OFF);
            button_long_handled();
        }

        // Handle the "gpio-poweroff" GPIO, i.e. Raspberry
        // reports "powered down"
        if(gpio_get_raspberry_state() == STATE_POWERED_OFF &&
                (state_get_raspberry() == STATE_SHUTTING_DOWN ||
                 state_get_raspberry() == STATE_RUNNING)) {
            camera_power_off();
            printer_power_off();
            // The user has initiated the shutdown from the menu
            // we need to take care of a bit more
            if(state_get_raspberry() == STATE_RUNNING) {
                light_power_off();
            }
            system_power_off();
            state_set_raspberry(STATE_POWERED_OFF);
            led_set_state(LED_STATE_POWERED_OFF);
        }

        // Handle the userspace Raspberry GPIO "boot complete"
        if(gpio_get_raspberry_state() == STATE_RUNNING &&
                state_get_raspberry() == STATE_BOOTING) {
            if(m_shutdown_after_boot) {
                raspberry_power_off();
                m_shutdown_after_boot = false;
            } else {
                state_set_raspberry(STATE_RUNNING);
                led_set_state(LED_STATE_RUNNING);
            }
        }

        /*
        cmd = uart_handle_command();
        switch(cmd) {
        case COMMAND_BOOTUP_COMPLETE:
            if(m_shutdown_after_boot) {
                raspberry_power_off();
            } else {
                state_set_raspberry(STATE_RUNNING);
                led_set_state(LED_STATE_RUNNING);
            }
            break;
        case COMMAND_SHUTDOWN:
            led_set_state(LED_STATE_SHUTTING_DOWN);
            state_set_raspberry(STATE_SHUTTING_DOWN);
            if(state_get_printer() != STATE_POWERED_OFF) {
                printer_power_off();
            }
            light_power_off();
            camera_power_off();
            break;
        case COMMAND_NO_COMMAND:
            break;
        }
        */
    }

    return 0;
}
