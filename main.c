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
    eUartCommand cmd;
    io_ports_init();
    uart_init();
    state_machine_init();
    led_set_state(LED_STATE_POWERED_OFF);

    // Work around a Caterian bug (bootloader bug), that does
    // not disable the USB interrupt
    USBCON=0;

    sei();

    for(;;) {
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

            button_handled();
        }
        if(gpio_get_raspberry_state() == STATE_POWERED_OFF && state_get_raspberry() == STATE_SHUTTING_DOWN) {
            camera_power_off();
            //light_power_off();
            system_power_off();
            state_set_raspberry(STATE_POWERED_OFF);
            led_set_state(LED_STATE_POWERED_OFF);
        }
        if(gpio_get_raspberry_state() == STATE_RUNNING && state_get_raspberry() == STATE_BOOTING) {
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
