#include "raspberry.h"
#include "io_ports.h"
#include "uart.h"
#include "state_machine.h"

void raspberry_power_on(void) {
    system_power_on();
    state_set_raspberry(STATE_BOOTING);
}

void raspberry_power_off(void) {
    //uart_send_command(COMMAND_SHUTDOWN);
    raspberry_power_off_gpio();
    state_set_raspberry(STATE_SHUTTING_DOWN);
}
