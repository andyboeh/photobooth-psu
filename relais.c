#include "relais.h"
#include "iomacro.h"
#include "photobooth-psu.h"
#include <util/delay.h>
#include "state_machine.h"

void printer_power_on(void) {
    RESET(PIN_RELAIS1);
    _delay_ms(1000);
    SET(PIN_RELAIS1);
    state_set_printer(STATE_RUNNING);
}

void printer_power_off(void) {
    RESET(PIN_RELAIS1);
    _delay_ms(1000);
    SET(PIN_RELAIS1);
    state_set_printer(STATE_POWERED_OFF);
}

void camera_power_on(void) {
    RESET(PIN_RELAIS2);
}

void camera_power_off(void) {
    SET(PIN_RELAIS2);
}

void light_power_on(void) {
    RESET(PIN_RELAIS3);
}

void light_power_off(void) {
    SET(PIN_RELAIS3);
}
